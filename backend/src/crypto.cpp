#include "crypto.hpp"

#include <cryptopp/aes.h>
#include <cryptopp/gcm.h>
#include <cryptopp/hex.h>
#include <cryptopp/base64.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>

#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <format>

// Constructor
Crypto::Crypto(const std::string& password) : userPassword(password) {
    if (password.empty()) {
        throw std::invalid_argument("Password cannot be empty");
    }
}

// Key derivation from password
void Crypto::deriveKeyFromPassword(const std::string& password, 
                                  const CryptoPP::SecByteBlock& salt,
                                  CryptoPP::SecByteBlock& derivedKey) {
    derivedKey.resize(AES_KEY_SIZE);
    
    CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256> pbkdf2;
    pbkdf2.DeriveKey(
        derivedKey.data(), derivedKey.size(),
        0x00, // purpose (not used)
        reinterpret_cast<const CryptoPP::byte*>(password.data()), password.length(),
        salt.data(), salt.size(),
        PBKDF2_ITERATIONS,
        0.0 // timeInSeconds (not used)
    );
}

// Encryption
std::string Crypto::encrypt(const std::string& plaintext) {
    try {
        // Generate random salt
        CryptoPP::SecByteBlock salt(SALT_SIZE);
        rng.GenerateBlock(salt, salt.size());
        
        // Derive key from password and salt
        CryptoPP::SecByteBlock key;
        deriveKeyFromPassword(userPassword, salt, key);
        
        // Generate random IV
        CryptoPP::SecByteBlock iv(IV_SIZE);
        rng.GenerateBlock(iv, iv.size());
        
        // AES-GCM encryption
        std::string ciphertext;
        CryptoPP::GCM<CryptoPP::AES>::Encryption enc;
        enc.SetKeyWithIV(key, key.size(), iv, iv.size());
        
        CryptoPP::StringSource ss(plaintext, true,
            new CryptoPP::AuthenticatedEncryptionFilter(enc,
                new CryptoPP::StringSink(ciphertext),
                false, TAG_SIZE
            )
        );
        
        // Combine salt + IV + ciphertext + tag
        std::string combined;
        combined.reserve(SALT_SIZE + IV_SIZE + ciphertext.length());
        combined.append(reinterpret_cast<const char*>(salt.data()), salt.size());
        combined.append(reinterpret_cast<const char*>(iv.data()), iv.size());
        combined.append(ciphertext);
        
        // Encode to Base64
        std::string encoded;
        CryptoPP::StringSource ss2(combined, true,
            new CryptoPP::Base64Encoder(
                new CryptoPP::StringSink(encoded),
                false // no line breaks
            )
        );
        
        return encoded;
        
    } 
    catch (const CryptoPP::Exception& e) {
        throw std::runtime_error("Encryption error: " + std::string(e.what()));
    }
}

// Decryption
std::string Crypto::decrypt(const std::string& ciphertext) {
    try {
        // Decode from Base64
        std::string decoded;
        CryptoPP::StringSource ss(ciphertext, true,
            new CryptoPP::Base64Decoder(
                new CryptoPP::StringSink(decoded)
            )
        );
        
        // Check minimum length
        if (decoded.length() < SALT_SIZE + IV_SIZE + TAG_SIZE) {
            throw std::runtime_error("Invalid encrypted data - too short");
        }
        
        // Extract salt
        CryptoPP::SecByteBlock salt(
            reinterpret_cast<const CryptoPP::byte*>(decoded.data()), 
            SALT_SIZE
        );
        
        // Extract IV
        CryptoPP::SecByteBlock iv(
            reinterpret_cast<const CryptoPP::byte*>(decoded.data() + SALT_SIZE), 
            IV_SIZE
        );
        
        // Extract ciphertext + tag
        std::string encryptedData = decoded.substr(SALT_SIZE + IV_SIZE);
        
        // Derive key from password and extracted salt
        CryptoPP::SecByteBlock key;
        deriveKeyFromPassword(userPassword, salt, key);
        
        // AES-GCM decryption
        std::string recovered;
        CryptoPP::GCM<CryptoPP::AES>::Decryption dec;
        dec.SetKeyWithIV(key, key.size(), iv, iv.size());
        
        CryptoPP::StringSource ss2(encryptedData, true,
            new CryptoPP::AuthenticatedDecryptionFilter(dec,
                new CryptoPP::StringSink(recovered),
                CryptoPP::AuthenticatedDecryptionFilter::DEFAULT_FLAGS,
                TAG_SIZE
            )
        );
        
        return recovered;
        
    } 
    catch (const CryptoPP::Exception& e) {
        throw std::runtime_error("Decryption error (probably wrong password): " + std::string(e.what()));
    }
}

std::map<std::uint32_t, std::unique_ptr<Crypto>> CryptoManager::usersCrypto;
std::mutex CryptoManager::mtx;

void CryptoManager::registerCrypto(const std::string& password, const std::uint32_t id) {
    // Check if Crypto with this key allready exists, if so - erase it
    auto it = usersCrypto.find(id);
    if (it != usersCrypto.end()) {
        usersCrypto.erase(it);
    }

    // Register new instance
    usersCrypto[id] = std::make_unique<Crypto>(password);
}

Crypto* CryptoManager::get(const std::uint32_t id) {
    auto it = usersCrypto.find(id);
    if (it == usersCrypto.end()) {
        throw std::runtime_error(std::format("Crypto with id: {} was not found", id));
    }
    return it->second.get();
}