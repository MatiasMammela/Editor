#pragma once

#include <gtk/gtk.h>

class MainWindow {
public:
    MainWindow();

private:
    static void on_activate(GtkApplication *app, gpointer user_data);
    static void print_hello(GtkWidget *widget, gpointer user_data);

    GtkApplication *application;
    GtkWindow *window;
    GtkButton *button;
    GtkHeaderBar *headerbar;
    GtkGrid *grid;
    GtkWidget *button_box;
    GtkBuilder *builder;
};
