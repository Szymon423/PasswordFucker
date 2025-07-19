file(GLOB SOURCES
    "*.cpp"
    "*.hpp"
    "*.h"
)

add_library(PasswordFucker_lib ${SOURCES}) 

target_link_libraries(PasswordFucker_lib PRIVATE 
    spdlog::spdlog
    Poco::Net
	Poco::Util
	Poco::JWT
    ws2_32
    kernel32
    SQLiteCpp
    cryptopp::cryptopp
)

target_include_directories(PasswordFucker_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)