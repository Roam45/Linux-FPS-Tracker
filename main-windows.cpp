#include <gtk/gtk.h>
#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <Windows.h>
#endif

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

// FPS calculation for Windows
void windows_fps(GtkLabel* fps_label, GtkLabel* stats_label) {
    std::cout << "Running on Windows: FPS calculation started." << std::endl;

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
            gchar* text = g_strdup_printf("FPS: %d (Windows)", fps);
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

    // Start FPS calculation on Windows
    std::cout << "Detected Windows system." << std::endl;
    std::thread(windows_fps, GTK_LABEL(fps_label), GTK_LABEL(stats_label)).detach();

    // GTK main loop
    gtk_main();

    return 0;
}
