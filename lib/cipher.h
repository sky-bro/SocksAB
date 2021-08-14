#ifndef CIPHER_H
#define CIPHER_H

#include <botan/auto_rng.h>
#include <botan/filters.h>
#include <botan/hkdf.h>
#include <botan/hmac.h>
#include <botan/kdf.h>
#include <botan/md5.h>
#include <botan/pipe.h>
#include <botan/sha160.h>

#include <QDebug>
#include <QtEndian>
#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>

const size_t AEAD_CHUNK_SIZE_LEN = 2;
const uint16_t AEAD_CHUNK_SIZE_MASK = 0x3FFF;

class Cipher {
  public:
    enum CipherType { STREAM, AEAD };

    struct CipherInfo {
        std::string internalName;  // internal implementation name in Botan
        size_t keyLen;
        size_t ivLen;
        CipherType type;
        size_t saltLen;  // only for AEAD
        size_t tagLen;   // only for AEAD

        CipherInfo(std::string internalName, size_t keyLen, size_t ivLen,
                   CipherType type, size_t saltLen = 0, size_t tagLen = 0)
            : internalName(internalName),
              keyLen(keyLen),
              ivLen(ivLen),
              type(type),
              saltLen(saltLen),
              tagLen(tagLen) {}
    };

    // [this]() { return std::make_unique<Encryptor>(m_profile.method(),
    // m_profile.password()); }
    using CipherCreator = std::function<std::unique_ptr<Cipher>()>;

    static const std::unordered_map<std::string, CipherInfo> cipherInfoMap;

    /*
     * The label/info string used for key derivation function
     */
    static const std::string kdfLabel;

    /**
     * @brief randomIv Generates a vector of random characters of given length
     * @param length
     * @return
     */
    static std::string randomIv(int length);

    /**
     * @brief randomIv An overloaded function to generate randomised IV for
     * given cipher method For AEAD ciphers, this method returns all zeros
     * @param method The Shadowsocks cipher method name
     * @return
     */
    static std::string randomIv(const std::string &method);

    // Copied from libsodium's sodium_increment
    void nonceIncrement(unsigned char *n, const size_t nlen);

    /**
     * @brief incrementIv Increments the current nonce by 1
     * This is required by Shadowsocks AEAD operation after each
     * encryption/decryption
     */
    void incrementIv(Botan::Keyed_Filter *m_filter, std::string &m_iv);

    static std::string md5Hash(const std::string &in);

    /*
     * get session key in aead mode
     */
    static std::string deriveAeadSubkey(size_t length,
                                        const std::string &masterKey,
                                        const std::string &salt);

    Cipher(const std::string method, const std::string password);
    Cipher(Cipher &cipher);

    std::string update(const std::string &data,
                       std::shared_ptr<Botan::Pipe> m_pipe);
    std::string update(const char *data, size_t length,
                       std::shared_ptr<Botan::Pipe> m_pipe);

    std::string enc(std::string &data);
    std::string enc(const char *data, size_t length);
    std::string dec(std::string &data);
    std::string dec(const char *data, size_t length);

  private:
    void initEnc(std::string &header);
    void initDec(const char *data, size_t length, size_t &offset);

    std::string evpBytesToKey(const CipherInfo &cipherInfo,
                              const std::string &password);

    Botan::Keyed_Filter *m_filter_enc;
    Botan::Keyed_Filter *m_filter_dec;
    std::shared_ptr<Botan::Pipe> m_pipe_enc;
    std::shared_ptr<Botan::Pipe> m_pipe_dec;
    const std::string m_method;
    const CipherInfo m_cipherInfo;
    const std::string m_key;  // preshared master key, derived from password
    std::string m_key_enc;
    std::string m_key_dec;
    std::string m_iv_enc;  // nonce for enc
    std::string m_iv_dec;  // nonce for dec

    std::string m_incompleteChunk;
    uint16_t m_incompleteLength;
};

#endif  // CIPHER_H
