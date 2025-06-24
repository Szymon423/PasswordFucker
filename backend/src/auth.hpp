#pragma once

#include <string>
#include <cstddef>
#include <list>
#include <optional>
#include <nlohmann/json.hpp>
#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>
#include <mutex>
#include <database-manager.hpp>

namespace auth {
    /// @brief Class with User object
    class User {
    public:
        std::uint32_t id;           // id of user 
        std::string login;          // login of user 
        std::string password;       // password of user     
        std::string name;           // name of user 
        std::string surname;        // surname of user     

        /// @brief Function to convert user object to Json
        /// @return json object
        nlohmann::json toJson() const;

        /// @brief Function to convert Json to User object
        /// @param user Json with user
        /// @return User object
        static User fromJson(const nlohmann::json& user);
    };

    /// @brief Interface for User repository
    class IUserRepository {
    public:
        /// @brief Virtual destructor for user repository
        virtual ~IUserRepository() = default;

        /// @brief Virtual getter for users
        /// @return list of users
        virtual std::list<User> getAll() = 0;
        
        /// @brief Virtual function to read user with given id
        /// @param id id of user to read
        /// @return optional user object
        virtual std::optional<User> getById(const std::uint32_t& id) = 0;
        
        /// @brief Virtual function to add new user to repository
        /// @param user user to add
        virtual void add(User& user) = 0;
        
        /// @brief Virtual function to update user in repository
        /// @param user user to update
        virtual void update(const User& user) = 0;
        
        /// @brief Virtual function to remove user from repository
        /// @param id id of user to remove
        virtual void remove(const std::uint32_t id) = 0;
    };

    /// @brief Thread-safe SQLite repository for user storing implementing Singleton pattern
    class SQLiteUserRepository : public IUserRepository {
    private:
        std::shared_ptr<SQLite::Database> db;   // Pointer to database

        /// @brief Private constructor for Singleton pattern
        explicit SQLiteUserRepository();
        
        /// @brief Private destructor
        ~SQLiteUserRepository() = default;
        
        /// @brief Delete copy, assignment, move, move assignment constructor
        SQLiteUserRepository(const SQLiteUserRepository&) = delete;
        SQLiteUserRepository& operator=(const SQLiteUserRepository&) = delete;
        SQLiteUserRepository(SQLiteUserRepository&&) = delete;
        SQLiteUserRepository& operator=(SQLiteUserRepository&&) = delete;

        /// @brief Initialize database schema
        void initializeDatabase();

    public:
        /// @brief Get singleton instance of repository
        /// @param dbPath Path to database file (used only on first call)
        /// @return Reference to repository instance
        static SQLiteUserRepository& getInstance();

        /// @brief Get all users from repository
        /// @return List of all users
        std::list<User> getAll() override;

        /// @brief Get user by its id
        /// @param id ID of user to retrieve
        /// @return Optional containing user if found
        std::optional<User> getById(const std::uint32_t& id) override;

        /// @brief Add new user to repository
        /// @param user User to add
        void add(User& user) override;

        /// @brief Update existing user in repository
        /// @param user User to update
        void update(const User& user) override;

        /// @brief Remove user from repository
        /// @param id ID of user to remove
        void remove(const std::uint32_t id) override;

        /// @brief Execute custom database operation with automatic locking
        /// @tparam Func Type of lambda function
        /// @param operation Lambda function with database operation
        /// @return Result of operation
        template<typename Func>
        auto executeOperation(Func operation) {
            std::lock_guard<std::mutex> lock(DatabaseManager::getInstance().getMutex());
            return operation(db.get());
        }
    };

    /// @brief Manager class for convenient users operations
    class AuthenticationManager {
    private:
        SQLiteUserRepository& repo;    // Reference to repository singleton
        
    public:
        /// @brief Constructor
        explicit AuthenticationManager();
        
        /// @brief secret key for JWT signer
        static std::string secretKey;

        /// @brief Get all users
        /// @return List of all users
        std::list<User> getAllUsers();

        /// @brief Get user by id
        /// @param id ID of user to retrieve
        /// @return Optional containing user if found
        std::optional<User> getUserById(const std::uint32_t& id);

        /// @brief Add new user
        /// @param user user to add
        void addUser(User& user);

        /// @brief Update existing user
        /// @param user user to update
        void updateUser(const User& user);

        /// @brief Remove user
        /// @param id ID of user to remove
        void removeUser(const std::uint32_t id);

        /// @brief Execute custom database operation
        /// @tparam Func Type of lambda function
        /// @param operation Lambda function with database operation
        /// @return Result of operation
        template<typename Func>
        auto executeCustomOperation(Func operation) {
            return repo.executeOperation(operation);
        }

        /// @brief Function to set Private Key
        /// @param key key for JWT signer
        static void setPrivateKey(const std::string& key);

        /// @brief Function to check credentials for loging in
        /// @param login user login 
        /// @param password user password
        /// @return User object when credentials correct, nullopt otherwise
        static std::optional<User> checkCredentials(const std::string& login, const std::string& password);

        /// @brief Function to generate JWT token
        /// @param user user for who generate token
        /// @return string with JWT
        static std::string generateJWTToken(const User& user);
        
        /// @brief Function to validate JWT token
        /// @param token token to validate
        /// @return id of user carried in token
        static uint32_t validateJWTToken(const std::string& token);
    };
}