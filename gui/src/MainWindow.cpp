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
    gtk_builder_add_from_file(mainWindow->builder, "builder.ui", NULL);

    mainWindow->window = gtk_builder_get_object(mainWindow->builder, "window");

    mainWindow->button = gtk_builder_get_object (mainWindow->builder, "button1");
    g_signal_connect (mainWindow->button, "clicked", G_CALLBACK (print_hello), NULL);


    gtk_window_set_application(GTK_WINDOW(mainWindow->window), app);
    gtk_widget_set_visible(GTK_WIDGET(mainWindow->window), TRUE);

    g_object_unref(mainWindow->builder);
}

void MainWindow::print_hello(GtkWidget *widget, gpointer user_data) {
    g_print("Hello, World!\n");
}
