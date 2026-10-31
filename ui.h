#ifndef FETHER_UI_H
#define FETHER_UI_H

#include <gtk/gtk.h>

void ui_set_default_window_size(GtkWindow *window);
void ui_watch_color_scheme(GtkApplication *app, GtkWidget *background, GtkWidget *label);

#endif
