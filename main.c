#include <gtk/gtk.h>
#include "ui.h"

static void activate(GtkApplication *app, gpointer user_data)
{
    GdkDisplay *display = gdk_display_get_default();
    GtkCssProvider* provider = gtk_css_provider_new();

    gtk_css_provider_load_from_string(
        provider,
        ".main-background {"
        "  background-color: hotpink;"
        "}"
        ".main-label {"
        "  color: navy;"
        "  font-size: 32px;"
        "}"
        ".main-background.dark {"
        "  background-color: navy;"
        "}"
        ".main-label.dark {"
        "  color: hotpink;"
        "}"
    );

    gtk_style_context_add_provider_for_display(
        display,
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

    ui_watch_color_scheme(app, box, label);
    gtk_window_present(GTK_WINDOW(window));
    ui_set_default_window_size(GTK_WINDOW(window));
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