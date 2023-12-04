#include <gtk/gtk.h>

static GtkNotebook *notebook;
static GtkWindow *window;
struct fileTab {
    char *filePath;
    GtkWidget *scrolled_window;
    GtkWidget *text_view;
    GtkTextBuffer *buffer;
};
GList *tabs = NULL;
static void dialog_response_saveAs(GtkDialog *dialog, gint response_id, gpointer user_data) {
    if (response_id == GTK_RESPONSE_ACCEPT) {
        GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        char *filePath = g_file_get_path(file);
        g_print("File selected: %s\n", filePath);

        GtkWidget *scrolled_window = gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook));
        GtkWidget *text_view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(scrolled_window));
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(buffer, &start);
        gtk_text_buffer_get_end_iter(buffer, &end);
        char *contents = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

        GError *error = NULL;
        if (!g_file_set_contents(filePath, contents, -1, &error)) {
            g_printerr("Error saving file: %s\n", error->message);
            g_error_free(error);
        }

        g_free(contents);
        g_free(filePath);
        g_object_unref(file);
    }
    gtk_window_close(GTK_WINDOW(dialog));
}

static void saveAs_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    GtkWidget *scrolled_window = gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook));
    GtkWidget *text_view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(scrolled_window));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    char *contents = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    // Create a file chooser dialog for saving the file
    GtkWindow *dialog = GTK_WINDOW(gtk_file_chooser_dialog_new("Save File", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT, NULL));
    gtk_window_set_transient_for(dialog, window);

    // Show the dialog
    gtk_window_present(dialog);
    // Connect the response signal to the handling function
    g_signal_connect(dialog, "response", G_CALLBACK(dialog_response_saveAs), user_data);
}
extern GList *tabs;
static void dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data) {
    if (response_id == GTK_RESPONSE_ACCEPT) {
        GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        char *filePath = g_file_get_path(file);

        GError *error = NULL;
        char *contents = NULL;
        gsize length = 0;

        if (g_file_load_contents(file, NULL, &contents, &length, NULL, &error)) {

            struct fileTab *tab = g_malloc(sizeof(struct fileTab));
            tab->filePath = filePath;

            GtkWidget *scrolled_window = gtk_scrolled_window_new();
            gtk_widget_set_hexpand(scrolled_window, TRUE);
            gtk_widget_set_vexpand(scrolled_window, TRUE);
            gtk_notebook_append_page(notebook, scrolled_window, gtk_label_new(g_path_get_basename(filePath)));

            tab->scrolled_window = scrolled_window;

            GtkWidget *text_view = gtk_text_view_new();
            gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), TRUE);
            gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), TRUE);
            gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
            gtk_text_view_set_monospace(GTK_TEXT_VIEW(text_view), TRUE);
            gtk_text_view_set_buffer(GTK_TEXT_VIEW(text_view), gtk_text_buffer_new(NULL));
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view)), contents, -1);

            tab->text_view = text_view;

            tabs = g_list_append(tabs, tab);

            // Use gtk_widget_set_child to set the child widget for the scrolled_window
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), text_view);
        } else {
            g_printerr("%s\n", error->message);
            g_error_free(error);
        }

        g_free(contents);
        g_object_unref(file);
    }
    gtk_window_close(GTK_WINDOW(dialog));
}
static void save_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    // Cast the user data to the fileTab structure
    // get the current tab index
    gint current_tab = gtk_notebook_get_current_page(notebook);
    struct fileTab *tab = g_list_nth_data(tabs, current_tab);

    // Check if tab is not NULL to avoid potential issues
    if (tab == NULL) {
        g_printerr("Error: fileTab is NULL\n");
        return;
    }

    GtkWidget *scrolled_window = tab->scrolled_window;
    if (scrolled_window == NULL) {
        g_printerr("Error: Scrolled window is NULL\n");
        return;
    }

    GtkWidget *text_view = tab->text_view;
    if (text_view == NULL) {
        g_printerr("Error: Text view is NULL\n");
        return;
    }

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    // Check if buffer is not NULL to avoid potential issues
    if (buffer == NULL) {
        g_printerr("Error: Text buffer is NULL\n");
        return;
    }

    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    // Get the contents of the buffer
    char *contents = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    g_print("Contents: %s\n", contents);

    // Get the file path from the struct
    char *filePath = tab->filePath;

    g_print("File selected: %s\n", filePath);

    // Save the file at the path
    GError *error = NULL;
    if (!g_file_set_contents(filePath, contents, -1, &error)) {
        g_printerr("Error saving file: %s\n", error->message);
        g_error_free(error);
    }

    // Deallocate the memory
    g_free(contents);
}

static void open_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    GtkWindow *dialog = GTK_WINDOW(gtk_file_chooser_dialog_new("Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL));
    gtk_window_set_transient_for(dialog, window);
    gtk_window_present(dialog);
    g_signal_connect(dialog, "response", G_CALLBACK(dialog_response), user_data);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkBuilder *builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "window.ui", NULL);

    window = GTK_WINDOW(gtk_builder_get_object(builder, "window"));
    notebook = GTK_NOTEBOOK(gtk_builder_get_object(builder, "notebook"));
    struct fileTab *tab = NULL;
    const GActionEntry app_entries[] = {
        {"open", open_activated, NULL, (gpointer)tab, NULL},
        {"saveas", saveAs_activated, NULL, (gpointer)tab, NULL},
        {"save", save_activated, NULL, (gpointer)tab, NULL},
    };
    g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS(app_entries), app);

    GMenuModel *menubar = G_MENU_MODEL(gtk_builder_get_object(builder, "menubar"));
    gtk_application_set_menubar(GTK_APPLICATION(app), menubar);

    gtk_window_set_application(GTK_WINDOW(window), app);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.gtk4", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    // if program exits, free the memory
    // Free memory associated with tabs

    // TODO :: Poista 8 riviä alla olevaa koodia, jotta ohjelma ei kaadu.
    GListModel *pages_model = gtk_notebook_get_pages(notebook);

    for (gint i = 0; i < g_list_model_get_n_items(pages_model); i++) {
        struct fileTab *tab = (struct fileTab *)g_object_get_data(G_OBJECT(g_list_model_get_item(pages_model, i)), "fileTab");

        // Free individual components
        g_free(tab->filePath);
        // g_free(tab);  // Comment out this line to prevent double-free
    }

    g_object_unref(pages_model);

    g_object_unref(app);

    return status;
}