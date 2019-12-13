// Copyright (c) 2019 <dev@brigid.jp>
// This software is released under the MIT License.
// https://opensource.org/licenses/mit-license.php

#include <brigid/crypto.hpp>
#include "test.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

namespace {
  static const std::string plaintext = "The quick brown fox jumps over the lazy dog";
  static const std::string key128 = "0123456789012345";
  static const std::string key192 = "012345678901234567890123";
  static const std::string key256 = "01234567890123456789012345678901";
  static const std::string iv = "0123456789012345";
  static const char ciphertext128_data[] = {
    '\x30', '\x89', '\xE6', '\xBC', '\x22', '\x4B', '\xD9', '\x5B',
    '\x85', '\xCF', '\x56', '\xF4', '\xB9', '\x67', '\x11', '\x8A',
    '\xAA', '\x47', '\x05', '\x43', '\x0F', '\x25', '\xB6', '\xB4',
    '\xD9', '\x53', '\x18', '\x8A', '\xD1', '\x5D', '\xD7', '\x8F',
    '\x38', '\x67', '\x57', '\x7E', '\x7D', '\x58', '\xE1', '\x8C',
    '\x9C', '\xB3', '\x40', '\x64', '\x7C', '\x8B', '\x4F', '\xD8',
  };
  static const std::string ciphertext128(ciphertext128_data, 48);
  static const char ciphertext192_data[] = {
    '\x70', '\xEE', '\xD7', '\x34', '\x63', '\x1F', '\xFF', '\x2A',
    '\x7E', '\x00', '\xB1', '\x70', '\x7A', '\xED', '\x19', '\xBB',
    '\xA9', '\x51', '\x20', '\x8B', '\x7F', '\xF1', '\x2F', '\x28',
    '\xD0', '\x43', '\xC8', '\x6C', '\x52', '\x06', '\x2C', '\x3E',
    '\x3F', '\xD6', '\xC1', '\x54', '\x8E', '\x4E', '\x79', '\x84',
    '\x05', '\xD0', '\x39', '\xD0', '\x46', '\x3F', '\x1C', '\x15',
  };
  static const std::string ciphertext192(ciphertext192_data, 48);
  static const char ciphertext256_data[] = {
    '\xE0', '\x6F', '\x63', '\xA7', '\x11', '\xE8', '\xB7', '\xAA',
    '\x9F', '\x94', '\x40', '\x10', '\x7D', '\x46', '\x80', '\xA1',
    '\x17', '\x99', '\x43', '\x80', '\xEA', '\x31', '\xD2', '\xA2',
    '\x99', '\xB9', '\x53', '\x02', '\xD4', '\x39', '\xB9', '\x70',
    '\x2C', '\x8E', '\x65', '\xA9', '\x92', '\x36', '\xEC', '\x92',
    '\x07', '\x04', '\x91', '\x5C', '\xF1', '\xA9', '\x8A', '\x44',
  };
  static const std::string ciphertext256(ciphertext256_data, 48);

  void test_encryptor1(brigid::crypto_cipher cipher, const std::string& key, const std::string& ciphertext) {
    std::vector<char> buffer(plaintext.size() + 16);
    auto encryptor = brigid::make_encryptor(cipher, key.data(), key.size(), iv.data(), iv.size());
    size_t result = encryptor->update(plaintext.data(), plaintext.size(), buffer.data(), buffer.size(), true);

    BRIGID_CHECK(result == ciphertext.size());
    buffer.resize(result);
    BRIGID_CHECK(std::equal(buffer.begin(), buffer.end(), ciphertext.begin()));
  }

  void test_encryptor2(brigid::crypto_cipher cipher, const std::string& key, const std::string& ciphertext) {
    std::vector<char> buffer(plaintext.size() + 16);
    auto encryptor = brigid::make_encryptor(cipher, key.data(), key.size(), iv.data(), iv.size());

    size_t result = encryptor->update(plaintext.data(), 16, buffer.data(), buffer.size(), false);
    BRIGID_CHECK(result == ciphertext.size() / 3);

    result = encryptor->update(plaintext.data() + 16, 16, buffer.data() + 16, buffer.size() - 16, false);
    BRIGID_CHECK(result == ciphertext.size() / 3);

    result = encryptor->update(plaintext.data() + 32, plaintext.size() - 32, buffer.data() + 32, buffer.size() - 32, true);
    BRIGID_CHECK(result == ciphertext.size() / 3);

    buffer.resize(ciphertext.size());
    BRIGID_CHECK(std::equal(buffer.begin(), buffer.end(), ciphertext.begin()));
  }

  void test_decryptor1(brigid::crypto_cipher cipher, const std::string& key, const std::string& ciphertext) {
    std::vector<char> buffer(ciphertext.size());
    auto decryptor = brigid::make_decryptor(cipher, key.data(), key.size(), iv.data(), iv.size());
    size_t result = decryptor->update(ciphertext.data(), ciphertext.size(), buffer.data(), buffer.size(), true);

    BRIGID_CHECK(result == plaintext.size());
    buffer.resize(result);
    BRIGID_CHECK(std::equal(buffer.begin(), buffer.end(), plaintext.begin()));
  }

  void test_decryptor2(brigid::crypto_cipher cipher, const std::string& key, const std::string& ciphertext) {
    std::vector<char> buffer(ciphertext.size());
    auto decryptor = brigid::make_decryptor(cipher, key.data(), key.size(), iv.data(), iv.size());

    size_t result1 = decryptor->update(ciphertext.data(), 16, buffer.data(), buffer.size(), false);
    size_t result2 = result1 + decryptor->update(ciphertext.data() + 16, 16, buffer.data() + result1, buffer.size() - result1, false);
    size_t result3 = result2 + decryptor->update(ciphertext.data() + 32, 16, buffer.data() + result2, buffer.size() - result2, true);
    BRIGID_CHECK(result3 == plaintext.size());

    std::cout << result1 << ", " << result2 << ", " << result3 << "\n";

    buffer.resize(result3);
    BRIGID_CHECK(std::equal(buffer.begin(), buffer.end(), plaintext.begin()));
  }

  void test_error1() {
    BRIGID_CHECK_THROW([&](){ brigid::make_encryptor(brigid::crypto_cipher::aes_256_cbc, key256.data(), key256.size() - 1, iv.data(), iv.size()); });
  }

  void test_error2() {
    BRIGID_CHECK_THROW([&](){ brigid::make_encryptor(brigid::crypto_cipher::aes_256_cbc, key256.data(), key256.size(), iv.data(), iv.size() - 1); });
  }

  void test_buffer_size() {
    auto encryptor = brigid::make_encryptor(brigid::crypto_cipher::aes_256_cbc, key256.data(), key256.size(), iv.data(), iv.size());
    BRIGID_CHECK(encryptor->calculate_buffer_size(17) == 33);
    auto decryptor = brigid::make_decryptor(brigid::crypto_cipher::aes_256_cbc, key256.data(), key256.size(), iv.data(), iv.size());
    BRIGID_CHECK(decryptor->calculate_buffer_size(32) == 32);
  }

  BRIGID_MAKE_TEST_CASE([](){ test_encryptor1(brigid::crypto_cipher::aes_128_cbc, key128, ciphertext128); });
  BRIGID_MAKE_TEST_CASE([](){ test_encryptor2(brigid::crypto_cipher::aes_128_cbc, key128, ciphertext128); });
  BRIGID_MAKE_TEST_CASE([](){ test_encryptor1(brigid::crypto_cipher::aes_192_cbc, key192, ciphertext192); });
  BRIGID_MAKE_TEST_CASE([](){ test_encryptor2(brigid::crypto_cipher::aes_192_cbc, key192, ciphertext192); });
  BRIGID_MAKE_TEST_CASE([](){ test_encryptor1(brigid::crypto_cipher::aes_256_cbc, key256, ciphertext256); });
  BRIGID_MAKE_TEST_CASE([](){ test_encryptor2(brigid::crypto_cipher::aes_256_cbc, key256, ciphertext256); });
  BRIGID_MAKE_TEST_CASE([](){ test_decryptor1(brigid::crypto_cipher::aes_128_cbc, key128, ciphertext128); });
  BRIGID_MAKE_TEST_CASE([](){ test_decryptor2(brigid::crypto_cipher::aes_128_cbc, key128, ciphertext128); });
  BRIGID_MAKE_TEST_CASE([](){ test_decryptor1(brigid::crypto_cipher::aes_192_cbc, key192, ciphertext192); });
  BRIGID_MAKE_TEST_CASE([](){ test_decryptor2(brigid::crypto_cipher::aes_192_cbc, key192, ciphertext192); });
  BRIGID_MAKE_TEST_CASE([](){ test_decryptor1(brigid::crypto_cipher::aes_256_cbc, key256, ciphertext256); });
  BRIGID_MAKE_TEST_CASE([](){ test_decryptor2(brigid::crypto_cipher::aes_256_cbc, key256, ciphertext256); });
  BRIGID_MAKE_TEST_CASE(test_error1);
  BRIGID_MAKE_TEST_CASE(test_error2);
  BRIGID_MAKE_TEST_CASE(test_buffer_size);
}
