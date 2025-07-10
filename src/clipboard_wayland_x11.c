#include <wayland-client.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

int copy_x11(char* to_copy, char* type) {
    Display *display = XOpenDisplay(NULL);

    // Initialization
    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 1, 1, 0, 0, 0);

    Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
    Atom utf8_string = XInternAtom(display, type, False);
    Atom target = XInternAtom(display, "TARGETS", False);

    XSetSelectionOwner(display, clipboard, window, CurrentTime);
    if (XGetSelectionOwner(display, clipboard) != window) {
        fprintf(stderr, "Failed to set clipboard ownership\n");
        XDestroyWindow(display, window);
        XCloseDisplay(display);
        return 1;
    }

    // Event loop
    XEvent event;
    while (True) {
        if (XPending(display)) {
            XNextEvent(display, &event);
            if (event.type == SelectionRequest) {
                XSelectionRequestEvent *req = &event.xselectionrequest;
                XSelectionEvent response = {0};
                response.type = SelectionNotify;
                response.requestor = req->requestor;
                response.selection = req->selection;
                response.target = req->target;
                response.property = req->property;
                response.time = req->time;

                if (req->target == target) {
                    Atom targets[] = {utf8_string, XA_STRING};
                    XChangeProperty(display, req->requestor, req->property, XA_ATOM, 32, PropModeReplace, (unsigned char *)targets, 2);
                } else if (req->target == utf8_string || req->target == XA_STRING) {
                    XChangeProperty(display, req->requestor, req->property, req->target, 8, PropModeReplace, (unsigned char *)to_copy, strlen(to_copy));
                } else {
                    response.property = None;
                }

                XSendEvent(display, req->requestor, False, 0, (XEvent *)&response);
                XFlush(display);
            } else if (event.type == SelectionClear) {
                break;
            }
        }
    }
    
    // Clean up
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}

// Wayland is more restrictive so we're using wl-copy for this
int copy_wayland(char *to_copy, char* type) {
    size_t length = strlen(to_copy);
    char command[1024];

    snprintf(command, sizeof(command), "wl-copy --type %s", type);

    FILE *pipe = popen(command, "w");
    
    if (!pipe) {
        fprintf(stderr, "Error: Failed to open pipe to wl-copy\n");
        return 1;
    }

    if (fwrite(to_copy, 1, length, pipe) != length) {
        fprintf(stderr, "Error: Failed to write data to wl-copy\n");
        return 1;
    }

    int status = pclose(pipe);
    if (status != 0) {
        fprintf(stderr, "Error: wl-copy failed with status %d", status);
        return 1;
    }

    return 0;
}

int main(int argc, char **argv) {
    // Validation
    if (argc < 2) {
        fprintf(stderr, "Usage: \"%s {text_to_copy} [--file]\"\n", argv[0]);
        return 1;
    }

    // String/File choose
    char* type = NULL;
    char to_copy[512];

    if (argc >= 3 && strcmp(argv[2], "--file") == 0) {
        type = "text/uri-list";
        sprintf(to_copy, "file://%s", argv[1]);
    } else {
        type = "text/plain";
        strcpy(to_copy, argv[1]);
    }

    // Detecting window manager
    const char *wayland_display = getenv("WAYLAND_DISPLAY");
    const char *x11_display = getenv("DISPLAY");
    
    int status = 1;
    if (wayland_display != NULL && strlen(wayland_display) > 0) {
        printf("Using Wayland clipboard\n");

        status = copy_wayland(to_copy, type);

    } else if (x11_display != NULL && strlen(x11_display) > 0) {
        printf("Using X11 clipboard\n");

        status = copy_x11(to_copy, type);

    } else {
        printf("This program requires either Wayland or X11 to work\n");
        return 1;
    }

    if (status == 0) printf("Copied successfully!\n");
    else printf("Failed to copy\n");

    return 0;
}
