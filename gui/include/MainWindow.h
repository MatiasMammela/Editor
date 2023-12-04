#pragma once

#include <gtk/gtk.h>

class MainWindow {
public:
    MainWindow();

private:
    static void on_activate(GtkApplication *app, gpointer user_data);
    static void open_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data);
    static void dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data);
    static void create_new_notebook_page(GtkWidget *widget, gpointer user_data);
    GtkApplication *application;
    static MainWindow *self;
    GFile *filePath;
    GtkApplicationWindow *window;
    char *get_filename(GtkListItem *item, GFileInfo *info);
    GMenuModel *menu;
    GtkNotebook *notebook;
    GtkBuilder *builder;
};