cmake_minimum_required(VERSION 3.10)
project(ClipboardCopy)

find_package(X11 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)

add_executable(ClipboardCopy src/clipboard_wayland_x11.c)

target_link_libraries(ClipboardCopy PRIVATE
    ${X11_LIBRARIES}
    ${WAYLAND_CLIENT_LIBRARIES}
)