# ClipboardCopyLinux
A program that copies an input string/filename to clipboard on Linux using X11/Wayland window managers.\
Requires wl-copy to work on Wayland.\
Requires X11 (libx11-dev or libX11-devel) and Wayland (libwayland-dev or wayland-devel) libs to compile.
Usage: `ClipboardCopy {text} [--file]`

# Dependencies
## Debian-based
`sudo apt-get install libx11-dev libwayland-dev`\
to run on Wayland:\
`sudo apt-get install wl-clipboard`\

## Fedora-based
`sudo dnf install libX11-devel wayland-devel`\
to run on Wayland:\
`sudo dnf install wl-clipboard`

# Building
`cmake -B build && cmake --build build`
