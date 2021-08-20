#include <gtest/gtest.h>

#include "cipher.h"

std::string m_password = "sky-io";
std::string pt0 = "12345678";

TEST(test_cipher, test_all_ciphers_with_whole_data) {
    std::string pt;
    for (int i = 0; i < 128; ++i) {
        pt += pt0;
    }
    for (auto &p : Cipher::cipherInfoMap) {
        std::string m_method = p.first;
        std::unique_ptr<Cipher> cipher =
            std::make_unique<Cipher>(m_method, m_password);
        std::string ct = cipher->enc(pt);
        qInfo() << "method:" << m_method.data();
        std::string pt2 = cipher->dec(ct);
        ASSERT_EQ(pt2, pt) << "enc & dec not match";
    }
}

TEST(test_cipher, test_all_ciphers_with_segmented_data) {
    std::string pt;
    for (int i = 0; i < 128; ++i) {
        pt += pt0;
    }

    for (auto &p : Cipher::cipherInfoMap) {
        std::string m_method = p.first;
        qInfo() << "method:" << m_method.data();
        std::unique_ptr<Cipher> cipher =
            std::make_unique<Cipher>(m_method, m_password);

        std::string ct;
        int i = 0;
        int pt_sz = pt.size();
        for (int j = 0; i < pt_sz; i += j) {
            if (j < 512) ++j;
            if (i + j < pt_sz)
                ct += cipher->enc(pt.substr(i, j));
            else
                ct += cipher->enc(pt.substr(i));
        }

        std::string pt2;
        i = 0;
        int ct_sz = ct.size();
        for (int j = 0; i < ct_sz; i += j) {
            if (j < 512) ++j;
            if (i + j < ct_sz)
                pt2 += cipher->dec(ct.substr(i, j));
            else
                pt2 += cipher->dec(ct.substr(i));
        }

        ASSERT_EQ(pt2, pt) << "enc & dec not match";
    }
}
