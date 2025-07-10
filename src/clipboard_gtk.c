#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void copy_to_clipboard(const char *content, gboolean is_file) {
    gtk_init(NULL, NULL);
    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

    if (is_file) {
        char *uri = g_filename_to_uri(content, NULL, NULL);
        if (uri) {
            gtk_clipboard_set_text(clipboard, uri, -1);
            g_free(uri);
        } else {
            g_printerr("Error: Could not convert file path to URI\n");
            exit(1);
        }
    } else {
        gtk_clipboard_set_text(clipboard, content, -1);
    }
    gtk_clipboard_store(clipboard);
}

int main(int argc, char **argv) {

    
    // Validation
    if (argc < 2) {
        g_printerr("Usage: \"%s {text_to_copy} [--file]\"\n", argv[0]);
        return 1;
    }

    const char *content = argv[1];
    gboolean is_file = FALSE;

    // File check
    if (argc >= 3 && strcmp(argv[2], "--file") == 0) {
        is_file = TRUE;
    }

    copy_to_clipboard(content, is_file);
    return 0;
}