#include <gtk/gtk.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>

// Function to update the label safely from another thread
void update_label(GtkLabel* label, const char* text) {
    struct LabelUpdateData {
        GtkLabel* label;
        gchar* text;
    };

    LabelUpdateData* data = new LabelUpdateData();
    data->label = label;
    data->text = g_strdup(text);

    g_idle_add([](gpointer data) -> gboolean {
        LabelUpdateData* update_data = static_cast<LabelUpdateData*>(data);
        gtk_label_set_text(update_data->label, update_data->text);
        g_free(update_data->text);
        delete update_data;
        return G_SOURCE_REMOVE;
    }, data);
}

// Windows FPS Calculation
void windows_fps(GtkLabel* fps_label, GtkLabel* stats_label) {
    std::cout << "Running on Windows: FPS calculation started." << std::endl;

    LARGE_INTEGER frequency;
    LARGE_INTEGER last_time;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&last_time);

    int frame_count = 0;
    int fps = 0;

    while (true) {
        frame_count++;

        LARGE_INTEGER current_time;
        QueryPerformanceCounter(&current_time);

        double elapsed_time = static_cast<double>(current_time.QuadPart - last_time.QuadPart) / frequency.QuadPart;

        if (elapsed_time >= 1.0) {
            fps = frame_count;
            frame_count = 0;
            last_time = current_time;

            // Update the FPS label
            gchar* text = g_strdup_printf("FPS: %d (Windows)", fps);
            update_label(fps_label, text);
            g_free(text);

            // Update the performance stats label
            gchar* stats_text = g_strdup_printf("Frame count: %d\nElapsed time: %.2f s", frame_count, elapsed_time);
            update_label(stats_label, stats_text);
            g_free(stats_text);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
}

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 100);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_NONE);
    gtk_window_move(GTK_WINDOW(window), 0, 0);

    GtkWidget* fps_label = gtk_label_new("FPS: 0");
    GtkWidget* stats_label = gtk_label_new("Frame count: 0\nElapsed time: 0 s");

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), fps_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), stats_label, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkCssProvider* css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider, "window { background-color: rgba(0, 0, 0, 0.18); }", -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gtk_widget_show_all(window);

    // Start Windows FPS calculation
    std::thread(windows_fps, GTK_LABEL(fps_label), GTK_LABEL(stats_label)).detach();

    gtk_main();
    return 0;
}
