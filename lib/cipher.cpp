#include "cipher.h"

const std::string Cipher::kdfLabel = {"get-subkey"};

const std::unordered_map<std::string, Cipher::CipherInfo>
    Cipher::cipherInfoMap = {
        {"aes-128-cfb", {"AES-128/CFB", 16, 16, Cipher::CipherType::STREAM}},
        {"aes-192-cfb", {"AES-192/CFB", 24, 16, Cipher::CipherType::STREAM}},
        {"aes-256-cfb", {"AES-256/CFB", 32, 16, Cipher::CipherType::STREAM}},
        {"aes-128-ctr", {"AES-128/CTR-BE", 16, 16, Cipher::CipherType::STREAM}},
        {"aes-192-ctr", {"AES-192/CTR-BE", 24, 16, Cipher::CipherType::STREAM}},
        {"aes-256-ctr", {"AES-256/CTR-BE", 32, 16, Cipher::CipherType::STREAM}},
        {"bf-cfb", {"Blowfish/CFB", 16, 8, Cipher::CipherType::STREAM}},
        {"camellia-128-cfb",
         {"Camellia-128/CFB", 16, 16, Cipher::CipherType::STREAM}},
        {"camellia-192-cfb",
         {"Camellia-192/CFB", 24, 16, Cipher::CipherType::STREAM}},
        {"camellia-256-cfb",
         {"Camellia-256/CFB", 32, 16, Cipher::CipherType::STREAM}},
        {"cast5-cfb", {"CAST-128/CFB", 16, 8, Cipher::CipherType::STREAM}},
        {"chacha20", {"ChaCha", 32, 8, Cipher::CipherType::STREAM}},
        {"chacha20-ietf", {"ChaCha", 32, 12, Cipher::CipherType::STREAM}},
        {"des-cfb", {"DES/CFB", 8, 8, Cipher::CipherType::STREAM}},
        {"idea-cfb", {"IDEA/CFB", 16, 8, Cipher::CipherType::STREAM}},
        //#ifndef USE_BOTAN2
        //    // RC2 is not supported by botan-2
        //    {"rc2-cfb", {"RC2/CFB", 16, 8, Cipher::CipherType::STREAM}},
        //#endif
        {"rc4-md5", {"RC4-MD5", 16, 16, Cipher::CipherType::STREAM}},
        {"salsa20", {"Salsa20", 32, 8, Cipher::CipherType::STREAM}},
        {"seed-cfb", {"SEED/CFB", 16, 16, Cipher::CipherType::STREAM}},
        {"serpent-256-cfb", {"Serpent/CFB", 32, 16, Cipher::CipherType::STREAM}}
        //#ifdef USE_BOTAN2
        ,
        {"chacha20-ietf-poly1305",
         {"ChaCha20Poly1305", 32, 12, Cipher::CipherType::AEAD, 32, 16}},
        {"aes-128-gcm",
         {"AES-128/GCM", 16, 12, Cipher::CipherType::AEAD, 16, 16}},
        {"aes-192-gcm",
         {"AES-192/GCM", 24, 12, Cipher::CipherType::AEAD, 24, 16}},
        {"aes-256-gcm",
         {"AES-256/GCM", 32, 12, Cipher::CipherType::AEAD, 32, 16}}
        //#endif
};

std::string Cipher::randomIv(int length) {
    // directly return empty byte array if no need to genenrate iv
    if (length == 0) {
        return std::string();
    }

    Botan::AutoSeeded_RNG rng;
    Botan::secure_vector<Botan::byte> out = rng.random_vec(length);
    return std::string(reinterpret_cast<const char *>(out.data()), out.size());
}

std::string Cipher::randomIv(const std::string &method) {
    CipherInfo cipherInfo = cipherInfoMap.at(method);
    if (cipherInfo.type == AEAD) {
        return std::string(cipherInfo.ivLen, static_cast<char>(0));
    }
    return randomIv(cipherInfo.ivLen);
}

void Cipher::nonceIncrement(unsigned char *n, const size_t nlen) {
    uint_fast16_t c = 1U;
    for (size_t i = 0U; i < nlen; i++) {
        c += static_cast<uint_fast16_t>(n[i]);
        n[i] = static_cast<unsigned char>(c);
        c >>= 8;
    }
}

void Cipher::incrementIv(Botan::Keyed_Filter *m_filter, std::string &m_iv) {
    nonceIncrement(reinterpret_cast<unsigned char *>(&m_iv[0]), m_iv.size());
    m_filter->set_iv(Botan::InitializationVector(
        reinterpret_cast<const Botan::byte *>(m_iv.data()), m_iv.size()));
}

std::string Cipher::md5Hash(const std::string &in) {
    Botan::MD5 md5;
    Botan::secure_vector<Botan::byte> result = md5.process(in);
    return std::string(reinterpret_cast<const char *>(result.data()),
                       result.size());
}

std::string Cipher::deriveAeadSubkey(size_t length,
                                     const std::string &masterKey,
                                     const std::string &salt) {
    std::unique_ptr<Botan::KDF> kdf;
    kdf = std::make_unique<Botan::HKDF>(new Botan::HMAC(new Botan::SHA_160()));
    // std::string salt = randomIv(cipherInfo.saltLen);
    Botan::secure_vector<Botan::byte> skey = kdf->derive_key(
        length, reinterpret_cast<const uint8_t *>(masterKey.data()),
        masterKey.length(), salt, kdfLabel);
    return std::string(reinterpret_cast<const char *>(skey.data()),
                       skey.size());
}

Cipher::Cipher(const std::string method, const std::string password)
    : m_method(method),
      m_cipherInfo(Cipher::cipherInfoMap.at(m_method)),
      m_key(evpBytesToKey(m_cipherInfo, password)) {}

std::string Cipher::update(const std::string &data,
                           std::shared_ptr<Botan::Pipe> m_pipe) {
    return update(data.data(), data.length(), m_pipe);
}

std::string Cipher::update(const char *data, size_t length,
                           std::shared_ptr<Botan::Pipe> m_pipe) {
    if (m_pipe) {
        m_pipe->process_msg(reinterpret_cast<const Botan::byte *>(data),
                            length);
        Botan::secure_vector<Botan::byte> c =
            m_pipe->read_all(Botan::Pipe::LAST_MESSAGE);
        return std::string(reinterpret_cast<const char *>(c.data()), c.size());
    }
    throw std::logic_error("Underlying ciphers are all uninitialised!");
}

std::string Cipher::enc(std::string &data) {
    return enc(data.data(), data.size());
}

std::string Cipher::enc(const char *data, size_t length) {
    if (length <= 0) {
        return std::string();
    }

    std::string header;
    if (!m_pipe_enc) {
        initEnc(header);
    }

    std::string encrypted;
    if (m_cipherInfo.type == Cipher::CipherType::AEAD) {
        uint16_t inLen =
            length > AEAD_CHUNK_SIZE_MASK ? AEAD_CHUNK_SIZE_MASK : length;
        std::string rawLength(AEAD_CHUNK_SIZE_LEN, static_cast<char>(0));
        qToBigEndian(inLen, reinterpret_cast<uint8_t *>(&rawLength[0]));
        std::string encLength = update(rawLength, m_pipe_enc);  // length + tag
        incrementIv(m_filter_enc, m_iv_enc);
        std::string encPayload =
            update(data, inLen, m_pipe_enc);  // payload + tag
        incrementIv(m_filter_enc, m_iv_enc);
        encrypted = encLength + encPayload;
        if (inLen < length) {
            // Append the remaining part recursively if there is any
            encrypted += enc(data + inLen, length - inLen);
        }
    } else {
        encrypted = update(data, length, m_pipe_enc);
    }
    return header + encrypted;
}

std::string Cipher::dec(std::string &data) {
    return dec(data.data(), data.size());
}

std::string Cipher::dec(const char *data, size_t length) {
    if (length <= 0) {
        return std::string();
    }

    std::string out;
    if (!m_pipe_dec) {
        size_t headerLength = 0;
        initDec(reinterpret_cast<const char *>(data), length, headerLength);
        data += headerLength;
        length -= headerLength;
    }

    if (m_cipherInfo.type == Cipher::CipherType::AEAD) {
        // Concatenate the data with incomplete chunk (if it exists)
        std::string chunk =
            m_incompleteChunk +
            std::string(reinterpret_cast<const char *>(data), length);
        data = chunk.data();
        length = chunk.length();
        const char *dataEnd = data + length;

        uint16_t payloadLength = 0;
        if (m_incompleteLength != 0u) {
            // The payload length is already known
            payloadLength = m_incompleteLength;
            m_incompleteLength = 0;
            m_incompleteChunk.clear();
        } else {
            if (dataEnd - data < AEAD_CHUNK_SIZE_LEN + m_cipherInfo.tagLen) {
                qDebug("AEAD data chunk is incomplete (too small for length)");
                m_incompleteChunk = std::string(
                    reinterpret_cast<const char *>(data), dataEnd - data);
                return std::string();
            }
            std::string decLength = update(
                data, AEAD_CHUNK_SIZE_LEN + m_cipherInfo.tagLen, m_pipe_dec);
            incrementIv(m_filter_dec, m_iv_dec);
            data += (AEAD_CHUNK_SIZE_LEN + m_cipherInfo.tagLen);
            payloadLength = qFromBigEndian(*reinterpret_cast<const uint16_t *>(
                                decLength.data())) &
                            AEAD_CHUNK_SIZE_MASK;
            if (payloadLength == 0) {
                throw std::length_error("AEAD data chunk length is invalid");
            }
        }

        if (dataEnd - data < payloadLength + m_cipherInfo.tagLen) {
            qDebug("AEAD data chunk is incomplete (too small for payload)");
            m_incompleteChunk = std::string(
                reinterpret_cast<const char *>(data), dataEnd - data);
            m_incompleteLength = payloadLength;
            return std::string();
        }
        out = update(data, payloadLength + m_cipherInfo.tagLen, m_pipe_dec);
        incrementIv(m_filter_dec, m_iv_dec);
        data += (payloadLength + m_cipherInfo.tagLen);
        if (dataEnd > data) {
            // Append remaining decrypted chunks recursively if there is any
            out += dec(data, dataEnd - data);
        }
    } else {
        out = update(data, length, m_pipe_dec);
    }
    return out;
}

void Cipher::initEnc(std::string &header) {
    m_iv_enc = Cipher::randomIv(m_method);
    if (m_cipherInfo.type == Cipher::CipherType::AEAD) {
        const std::string salt = Cipher::randomIv(m_cipherInfo.saltLen);
        m_key_enc = Cipher::deriveAeadSubkey(m_cipherInfo.keyLen, m_key, salt);
        header = salt;
    } else {
        m_key_enc = m_key;
        header = m_iv_enc;
    }
    try {
        Botan::SymmetricKey _key(
            reinterpret_cast<const Botan::byte *>(m_key.data()), m_key.size());
        Botan::InitializationVector _iv(
            reinterpret_cast<const Botan::byte *>(m_iv_enc.data()),
            m_iv_enc.size());
        m_filter_enc = Botan::get_cipher(m_cipherInfo.internalName, _key, _iv,
                                         Botan::ENCRYPTION);
        if (m_filter_enc) {
            qDebug() << "m_filter_enc ok";
        } else {
            qDebug() << "m_filter_enc failed";
        }
        // Botan::pipe will take control over filter
        // we shouldn't deallocate filter externally
        m_pipe_enc = std::make_shared<Botan::Pipe>(m_filter_enc);
    } catch (const Botan::Exception &e) {
        QDebug(QtMsgType::QtFatalMsg)
            << "Failed to initialise cipher: " << e.what();
    }
}

void Cipher::initDec(const char *data, size_t length, size_t &offset) {
    if (m_cipherInfo.type == Cipher::CipherType::AEAD) {
        m_iv_dec = std::string(m_cipherInfo.ivLen, static_cast<char>(0));
        if (length < m_cipherInfo.saltLen) {
            throw std::length_error(
                "Data chunk is too small to initialise an AEAD decipher");
        }
        m_key_dec =
            Cipher::deriveAeadSubkey(m_cipherInfo.keyLen, m_key,
                                     std::string(data, m_cipherInfo.saltLen));
        offset = m_cipherInfo.saltLen;
    } else {
        if (length < m_cipherInfo.ivLen) {
            throw std::length_error(
                "Data chunk is too small to initialise a stream decipher");
        }
        m_iv_dec = std::string(data, m_cipherInfo.ivLen);
        m_key_dec = m_key;
        offset = m_cipherInfo.ivLen;
    }

    try {
        Botan::SymmetricKey _key(
            reinterpret_cast<const Botan::byte *>(m_key.data()), m_key.size());
        Botan::InitializationVector _iv(
            reinterpret_cast<const Botan::byte *>(m_iv_dec.data()),
            m_iv_dec.size());
        m_filter_dec = Botan::get_cipher(m_cipherInfo.internalName, _key, _iv,
                                         Botan::DECRYPTION);

        if (m_filter_dec) {
            qDebug() << "m_filter_dec ok";
        } else {
            qDebug() << "m_filter_dec failed";
        }
        // Botan::pipe will take control over filter
        // we shouldn't deallocate filter externally
        m_pipe_dec = std::make_shared<Botan::Pipe>(m_filter_dec);
    } catch (const Botan::Exception &e) {
        QDebug(QtMsgType::QtFatalMsg)
            << "Failed to initialise cipher: " << e.what();
    }
}

std::string Cipher::evpBytesToKey(const Cipher::CipherInfo &cipherInfo,
                                  const std::string &password) {
    std::vector<std::string> m;
    std::string data;
    int i = 0;

    while (m.size() < cipherInfo.keyLen + cipherInfo.ivLen) {
        if (i == 0) {
            data = password;
        } else {
            data = m[i - 1] + password;
        }
        m.push_back(Cipher::md5Hash(data));
        ++i;
    }

    std::string ms;
    std::for_each(m.begin(), m.end(),
                  [&ms](const std::string &bytes) { ms += bytes; });

    return ms.substr(0, cipherInfo.keyLen);
}
