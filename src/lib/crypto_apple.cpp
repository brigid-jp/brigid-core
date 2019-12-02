// Copyright (c) 2019 <dev@brigid.jp>
// This software is released under the MIT License.
// https://opensource.org/licenses/mit-license.php

#include <brigid/crypto.hpp>
#include <brigid/noncopyable.hpp>
#include "error.hpp"
#include "type_traits.hpp"

#include <CommonCrypto/CommonCrypto.h>

#include <stddef.h>
#include <memory>

namespace brigid {
  namespace {
    inline const char* make_error_message(CCCryptorStatus status) {
      switch (status) {
        case -4300: return "kCCParamError";
        case -4301: return "kCCBufferTooSmall";
        case -4302: return "kCCMemoryFailure";
        case -4303: return "kCCAlignmentError";
        case -4304: return "kCCDecodeError";
        case -4305: return "kCCUnimplemented";
        case -4306: return "kCCOverflow";
        case -4307: return "kCCRNGFailure";
        case -4308: return "kCCUnspecifiedError";
        case -4309: return "kCCCallSequenceError";
        case -4310: return "kCCKeySizeError";
        case -4311: return "kCCInvalidKey";
      }
      return nullptr;
    }

    inline void check(CCCryptorStatus status) {
      if (status != kCCSuccess) {
        if (const char* message = make_error_message(status)) {
          throw BRIGID_ERROR(message);
        } else {
          throw BRIGID_ERROR(make_error_code("CCCryptorStatus", status));
        }
      }
    }

    using cryptor_ref_t = std::unique_ptr<remove_pointer_t<CCCryptorRef>, decltype(&CCCryptorRelease)>;

    inline cryptor_ref_t make_cryptor_ref(CCCryptorRef cryptor = nullptr) {
      return cryptor_ref_t(cryptor, &CCCryptorRelease);
    }

    class aes_cryptor_impl : public cryptor, private noncopyable {
    public:
      aes_cryptor_impl(CCOperation operation, const char* key_data, size_t key_size, const char* iv_data)
        : cryptor_(make_cryptor_ref()) {
        CCCryptorRef cryptor = nullptr;
        check(CCCryptorCreateWithMode(operation, kCCModeCBC, kCCAlgorithmAES, kCCOptionPKCS7Padding, iv_data, key_data, key_size, nullptr, 0, 0, 0, &cryptor));
        cryptor_ = make_cryptor_ref(cryptor);
      }

      virtual size_t update(const char* in_data, size_t in_size, char* out_data, size_t out_size, bool padding) {
        size_t size1 = 0;
        size_t size2 = 0;
        check(CCCryptorUpdate(cryptor_.get(), in_data, in_size, out_data, out_size, &size1));
        if (padding) {
          check(CCCryptorFinal(cryptor_.get(), out_data + size1, out_size - size1, &size2));
        }
        return size1 + size2;
      }

    private:
      cryptor_ref_t cryptor_;
    };
  }

  crypto_initializer::crypto_initializer() {}
  crypto_initializer::~crypto_initializer() {}

  std::unique_ptr<cryptor> make_encryptor(crypto_cipher cipher, const char* key_data, size_t key_size, const char* iv_data, size_t iv_size) {
    switch (cipher) {
      case crypto_cipher::aes_128_cbc:
      case crypto_cipher::aes_192_cbc:
      case crypto_cipher::aes_256_cbc:
        if (iv_size != 16) {
          throw BRIGID_ERROR("invalid initialization vector size");
        }
        return std::unique_ptr<cryptor>(new aes_cryptor_impl(kCCEncrypt, key_data, key_size, iv_data));
      default:
        throw BRIGID_ERROR("unsupported cipher");
    }
  }

  std::unique_ptr<cryptor> make_decryptor(crypto_cipher cipher, const char* key_data, size_t key_size, const char* iv_data, size_t iv_size) {
    switch (cipher) {
      case crypto_cipher::aes_128_cbc:
      case crypto_cipher::aes_192_cbc:
      case crypto_cipher::aes_256_cbc:
        if (iv_size != 16) {
          throw BRIGID_ERROR("invalid initialization vector size");
        }
        return std::unique_ptr<cryptor>(new aes_cryptor_impl(kCCDecrypt, key_data, key_size, iv_data));
      default:
        throw BRIGID_ERROR("unsupported cipher");
    }
  }
}
