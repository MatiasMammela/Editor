#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include <glib.h>
static GtkNotebook *notebook;
static GtkWindow *window;
static GtkWindow *preferences_dialog;
static GtkStringList *scheme_list;
static GtkDropDown *scheme_dropdown;
static GtkDropDown *global_theme_dropdown;
static GtkButton *ok_button;
static GtkStringList *global_theme_list;
static gchar *selectedScheme = NULL;


static GList* explorerFiles = NULL;

GList *tabs = NULL;
struct fileTab {
    char *filePath;
    GtkWidget *scrolled_window;
    GtkWidget *source_view;
    GtkSourceBuffer *buffer;
    GtkWidget *label;
};
extern GList *tabs;

const gchar *load_config() {
    GKeyFile *keyfile = g_key_file_new();
    gchar *configData = NULL;

    if (g_file_get_contents("config.ini", &configData, NULL, NULL)) {
        if (g_key_file_load_from_data(keyfile, configData, -1, G_KEY_FILE_NONE, NULL)) {
            selectedScheme = g_key_file_get_string(keyfile, "Preferences", "SelectedScheme", NULL); 
            g_free(configData);
            g_key_file_free(keyfile);
            return selectedScheme;
        }
    }

    g_free(configData);
    g_key_file_free(keyfile);
    return NULL;
}
void save_config(const gchar *selectedScheme) {
    GKeyFile *keyfile = g_key_file_new();
    g_key_file_set_string(keyfile, "Preferences", "SelectedScheme", selectedScheme);

    gchar *configData = g_key_file_to_data(keyfile, NULL, NULL);
    g_file_set_contents("config.ini", configData, -1, NULL);

    //load the config file
    load_config();

    g_free(configData);
    g_key_file_free(keyfile);
}


void remove_tab(GtkWidget *button, gpointer data) {
    struct fileTab *tab = (struct fileTab *)data;
    gint index = gtk_notebook_page_num(notebook, tab->scrolled_window);
    gtk_notebook_remove_page(notebook, index);
    tabs = g_list_remove(tabs, tab);
    g_free(tab);
}

void new_notebook_page(char *filePath, char *contents, gsize length) {
    struct fileTab *tab = g_malloc(sizeof(struct fileTab));
    tab->filePath = filePath;

    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_widget_set_vexpand(scrolled_window, TRUE);

    GtkWidget *close_button = gtk_button_new_from_icon_name("window-close-symbolic");
    gtk_widget_set_margin_start(close_button, 5);
    gtk_button_set_has_frame(GTK_BUTTON(close_button), FALSE);

    GtkWidget *label = gtk_label_new(g_path_get_basename(filePath));
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_append(GTK_BOX(hbox), label);
    gtk_box_append(GTK_BOX(hbox), close_button);

    g_signal_connect(close_button, "clicked", G_CALLBACK(remove_tab), tab);

    GtkSourceLanguageManager *language_manager = gtk_source_language_manager_new();
    GtkSourceLanguage *language = gtk_source_language_manager_guess_language(language_manager, filePath, NULL);




    if (language != NULL) {
        tab->buffer = gtk_source_buffer_new_with_language(language);
    } else {
        tab->buffer = gtk_source_buffer_new(NULL);
    }

    // set buffer text to sourceview
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(tab->buffer), contents, length);

    // set sourceview to scrolled window
    GtkWidget *source_view = gtk_source_view_new_with_buffer(tab->buffer);

    tab->source_view = source_view;
    tab->scrolled_window = scrolled_window;
    tab->label = label;
    
    //set line numbers
    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(source_view), TRUE);

    // set sourceview to scrolled window
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), source_view);

    // append scrolled window to notebook
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_window, hbox);

    //set the style scheme
    GtkSourceStyleScheme *style_scheme = gtk_source_style_scheme_manager_get_scheme(gtk_source_style_scheme_manager_get_default(), selectedScheme);
    gtk_source_buffer_set_style_scheme(tab->buffer, style_scheme);

    // append tab to list
    tabs = g_list_append(tabs, tab);

    // set current page to new page
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook)) - 1);
}

static void open_file(GtkDialog *dialog, gint response_id, gpointer user_data) {
    if (response_id == GTK_RESPONSE_ACCEPT) {
        GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        char *filePath = g_file_get_path(file);
        
        //check if file is a directory
        if(g_file_query_file_type(file, G_FILE_QUERY_INFO_NONE, NULL) == G_FILE_TYPE_DIRECTORY){
            g_print("its a directory\n");
            //get the files in the directory and insert them into the explorer
            GFileEnumerator *enumerator = g_file_enumerate_children(file, G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NONE, NULL, NULL);
            GFileInfo *info = NULL;
            while ((info = g_file_enumerator_next_file(enumerator, NULL, NULL)) != NULL) {
                const char *name = g_file_info_get_name(info);
                char *path = g_build_filename(filePath, name, NULL);
                g_print("File: %s\n", path);
                explorerFiles = g_list_append(explorerFiles, path);
                g_object_unref(info);
            }
        }else{

            //if the file is not a directory, open it
            GError *error = NULL;
            char *contents = NULL;
            gsize length = 0;
            if (g_file_load_contents(file, NULL, &contents, &length, NULL, &error)) {
                new_notebook_page(filePath, contents, length);
            }
        }
    }
    gtk_window_close(GTK_WINDOW(dialog));
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

        // Update the file path in the struct
        gint current_tab = gtk_notebook_get_current_page(notebook);
        struct fileTab *tab = g_list_nth_data(tabs, current_tab);
        tab->filePath = filePath;

        // Update the label
        GtkWidget *label = tab->label;
        gtk_label_set_text(GTK_LABEL(label), g_path_get_basename(filePath));

        g_free(contents);
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

    // if tab is "untitled", open save as dialog
    if (g_strcmp0(tab->filePath, "Untitled") == 0) {
        saveAs_handler(action, parameter, user_data);
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

static void new_file(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    new_notebook_page("Untitled", "", 0);
}

static void change_theme(GtkStringList *string_list, const gchar *item, gpointer user_data) {
    // Handle the selected theme here
    GtkSourceStyleScheme *style_scheme = gtk_source_style_scheme_manager_get_scheme(gtk_source_style_scheme_manager_get_default(), item);

    // Save the selected theme to the config file
    save_config(item);


    // Loop through all tabs and set the selected style scheme
    for (GList *l = tabs; l != NULL; l = l->next) {
        struct fileTab *tab = l->data;
        GtkSourceBuffer *buffer = tab->buffer;
        gtk_source_buffer_set_style_scheme(buffer, style_scheme);
    }
}
static void change_global_theme(const gchar *item, gpointer user_data) {
    // Get the default settings
    GtkSettings *settings = gtk_settings_get_default();

    // Check the selected item and update the theme accordingly

    if (g_strcmp0(item, "Dark") == 0) {
        g_object_set(settings, "gtk-application-prefer-dark-theme", TRUE, NULL);
    } else {
        g_object_set(settings, "gtk-application-prefer-dark-theme", FALSE, NULL);

        g_print("Light theme selected\n");
    }
}
    typedef struct {
        GtkDropDown *scheme_dropdown;
        GtkDropDown *global_theme_dropdown;
    } Settings;

static void ok_button_clicked(GtkButton *button, gpointer user_data) {

    Settings *settings_data = (Settings *)user_data;

    // Get the selected scheme from the dropdown
    gint index = gtk_drop_down_get_selected(settings_data->scheme_dropdown);
    const gchar *selected_pointer = gtk_string_list_get_string(scheme_list, index);
    change_theme(scheme_list, selected_pointer, NULL);

    // Get the selected global theme from the dropdown
    gint global_theme_index = gtk_drop_down_get_selected(settings_data->global_theme_dropdown);
    const gchar *global_theme_pointer = gtk_string_list_get_string(global_theme_list, global_theme_index);
    change_global_theme(global_theme_pointer, NULL);
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


    Settings *settings_data = g_new(Settings, 1);
    settings_data->global_theme_dropdown = global_theme_dropdown;
    settings_data->scheme_dropdown = scheme_dropdown;
    GtkSourceStyleSchemeManager *scheme_manager = gtk_source_style_scheme_manager_get_default();
    const gchar *const *schemes = gtk_source_style_scheme_manager_get_scheme_ids(scheme_manager);
    g_signal_connect(ok_button, "clicked", G_CALLBACK(ok_button_clicked), settings_data);
    // make the button active
    gtk_widget_set_sensitive(GTK_WIDGET(ok_button), TRUE);
    // Print the available style schemes
    /*while (*schemes != NULL) {
        if(g_strcmp0(*schemes, selectedScheme) == 0) {
            //get the index of the selected scheme
            gint index = gtk_string_list_get_string(scheme_list, *schemes);
            gtk_drop_down_set_selected(scheme_dropdown, index);
        }
        gtk_string_list_append(scheme_list, *schemes);
        schemes++;
    }*/
    gint index = 0;

    for(gint i = 0; i < g_strv_length((gchar **)schemes); i++) {
        if(g_strcmp0(schemes[i], selectedScheme) == 0) {
            //get the index of the selected scheme
            g_print("Selected scheme: %s\n", schemes[i]);
            index=i;
        }
        if (gtk_string_list_get_string(scheme_list,i) == NULL) {
            gtk_string_list_append(scheme_list, schemes[i]);
        }
    }
    gtk_drop_down_set_selected(scheme_dropdown, index);
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
    global_theme_list = GTK_STRING_LIST(gtk_builder_get_object(builder, "global_theme_list"));
    global_theme_dropdown = GTK_DROP_DOWN(gtk_builder_get_object(builder, "global_theme_dropdown"));
    struct fileTab *tab = NULL;
    const GActionEntry app_entries[] = {
        {"open", open_file_handler, NULL, (gpointer)tab, NULL},
        {"saveas", saveAs_handler, NULL, (gpointer)tab, NULL},
        {"save", save_file, NULL, (gpointer)tab, NULL},
        {"new", new_file, NULL, (gpointer)tab, NULL},
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
    load_config();



    status = g_application_run(G_APPLICATION(app), argc, argv);



    g_object_unref(app);

    return status;
}
