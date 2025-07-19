#pragma once
#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <cstddef>
#include <cryptopp/secblock.h>
#include <cryptopp/osrng.h>

/// @brief Class for handling encryption/decryption of data based on user password
/// @note Uses AES-256-GCM with PBKDF2 for safe key derive from password. Each encryption uses unique salt and IV.
class Crypto {
public:
    /// @brief Constructor with user password
    /// @param password User password (cannot be empty)
    /// @throws std::invalid_argument if password is empty
    explicit Crypto(const std::string& password);
    
    /// @brief Destructor
    ~Crypto() = default;
    
    /// @brief Encrypts plaintext
    /// @param plaintext Text to encrypt
    /// @return Encrypted text in Base64 format (contains salt, IV, ciphertext and tag)
    /// @throws std::runtime_error in case of encryption error
    std::string encrypt(const std::string& plaintext);
    
    /// @brief Decrypts encrypted text
    /// @param ciphertext Encrypted text in Base64 format
    /// @return Decrypted plaintext
    /// @throws std::runtime_error in case of decryption error or wrong password
    std::string decrypt(const std::string& ciphertext);

private:
    std::string userPassword;
    CryptoPP::AutoSeededRandomPool rng;
    
    // Cryptographic constants
    static const size_t AES_KEY_SIZE = 32;        // AES-256 (32 bytes)
    static const size_t IV_SIZE = 12;             // 96 bits for GCM
    static const size_t TAG_SIZE = 16;            // 128 bits for GCM
    static const size_t SALT_SIZE = 16;           // 128 bits salt
    static const unsigned int PBKDF2_ITERATIONS = 100000; // 100k iterations
    
    /// @brief Derives encryption key from password using PBKDF2
    /// @param password User password
    /// @param salt Salt for key derivation
    /// @param derivedKey Output buffer for derived key
    void deriveKeyFromPassword(const std::string& password, 
                              const CryptoPP::SecByteBlock& salt,
                              CryptoPP::SecByteBlock& derivedKey);
    
    // Delete copy constructor and assignment operator
    Crypto(const Crypto&) = delete;
    Crypto& operator=(const Crypto&) = delete;
    
    // Allow move semantics
    Crypto(Crypto&&) = default;
    Crypto& operator=(Crypto&&) = default;
};

/// @brief Class for making access to crypto classes easier along entire software
class CryptoManager {
private:
    static std::map<std::uint32_t, std::unique_ptr<Crypto>> usersCrypto;    // map ID -> crypto
    static std::mutex mtx;                                                  // mutex for safety

public:
    /// @brief Function to register new Crypto object
    /// @param password user password
    /// @param id id of user
    static void registerCrypto(const std::string& password, const std::uint32_t id);

    /// @brief Function to get Crypto object associated with user of given ID
    /// @param id ID of user
    /// @return pointer to crypto object
    static Crypto* get(const std::uint32_t id);
};