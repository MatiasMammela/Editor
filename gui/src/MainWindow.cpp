#include "MainWindow.h"
#include <iostream>
MainWindow *MainWindow::self = nullptr;
MainWindow::MainWindow() {
    // Set 'self' to the current instance
    self = this;

    // Create a new GtkApplication
    application = gtk_application_new("com.example.hello_world", G_APPLICATION_DEFAULT_FLAGS);

    // Connect the "activate" signal to the on_activate function
    g_signal_connect(application, "activate", G_CALLBACK(on_activate), this);

    // Run the application
    g_application_run(G_APPLICATION(application), 0, nullptr);

    // Release resources
    g_object_unref(application);
}

char *MainWindow::get_filename(GtkListItem *item, GFileInfo *info) {
    return G_IS_FILE_INFO(info) ? g_strdup(g_file_info_get_name(info)) : nullptr;
}

void MainWindow::dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data) {
    if (response_id == GTK_RESPONSE_ACCEPT) {
        g_print("Open clicked\n");
        // Get the selected file
        self->filePath = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        // Get the file path
        char *filename = g_file_get_path(self->filePath);
        GError *error = nullptr;
        char *contents = nullptr;
        gsize length = 0;
        // get file contents
        if (g_file_load_contents(self->filePath, nullptr, &contents, &length, nullptr, &error)) {
            // Print the file contents
            g_print("%s\n", contents);
            // Free the file contents
            // add the filecontents to the notebook
            GtkWidget *scrolled_window = gtk_scrolled_window_new();
            gtk_widget_set_hexpand(scrolled_window, TRUE);
            gtk_widget_set_vexpand(scrolled_window, TRUE);
            gtk_notebook_append_page(self->notebook, scrolled_window, gtk_label_new(filename));

            GtkWidget *text_view = gtk_text_view_new();
            gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), TRUE);
            gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), TRUE);
            gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
            gtk_text_view_set_monospace(GTK_TEXT_VIEW(text_view), TRUE);
            gtk_text_view_set_buffer(GTK_TEXT_VIEW(text_view), gtk_text_buffer_new(NULL));
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view)), contents, -1);

            // Use gtk_widget_set_child to set the child widget for the scrolled_window
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), text_view);

            g_free(contents);
        } else {
            // Print the error message
            g_printerr("%s\n", error->message);
            // Free the error
            g_error_free(error);
        }
        // Print the file path
        g_print("%s\n", filename);
    }
    gtk_window_close(GTK_WINDOW(dialog));
}

void MainWindow::open_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    GtkFileChooserDialog *dialog;
    GtkFileChooserAction action_ = GTK_FILE_CHOOSER_ACTION_OPEN;

    // Create a file chooser dialog
    dialog = GTK_FILE_CHOOSER_DIALOG(gtk_file_chooser_dialog_new(
        "Open File",
        GTK_WINDOW(user_data),
        action_,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL));
    // Set transient parent
    gtk_window_set_transient_for(GTK_WINDOW(dialog), NULL);
    // Show the dialog
    gtk_window_present(GTK_WINDOW(dialog));

    // Connect the "response" signal to the dialog_response function
    g_signal_connect(dialog, "response", G_CALLBACK(dialog_response), user_data);
}

void MainWindow::on_activate(GtkApplication *app, gpointer user_data) {
    // Cast user_data to MainWindow pointer
    MainWindow *mainWindow = static_cast<MainWindow *>(user_data);

    // Check if 'self' is null
    if (!self) {
        g_printerr("Error: 'self' is null\n");
        return;
    }

    // Initialize GtkBuilder if 'self->builder' is null
    if (!self->builder) {
        self->builder = gtk_builder_new();
    }

    // Load UI file using the existing builder
    if (!gtk_builder_add_from_file(self->builder, "builder.ui", nullptr)) {
        g_printerr("Error loading UI file\n");
        return;
    }

    // Define actions
    const GActionEntry app_entries[] = {
        {"open", open_activated, NULL, NULL, NULL},
    };

    // Add actions to the application
    g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS(app_entries), app);

    // Get the main window and set up the menubar / notebook
    mainWindow->window = GTK_APPLICATION_WINDOW(gtk_builder_get_object(self->builder, "window"));
    GMenuModel *menubar = G_MENU_MODEL(gtk_builder_get_object(self->builder, "menubar"));
    mainWindow->notebook = GTK_NOTEBOOK(gtk_builder_get_object(self->builder, "notebook"));
    gtk_application_window_set_show_menubar(mainWindow->window, TRUE);
    gtk_application_set_menubar(GTK_APPLICATION(app), menubar);

    // Set the application for the window
    gtk_window_set_application(GTK_WINDOW(mainWindow->window), app);

    // Show the window
    gtk_window_present(GTK_WINDOW(mainWindow->window));
}
