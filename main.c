#include <gtk/gtk.h>

static void set_default_window_size(GtkWindow *window)
{
    GdkDisplay *display = gdk_display_get_default();
    if (display == NULL)
        return;

    GListModel *monitors = gdk_display_get_monitors(display);
    guint n_monitors = g_list_model_get_n_items(monitors);

    if (n_monitors == 0)
        return;

    int smallest_dimension = G_MAXINT;

    for (guint i = 0; i < n_monitors; i++) {
        GdkMonitor *monitor = g_list_model_get_item(monitors, i);

        GdkRectangle geometry;
        gdk_monitor_get_geometry(monitor, &geometry);

        int monitor_smallest_dimension = MIN(geometry.width, geometry.height);
        smallest_dimension = MIN(smallest_dimension, monitor_smallest_dimension);

        g_object_unref(monitor);
    }

    int height = (int) (smallest_dimension * 0.70);
    int width = height * 16 / 9;

    gtk_window_set_default_size(window, width, height);
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkCssProvider* provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(
        provider,
        ".main-background {"
        "  background-color: navy;"
        "}"
        ".main-label {"
        "  color: hotpink;"
        "  font-size: 32px;"
        "}",
        -1
    );

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    g_object_unref(provider);

    GtkWidget* window = gtk_application_window_new(app);

    GtkIconTheme *icon_theme = gtk_icon_theme_get_for_display(gdk_display_get_default());
    gtk_icon_theme_add_search_path(icon_theme, ".");
    gtk_window_set_icon_name(GTK_WINDOW(window), "com.weburg.example.fether");

    gtk_window_set_title(GTK_WINDOW(window), "FEtHEr");

    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(box, "main-background");
    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_vexpand(box, TRUE);

    GtkWidget* label = gtk_label_new("Excellent!");
    gtk_widget_add_css_class(label, "main-label");
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_vexpand(label, TRUE);

    gtk_box_append(GTK_BOX(box), label);
    gtk_window_set_child(GTK_WINDOW(window), box);

    gtk_window_present(GTK_WINDOW(window));
    set_default_window_size(GTK_WINDOW(window));
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.weburg.example.fether", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}