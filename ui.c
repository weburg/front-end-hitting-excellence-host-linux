#include "ui.h"

static void apply_color_scheme(GtkWidget *background, GtkWidget *label, guint32 color_scheme)
{
    if (color_scheme == 1) {
        gtk_widget_add_css_class(background, "dark");
        gtk_widget_add_css_class(label, "dark");
    } else {
        gtk_widget_remove_css_class(background, "dark");
        gtk_widget_remove_css_class(label, "dark");
    }
}

static gboolean get_uint32_variant(GVariant *variant, guint32 *value)
{
    GVariant *unwrapped = g_variant_ref(variant);

    while (g_variant_is_of_type(unwrapped, G_VARIANT_TYPE_VARIANT)) {
        GVariant *inner = g_variant_get_variant(unwrapped);
        g_variant_unref(unwrapped);
        unwrapped = inner;
    }

    gboolean is_uint32 = g_variant_is_of_type(unwrapped, G_VARIANT_TYPE_UINT32);
    if (is_uint32)
        *value = g_variant_get_uint32(unwrapped);

    g_variant_unref(unwrapped);
    return is_uint32;
}

static void portal_read_finished(GObject *source, GAsyncResult *result, gpointer user_data)
{
    GDBusConnection *connection = G_DBUS_CONNECTION(source);
    GtkApplication *app = GTK_APPLICATION(user_data);
    GError *error = NULL;
    GVariant *reply = g_dbus_connection_call_finish(connection, result, &error);

    if (reply == NULL) {
        g_warning("Could not read desktop color scheme from portal: %s", error->message);
        g_error_free(error);
        return;
    }

    GVariant *wrapped_value;
    guint32 color_scheme;
    g_variant_get(reply, "(@v)", &wrapped_value);

    if (get_uint32_variant(wrapped_value, &color_scheme)) {
        GtkWidget *background = GTK_WIDGET(
            g_object_get_data(G_OBJECT(app), "color-scheme-background"));
        GtkWidget *label = GTK_WIDGET(
            g_object_get_data(G_OBJECT(app), "color-scheme-label"));
        apply_color_scheme(background, label, color_scheme);
    } else {
        g_warning("Desktop portal returned an unexpected color-scheme value");
    }

    g_variant_unref(wrapped_value);
    g_variant_unref(reply);
}

static void portal_setting_changed(GDBusConnection *connection,
                                   const char *sender_name,
                                   const char *object_path,
                                   const char *interface_name,
                                   const char *signal_name,
                                   GVariant *parameters,
                                   gpointer user_data)
{
    GtkApplication *app = GTK_APPLICATION(user_data);
    const char *name_space;
    const char *key;
    GVariant *wrapped_value;
    g_variant_get(parameters, "(&s&s@v)", &name_space, &key, &wrapped_value);

    if (g_str_equal(name_space, "org.freedesktop.appearance") &&
        g_str_equal(key, "color-scheme")) {
        guint32 color_scheme;
        if (get_uint32_variant(wrapped_value, &color_scheme)) {
            GtkWidget *background = GTK_WIDGET(
                g_object_get_data(G_OBJECT(app), "color-scheme-background"));
            GtkWidget *label = GTK_WIDGET(
                g_object_get_data(G_OBJECT(app), "color-scheme-label"));
            apply_color_scheme(background, label, color_scheme);
        } else {
            g_warning("Desktop portal sent an unexpected color-scheme value");
        }
    }

    g_variant_unref(wrapped_value);
}

static void session_bus_ready(GObject *source, GAsyncResult *result, gpointer user_data)
{
    GtkApplication *app = GTK_APPLICATION(user_data);
    GError *error = NULL;
    GDBusConnection *connection = g_bus_get_finish(result, &error);

    if (connection == NULL) {
        g_warning("Could not connect to the session bus for color-scheme preference: %s",
                  error->message);
        g_error_free(error);
        return;
    }

    g_object_set_data_full(G_OBJECT(app), "color-scheme-connection",
                           connection, g_object_unref);
    g_dbus_connection_signal_subscribe(
        connection,
        "org.freedesktop.portal.Desktop",
        "org.freedesktop.portal.Settings",
        "SettingChanged",
        "/org/freedesktop/portal/desktop",
        NULL,
        G_DBUS_SIGNAL_FLAGS_NONE,
        portal_setting_changed,
        app,
        NULL
    );
    g_dbus_connection_call(
        connection,
        "org.freedesktop.portal.Desktop",
        "/org/freedesktop/portal/desktop",
        "org.freedesktop.portal.Settings",
        "Read",
        g_variant_new("(ss)", "org.freedesktop.appearance", "color-scheme"),
        G_VARIANT_TYPE("(v)"),
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        NULL,
        portal_read_finished,
        app
    );
}

void ui_watch_color_scheme(GtkApplication *app, GtkWidget *background, GtkWidget *label)
{
    g_object_set_data(G_OBJECT(app), "color-scheme-background", background);
    g_object_set_data(G_OBJECT(app), "color-scheme-label", label);
    g_bus_get(G_BUS_TYPE_SESSION, NULL, session_bus_ready, app);
}

void ui_set_default_window_size(GtkWindow *window)
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
