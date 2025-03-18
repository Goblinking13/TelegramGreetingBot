cmake_minimum_required(VERSION 3.14)
project(TelegramBot)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system)
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(tgBot
        src/main.cpp
        src/TelegramBot.cpp
        src/TelegramBot.hpp
)

target_link_libraries(tgBot
        Boost::system
        OpenSSL::SSL
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
)
