#include "text_editor.h"
#include "explorer.h"
#include "main.h"
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

GtkNotebook *notebook;
gchar *selectedScheme = NULL;
gchar *selectedGlobalTheme = NULL;
gchar *selectedFontSize = NULL;
GList *tabs = NULL;
static GList *explorerFiles = NULL;

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
    

    // set line numbers
    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(source_view), TRUE);

    // set sourceview to scrolled window
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), source_view);

    // append scrolled window to notebook
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_window, hbox);

    // set the style scheme
    if (selectedScheme == NULL) {
        selectedScheme = "classic";
    }
    GtkSourceStyleScheme *style_scheme = gtk_source_style_scheme_manager_get_scheme(gtk_source_style_scheme_manager_get_default(), selectedScheme);
    gtk_source_buffer_set_style_scheme(tab->buffer, style_scheme);


        //load css file for the source view and apply the #notebook to the tab->source_view

    const char cssPath[] = "main.css";
    GtkCssProvider * cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, cssPath);
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(cssProvider);
    // set the font
    
    //set name of source_view for the source_view
    gtk_widget_set_name(source_view, "source_view");


    // append tab to list
    tabs = g_list_append(tabs, tab);

    // set current page to new page
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook)) - 1);
}

void new_file(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    new_notebook_page("Untitled", "", 0);
}

static void open_file(GtkDialog *dialog, gint response_id, gpointer user_data) {
    if (response_id == GTK_RESPONSE_ACCEPT) {
        GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        char *filePath = g_file_get_path(file);

        // check if file is a directory
        if (g_file_query_file_type(file, G_FILE_QUERY_INFO_NONE, NULL) == G_FILE_TYPE_DIRECTORY) {
            g_print("its a directory\n");
            // Set up the tree view (add columns, populate the store with GFile, etc.)
            GtkTreeStore *treestore = gtk_tree_store_new(NUM_COLUMNS, G_TYPE_POINTER, G_TYPE_STRING, GDK_TYPE_PIXBUF);
            populate_tree_store(treestore, filePath, NULL);
            gtk_tree_view_set_model(tree_view, GTK_TREE_MODEL(treestore));
            g_object_unref(treestore);

            gchar *filename = g_file_get_basename(file);



            

            // use the same column if it exists
            GtkTreeViewColumn *column = gtk_tree_view_get_column(GTK_TREE_VIEW(tree_view), 0);
            if (column == NULL) {
                // create a new column

                GtkCellRenderer *renderer = gtk_cell_renderer_pixbuf_new();
                GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("", renderer, "pixbuf", COLUMN_ICON, NULL);
                gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);


                // Create an empty box for the icon column header
                GtkWidget *empty_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
                gtk_widget_set_name(empty_box, "empty-header");
                gtk_tree_view_column_set_widget(column, empty_box);




                renderer = gtk_cell_renderer_text_new();
                column = gtk_tree_view_column_new_with_attributes("Text", renderer, "text", COLUMN_NAME, NULL);
                gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

                // Create a box for the text column header
                GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
                GtkWidget *label = gtk_label_new(filename);
                gtk_box_append(GTK_BOX(box), label);
                gtk_widget_show(box);
                gtk_tree_view_column_set_widget(column, box);
                gtk_tree_view_set_enable_tree_lines(GTK_TREE_VIEW(tree_view), TRUE);

                g_signal_connect(tree_view, "row-activated", G_CALLBACK(on_row_activated), NULL);
            } else {
                // set the column name
                gtk_tree_view_column_set_title(column, filename);
            }
        } else {

            // if the file is not a directory, open it
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

void save_file(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
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

void saveAs_handler(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
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

void open_file_handler(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    GtkWindow *dialog = GTK_WINDOW(gtk_file_chooser_dialog_new("Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL));
    gtk_window_set_transient_for(dialog, window);
    gtk_window_present(dialog);
    g_signal_connect(dialog, "response", G_CALLBACK(open_file), user_data);
}

void open_folder_handler(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    GtkWindow *dialog = GTK_WINDOW(gtk_file_chooser_dialog_new("Open File", NULL, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL));
    gtk_window_set_transient_for(dialog, window);
    gtk_window_present(dialog);
    g_signal_connect(dialog, "response", G_CALLBACK(open_file), user_data);
}

void exit_handler(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    gtk_window_close(window);
}

void update_actions(GtkNotebook *notebook, GActionMap *action_map) {
    gboolean has_pages = gtk_notebook_get_n_pages(notebook) > 0;
    GAction *save_action = g_action_map_lookup_action(action_map, "save");
    GAction *saveas_action = g_action_map_lookup_action(action_map, "saveas");

    g_simple_action_set_enabled(G_SIMPLE_ACTION(save_action), has_pages);
    g_simple_action_set_enabled(G_SIMPLE_ACTION(saveas_action), has_pages);
}

void update_actions_handler(GtkNotebook *notebook, GtkWidget *child, guint page_num, gpointer user_data) {
    update_actions(notebook, user_data);
}
