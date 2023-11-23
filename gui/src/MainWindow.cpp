#include "MainWindow.h"

MainWindow::MainWindow() {
    application = gtk_application_new("com.example.hello_world", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(application, "activate", G_CALLBACK(on_activate), this);

    g_application_run(G_APPLICATION(application), 0, nullptr);

    g_object_unref(application);
}

void MainWindow::on_activate(GtkApplication *app, gpointer user_data) {
    MainWindow *mainWindow = static_cast<MainWindow *>(user_data);

    mainWindow->builder = gtk_builder_new();
    if (!gtk_builder_add_from_file(mainWindow->builder, "builder.ui", nullptr)) {
        g_printerr("Error loading UI file\n");
        g_object_unref(mainWindow->builder);
        return;
    }

    mainWindow->window = GTK_WINDOW(gtk_builder_get_object(mainWindow->builder, "window"));
    mainWindow->grid = GTK_GRID(gtk_builder_get_object(mainWindow->builder, "grid"));
    mainWindow->button = GTK_BUTTON(gtk_builder_get_object(mainWindow->builder, "button1"));

    gtk_window_set_title(mainWindow->window, "Your Title");

    gtk_window_set_application(mainWindow->window, app);

    gtk_widget_set_visible(GTK_WIDGET(mainWindow->window), TRUE);

    g_object_unref(mainWindow->builder);
}

void MainWindow::print_hello(GtkWidget *widget, gpointer user_data) {
    g_print("Hello, World!\n");
}
