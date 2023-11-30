#include "MainWindow.h"
#include <iostream>
MainWindow::MainWindow() {
    application = gtk_application_new("com.example.hello_world", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(application, "activate", G_CALLBACK(on_activate), this);

    g_application_run(G_APPLICATION(application), 0, nullptr);

    g_object_unref(application);
}
char *MainWindow::get_filename(GtkListItem *item, GFileInfo *info) {
    return G_IS_FILE_INFO(info) ? g_strdup(g_file_info_get_name(info)) : nullptr;
}
void MainWindow::dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data) {

    if (response_id == GTK_RESPONSE_ACCEPT) {

        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        GFile *file = gtk_file_chooser_get_file(chooser);
        char *filename = g_file_get_path(file);
        g_print("%s\n", filename);
        GFile *filePath = g_file_new_for_path(filename);
        g_free(filename);
        GtkDirectoryList *dl = gtk_directory_list_new("standard::name", filePath);
        g_object_unref(filePath);
    }

    gtk_window_close(GTK_WINDOW(dialog));
}

void MainWindow::open_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    // Create a file choosing dialog
    GtkFileChooserDialog *dialog;
    GtkFileChooserAction action_ = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = GTK_FILE_CHOOSER_DIALOG(gtk_file_chooser_dialog_new("Open File", GTK_WINDOW(user_data), action_, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL));

    gtk_window_present(GTK_WINDOW(dialog));

    g_signal_connect(dialog, "response", G_CALLBACK(dialog_response), user_data);
}

void MainWindow::on_activate(GtkApplication *app, gpointer user_data) {
    MainWindow *mainWindow = static_cast<MainWindow *>(user_data);

    mainWindow->builder = gtk_builder_new();
    if (!gtk_builder_add_from_file(mainWindow->builder, "uusi.ui", nullptr)) {
        g_printerr("Error loading UI file\n");
        g_object_unref(mainWindow->builder);
        return;
    }

    const GActionEntry app_entries[] = {
        {"open", open_activated, NULL, NULL, NULL},
    };

    g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS(app_entries), app);

    // mainWindow->window = gtk_application_window_new (GTK_APPLICATION (app));
    mainWindow->window = GTK_APPLICATION_WINDOW(gtk_builder_get_object(mainWindow->builder, "window"));
    // mainWindow->window = GTK_WINDOW(gtk_builder_get_object(mainWindow->builder, "window"));
    // mainWindow->grid = GTK_GRID(gtk_builder_get_object(mainWindow->builder, "grid"));

    GMenuModel *menubar = G_MENU_MODEL(gtk_builder_get_object(mainWindow->builder, "menubar"));
    gtk_application_window_set_show_menubar(mainWindow->window, TRUE);
    gtk_application_set_menubar(GTK_APPLICATION(app), menubar);

    gtk_window_set_application(GTK_WINDOW(mainWindow->window), app);

    // gtk_widget_set_visible(GTK_WIDGET(mainWindow->window), TRUE);
    gtk_window_present(GTK_WINDOW(mainWindow->window));

    g_object_unref(mainWindow->builder);
}
