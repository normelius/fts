

cmake_minimum_required(VERSION 3.1.0)

project(fts)

set(Qt5Widgets_DIR "/usr/local/opt/qt5/lib/cmake/Qt5Widgets")
set(Qt5Test_DIR "/usr/local/opt/qt5/lib/cmake/Qt5Test")
set(Qt5Core_DIR "/usr/local/opt/qt5/lib/cmake/Qt5Core")
set(Qt5_DIR "/usr/local/opt/qt5/lib/cmake/Qt5")
set(Qt5Concurrent_DIR "/usr/local/opt/qt5/lib/cmake/Qt5Concurrent")
set(Qt5Gui_DIR "/usr/local/opt/qt5/lib/cmake/Qt5Gui")
set(Qt5PrintSupport_DIR "/usr/local/opt/qt5/lib/cmake/Qt5PrintSupport")
set(Qt5Qml_DIR "/usr/local/opt/qt5/lib/cmake/Qt5Qml")
set(Qt5Quick_DIR "/usr/local/opt/qt5/lib/cmake/Qt5Quick")
set(Qt5QuickWidgets_DIR "/usr/local/opt/qt5/lib/cmake/Qt5QuickWidgets")

#set(CMAKE_CXX_FLAGS "-I/usr/local/opt/openssl/include")
set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(OpenSSL REQUIRED)
if (${OPENSSL_FOUND})
    MESSAGE(STATUS, "Found OPENSSL")
else (${OPENSSL_FOUND})
    MESSAGE(STATUS, "Could not find OPENSSL")
endif (${OPENSSL_FOUND})

find_package(Qt5 COMPONENTS Widgets Gui Core PrintSupport Qml Quick QuickWidgets REQUIRED)

file (GLOB_RECURSE fts_SOURCES CONFIGURE_DEPENDS "src/*.cc")
file (GLOB_RECURSE fts_HEADERS CONFIGURE_DEPENDS "src/*.h")

set (fts_INCLUDE_DIRS "")
foreach (_headerFile ${fts_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND fts_INCLUDE_DIRS ${_dir})
endforeach()
list (REMOVE_DUPLICATES fts_INCLUDE_DIRS)

add_executable(fts 
    resources.qrc 
    third-party/httplib.h
    third-party/qcustomplot.cpp
    third-party/qcustomplot.h
    third-party/date.h 
    ${fts_SOURCES})

target_include_directories(fts PRIVATE ${fts_INCLUDE_DIRS})

# So we can write "third-party/..." instead of ".../third-party/..."
target_include_directories(fts PUBLIC .)

find_package(nlohmann_json REQUIRED)



#if (APPLE)
    # This is a bug in CMake that causes it to prefer the system version over
    # the one in the specified ROOT folder.
#    set(OPENSSL_ROOT_DIR ${OPENSSL_ROOT_DIR} /usr/local/Cellar/openssl@1.1/1.1.1g/)
set(OPENSSL_CRYPTO_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libcrypto.dylib CACHE FILEPATH "" FORCE)
set(OPENSSL_SSL_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libssl.dylib CACHE FILEPATH "" FORCE)
#endif()

if(OPENSSL_FOUND)
    set(HTTPLIB_IS_USING_OPENSSL TRUE)
endif()

find_package(Poco REQUIRED Foundation Net NetSSL)

target_link_libraries(${PROJECT_NAME} PUBLIC
    #$<$<BOOL:${HTTPLIB_IS_USING_OPENSSL}>:OpenSSL::SSL>
    #$<$<BOOL:${HTTPLIB_IS_USING_OPENSSL}>:OpenSSL::Crypto>
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json 
    Qt5::Widgets 
    Qt5::Core 
    Qt5::Gui
    Qt5::PrintSupport
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickWidgets
    Poco::Foundation
    Poco::Net
    Poco::NetSSL)


target_compile_definitions(${PROJECT_NAME} PUBLIC
        $<$<BOOL:${HTTPLIB_IS_USING_OPENSSL}>:CPPHTTPLIB_OPENSSL_SUPPORT>
)




# ----------------
# Creating executable for validating indicators
add_executable(validate_indicators
    tests/indicators/validate_indicators.cc
    src/indicators/trend.h
    src/indicators/trend.cc
    src/indicators/momentum.h
    src/indicators/momentum.cc
    src/indicators/volatility.h
    src/indicators/volatility.cc
    src/indicators/statistics.h
    src/indicators/statistics.cc
)

target_include_directories(validate_indicators PUBLIC .)

target_link_libraries(validate_indicators PUBLIC
    Qt5::Widgets 
    Qt5::Core 
)