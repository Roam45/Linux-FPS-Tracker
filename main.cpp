#include <gtk/gtk.h>
#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>

#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <wayland-client.h>
#endif

// Function to check if running on Wayland
bool is_wayland() {
    const char* wayland_display = std::getenv("WAYLAND_DISPLAY");
    return wayland_display != nullptr;
}

// Function to check if running on X11
bool is_x11() {
    const char* x11_display = std::getenv("DISPLAY");
    return x11_display != nullptr;
}

// Function to safely update GTK labels from another thread
void update_label(GtkLabel* label, const char* text) {
    // Create a new structure to hold the label and text
    struct LabelUpdateData {
        GtkLabel* label;
        gchar* text;
    };

    LabelUpdateData* data = new LabelUpdateData();
    data->label = label;
    data->text = g_strdup(text);

    // Add to the idle queue to update the label safely in the main thread
    g_idle_add([](gpointer data) -> gboolean {
        LabelUpdateData* update_data = static_cast<LabelUpdateData*>(data);
        gtk_label_set_text(update_data->label, update_data->text);
        g_free(update_data->text);
        delete update_data;
        return G_SOURCE_REMOVE;
    }, data);
}

// Wayland FPS calculation
void wayland_fps(GtkLabel* fps_label, GtkLabel* stats_label) {
    std::cout << "Running on Wayland: FPS calculation started." << std::endl;

    static int frame_count = 0;
    static int fps = 0;
    static auto last_time = std::chrono::steady_clock::now();

    while (true) {
        frame_count++;

        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - last_time).count();

        if (elapsed_time >= 1) {
            fps = frame_count;
            frame_count = 0;
            last_time = current_time;

            // Update the FPS label
            gchar* text = g_strdup_printf("FPS: %d (Wayland)", fps);
            update_label(fps_label, text);
            g_free(text);

            // Update the performance stats label
            gchar* stats_text = g_strdup_printf("Frame count: %d\nElapsed time: %d s", frame_count, elapsed_time);
            update_label(stats_label, stats_text);
            g_free(stats_text);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
}

// FPS calculation for X11
void x11_fps(GtkLabel* fps_label, GtkLabel* stats_label) {
#ifdef __linux__
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        std::cerr << "Unable to open X display" << std::endl;
        return;
    }

    Window root = DefaultRootWindow(display);

    // Initialize XComposite
    int event_base, error_base;
    if (!XCompositeQueryExtension(display, &event_base, &error_base)) {
        std::cerr << "XComposite extension not available" << std::endl;
        XCloseDisplay(display);
        return;
    }

    static int frame_count = 0;
    static int fps = 0;
    static auto last_time = std::chrono::steady_clock::now();

    while (true) {
        // Get the currently focused window
        Window focused;
        int revert;
        XGetInputFocus(display, &focused, &revert);

        // Redirect the focused window's output to an off-screen buffer
        XCompositeRedirectWindow(display, focused, CompositeRedirectAutomatic);

        // FPS calculation
        frame_count++;
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - last_time).count();

        if (elapsed_time >= 1) {
            fps = frame_count;
            frame_count = 0;
            last_time = current_time;

            // Update the FPS label
            gchar* text = g_strdup_printf("FPS: %d (X11)", fps);
            update_label(fps_label, text);
            g_free(text);

            // Update the performance stats label
            gchar* stats_text = g_strdup_printf("Frame count: %d\nElapsed time: %d s", frame_count, elapsed_time);
            update_label(stats_label, stats_text);
            g_free(stats_text);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }

    XCloseDisplay(display);
#else
    std::cerr << "X11 is not supported on this platform." << std::endl;
#endif
}

int main(int argc, char* argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create a GTK window
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 100);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE); // Remove window decorations
    gtk_window_set_keep_above(GTK_WINDOW(window), TRUE); // Keep the window always on top
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_NONE); // Disable automatic positioning

    // Move the window to the top-left corner
    gtk_window_move(GTK_WINDOW(window), 0, 0);

    // Create labels to display the FPS and performance stats
    GtkWidget* fps_label = gtk_label_new("FPS: 0");
    GtkWidget* stats_label = gtk_label_new("Frame count: 0\nElapsed time: 0 s");

    // Create a vertical box to hold the labels
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), fps_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), stats_label, FALSE, FALSE, 0);

    // Add the box to the window
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Apply CSS for semi-transparent background
    GtkCssProvider* css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider, "window { background-color: rgba(0, 0, 0, 0.18); }", -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Show the window
    gtk_widget_show_all(window);

    // Detect display server and start FPS calculation
    if (is_wayland()) {
        std::cout << "Detected Wayland display server." << std::endl;
        std::thread(wayland_fps, GTK_LABEL(fps_label), GTK_LABEL(stats_label)).detach();
    } else if (is_x11()) {
        std::cout << "Detected X11 display server." << std::endl;
        std::thread(x11_fps, GTK_LABEL(fps_label), GTK_LABEL(stats_label)).detach();
    } else {
        std::cerr << "Cannot run on the current display server." << std::endl;
        gtk_label_set_text(GTK_LABEL(fps_label), "Unsupported display server.");
    }

    // GTK main loop
    gtk_main();

    return 0;
}
