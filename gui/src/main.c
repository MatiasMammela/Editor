#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

static GtkNotebook *notebook;
static GtkWindow *window;
static GtkWindow *preferences_dialog;
static GtkStringList *scheme_list;
static GtkDropDown *scheme_dropdown;
static GtkButton *ok_button;
static char *theme;
GList *tabs = NULL;
struct fileTab {
    char *filePath;
    GtkWidget *scrolled_window;
    GtkWidget *source_view;
    GtkSourceBuffer *buffer;
};
extern GList *tabs;
static void open_file(GtkDialog *dialog, gint response_id, gpointer user_data) {
    if (response_id == GTK_RESPONSE_ACCEPT) {
        GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        char *filePath = g_file_get_path(file);

        GtkSettings *settings = gtk_settings_get_default();
        g_object_set(settings, "gtk-application-prefer-dark-theme", TRUE, NULL);

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

            GtkSourceLanguageManager *language_manager = gtk_source_language_manager_new();
            GtkSourceLanguage *language = gtk_source_language_manager_guess_language(language_manager, filePath, NULL);

            if (language == NULL) {
                g_print("Failed to determine language for file: %s\n", filePath);
                // Add proper error handling or return from the function as needed
                g_free(filePath);
                return;
            }

            GtkSourceBuffer *buffer = gtk_source_buffer_new_with_language(language);
            // set buffer text to sourceview
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer), contents, length);
            tab->buffer = buffer;

            // set buffer text to sourceview
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer), contents, length);

            tabs = g_list_append(tabs, tab);

            // set sourceview to scrolled window
            GtkWidget *source_view = gtk_source_view_new_with_buffer(buffer);
            tab->source_view = source_view;

            // set sourceview to scrolled window
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), source_view);
            // close the dialog
            gtk_window_close(GTK_WINDOW(dialog));
        }
    }
}

/*static void open_options() {
    GtkSourceStyleSchemeManager *scheme_manager = gtk_source_style_scheme_manager_get_default();
    const gchar *const *schemes = gtk_source_style_scheme_manager_get_scheme_ids(scheme_manager);

    // Print the available style schemes
    while (*schemes != NULL) {
        g_print("Available Style Scheme: %s\n", *schemes);
        schemes++;
    }
}*/

static void save_file(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    gint current_tab = gtk_notebook_get_current_page(notebook);
    struct fileTab *tab = g_list_nth_data(tabs, current_tab);
    if (tab == NULL) {
        g_printerr("Error: fileTab is NULL\n");
        return;
    }
    GtkWidget *scrolled_window = tab->scrolled_window;
    if (scrolled_window == NULL) {
        g_printerr("Error: Scrolled window is NULL\n");
        return;
    }

    GtkWidget *source_view = tab->source_view;
    if (source_view == NULL) {
        g_printerr("Error: Text view is NULL\n");
        return;
    }
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(source_view));

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
static void saveAs(GtkDialog *dialog, gint response_id, gpointer user_data) {
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
static void saveAs_handler(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
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
    g_signal_connect(dialog, "response", G_CALLBACK(saveAs), user_data);
}

static void open_file_handler(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    GtkWindow *dialog = GTK_WINDOW(gtk_file_chooser_dialog_new("Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL));
    gtk_window_set_transient_for(dialog, window);
    gtk_window_present(dialog);
    g_signal_connect(dialog, "response", G_CALLBACK(open_file), user_data);
}

static void change_theme(GtkStringList *string_list, const gchar *item, gpointer user_data) {
    // Handle the selected theme here
    g_print("Selected theme: %s\n", item);
    GtkSourceStyleScheme *style_scheme = gtk_source_style_scheme_manager_get_scheme(gtk_source_style_scheme_manager_get_default(), item);

    // Loop through all tabs and set the selected style scheme
    for (GList *l = tabs; l != NULL; l = l->next) {
        struct fileTab *tab = l->data;
        GtkSourceBuffer *buffer = tab->buffer;
        gtk_source_buffer_set_style_scheme(buffer, style_scheme);
    }
}

static void ok_button_clicked(GtkButton *button, gpointer user_data) {
    GtkDropDown *scheme_dropdown = GTK_DROP_DOWN(user_data);

    gint index = gtk_drop_down_get_selected(scheme_dropdown);

    // get the name of the selected item from the scheme_list
    const gchar *selected_pointer = gtk_string_list_get_string(scheme_list, index);

    // get the selected item name from the pointer
    g_print("Selected item: %s\n", (gchar *)selected_pointer);

    change_theme(scheme_list, selected_pointer, NULL);
}
static void preferences_delete_event_handler(GtkWindow *dialog, gint response_id, gpointer user_data) {
    if (response_id == GTK_RESPONSE_DELETE_EVENT) {
        gtk_widget_hide(GTK_WIDGET(dialog));
    }
}
static void preferences_handler(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    gtk_window_set_transient_for(preferences_dialog, window);
    // set window size to 400x400
    gtk_window_set_default_size(preferences_dialog, 400, 400);
    gtk_window_present(preferences_dialog);

    GtkSourceStyleSchemeManager *scheme_manager = gtk_source_style_scheme_manager_get_default();
    const gchar *const *schemes = gtk_source_style_scheme_manager_get_scheme_ids(scheme_manager);
    g_signal_connect(ok_button, "clicked", G_CALLBACK(ok_button_clicked), scheme_dropdown);
    // make the button active
    gtk_widget_set_sensitive(GTK_WIDGET(ok_button), TRUE);
    // Print the available style schemes
    while (*schemes != NULL) {
        gtk_string_list_append(scheme_list, *schemes);
        schemes++;
    }
    g_signal_connect(preferences_dialog, "response", G_CALLBACK(preferences_delete_event_handler), NULL);
}
static void activate(GtkApplication *app, gpointer user_data) {
    GtkBuilder *builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "window.ui", NULL);

    window = GTK_WINDOW(gtk_builder_get_object(builder, "window"));
    notebook = GTK_NOTEBOOK(gtk_builder_get_object(builder, "notebook"));
    preferences_dialog = GTK_WINDOW(gtk_builder_get_object(builder, "preferences_dialog"));
    scheme_list = GTK_STRING_LIST(gtk_builder_get_object(builder, "scheme_list"));
    scheme_dropdown = GTK_DROP_DOWN(gtk_builder_get_object(builder, "scheme_dropdown"));
    ok_button = GTK_BUTTON(gtk_builder_get_object(builder, "ok_button"));

    struct fileTab *tab = NULL;
    const GActionEntry app_entries[] = {
        {"open", open_file_handler, NULL, (gpointer)tab, NULL},
        {"saveas", saveAs_handler, NULL, (gpointer)tab, NULL},
        {"save", save_file, NULL, (gpointer)tab, NULL},
        {"preferences", preferences_handler, NULL, (gpointer)tab, NULL}};
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

    g_object_unref(app);

    return status;
}
