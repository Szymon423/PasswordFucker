// src/passwords.hpp
#pragma once

#include <string>
#include <list>
#include <chrono>
#include <cstddef>
#include <optional>
#include <mutex>
#include <memory>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <SQLiteCpp/SQLiteCpp.h>

/// @brief Namespace of password related stuff
namespace pass {
    /// @brief Class with password object
    class Password {
    public:
        std::uint32_t id;       // ID of entry
        std::string login;      // Login to be used with password
        std::string password;   // Password to be used with login
        std::string name;       // Name to be displayed with Password
        std::string url;        // URL related to this password
        std::string notes;      // Note associated with Password
        std::chrono::system_clock::time_point createdAt;    // Timestamp of creation
        std::chrono::system_clock::time_point updatedAt;    // Timestamp of last update
        
        /// @brief Function to convert Password object to Json
        /// @return json object
        nlohmann::json toJson() const;

        /// @brief Function to convert Json to Password object
        /// @param password Json with password
        /// @return Password object
        static Password fromJson(const nlohmann::json& password);
    };

    /// @brief Interface for password repository
    class IPasswordRepository {
    public:
        /// @brief Virtual destructor for password repository
        virtual ~IPasswordRepository() = default;

        /// @brief Virtual getter for passwords
        /// @return list of passwords
        virtual std::list<Password> getAll() = 0;
        
        /// @brief Virtual function to read password with given id
        /// @param id id of password to read
        /// @return optional password object
        virtual std::optional<Password> getById(const std::uint32_t& id) = 0;
        
        /// @brief Virtual function to add new password to repository
        /// @param password password to add
        virtual void add(Password& password) = 0;
        
        /// @brief Virtual function to update password in repository
        /// @param password password to update
        virtual void update(const Password& password) = 0;
        
        /// @brief Virtual function to remove password from repository
        /// @param id id of password to remove
        virtual void remove(const std::uint32_t id) = 0;
    };

    /// @brief Thread-safe SQLite repository for password storing implementing Singleton pattern
    class SQLitePasswordRepository : public IPasswordRepository {
    private:
        std::unique_ptr<SQLite::Database> db;   // Pointer to database
        mutable std::mutex mutex;               // Mutex for thread-safe operations

        /// @brief Private constructor for Singleton pattern
        /// @param dbPath Path to database file
        explicit SQLitePasswordRepository(const std::filesystem::path& dbPath);
        
        /// @brief Private destructor
        ~SQLitePasswordRepository() = default;
        
        /// @brief Delete copy constructor
        SQLitePasswordRepository(const SQLitePasswordRepository&) = delete;
        
        /// @brief Delete assignment operator
        SQLitePasswordRepository& operator=(const SQLitePasswordRepository&) = delete;
        
        /// @brief Delete move constructor
        SQLitePasswordRepository(SQLitePasswordRepository&&) = delete;
        
        /// @brief Delete move assignment operator
        SQLitePasswordRepository& operator=(SQLitePasswordRepository&&) = delete;

        /// @brief Initialize database schema
        void initializeDatabase();

    public:
        /// @brief Get singleton instance of repository
        /// @param dbPath Path to database file (used only on first call)
        /// @return Reference to repository instance
        static SQLitePasswordRepository& getInstance(const std::filesystem::path& dbPath = "passwords.db");

        /// @brief Get all passwords from repository
        /// @return List of all passwords
        std::list<Password> getAll() override;

        /// @brief Get password by its id
        /// @param id ID of password to retrieve
        /// @return Optional containing password if found
        std::optional<Password> getById(const std::uint32_t& id) override;

        /// @brief Add new password to repository
        /// @param password Password to add
        void add(Password& password) override;

        /// @brief Update existing password in repository
        /// @param password Password to update
        void update(const Password& password) override;

        /// @brief Remove password from repository
        /// @param id ID of password to remove
        void remove(const std::uint32_t id) override;

        /// @brief Execute custom database operation with automatic locking
        /// @tparam Func Type of lambda function
        /// @param operation Lambda function with database operation
        /// @return Result of operation
        template<typename Func>
        auto executeOperation(Func operation) {
            std::lock_guard<std::mutex> lock(mutex);
            return operation(db.get());
        }
    };

    /// @brief Manager class for convenient password operations
    class PasswordManager {
    private:
        SQLitePasswordRepository& repo;    // Reference to repository singleton
        static std::filesystem::path dbPath;
        
    public:
        static void initialize(const std::filesystem::path& databasePath = "passwords.db");

        /// @brief Constructor
        /// @param dbPath Path to database file
        explicit PasswordManager();

        /// @brief Get all passwords
        /// @return List of all passwords
        std::list<Password> getAllPasswords();

        /// @brief Get password by id
        /// @param id ID of password to retrieve
        /// @return Optional containing password if found
        std::optional<Password> getPasswordById(const std::uint32_t& id);

        /// @brief Add new password
        /// @param password Password to add
        void addPassword(Password& password);

        /// @brief Update existing password
        /// @param password Password to update
        void updatePassword(const Password& password);

        /// @brief Remove password
        /// @param id ID of password to remove
        void removePassword(const std::uint32_t id);

        /// @brief Execute custom database operation
        /// @tparam Func Type of lambda function
        /// @param operation Lambda function with database operation
        /// @return Result of operation
        template<typename Func>
        auto executeCustomOperation(Func operation) {
            return repo.executeOperation(operation);
        }
    };
}
