cmake_minimum_required(VERSION 3.1.0)

project(ukui-samba-config LANGUAGES CXX)

add_definitions("-DQT_NO_KEYWORDS" -DDBUS_NAME="org.ukui.samba.share.config" -DDBUS_PATH="/org/ukui/samba/share")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig)
find_package(Qt5 COMPONENTS Core DBus)

pkg_check_modules(Polkit REQUIRED polkit-gobject-1)
pkg_check_modules(GIO REQUIRED gio-2.0)

include(data/data.cmake)

add_executable(peony-samba-service
    src/samba-config.cpp src/samba-config.h
    peony-samba-service.cpp
)
target_link_libraries(peony-samba-service Qt5::Core Qt5::DBus ${Polkit_LIBRARIES} ${GIO_LIBRARIES})
target_include_directories(peony-samba-service PUBLIC ${QT_INCLUDE_DIRS} ${Polkit_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS})
install (TARGETS peony-samba-service DESTINATION /usr/libexec/)

add_executable(test-auth test/test-auth.cpp src/samba-config-interface.cpp src/samba-config-interface.h)
target_link_libraries(test-auth Qt5::Core Qt5::DBus ${Polkit_LIBRARIES} ${GIO_LIBRARIES})
target_include_directories(test-auth PUBLIC ${QT_INCLUDE_DIRS} ${Polkit_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS})

#target_link_libraries(ukui-samba-config Qt${QT_VERSION_MAJOR}::Core)
