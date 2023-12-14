#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include "text_editor.h"
#include "preferences.h"
GtkWindow *window;
static GtkTreeView *tree_view;

enum {
    COLUMN_FILE,
    COLUMN_NAME,
    NUM_COLUMNS
};

void populate_tree_store(GtkTreeStore *treestore, const gchar *path, GtkTreeIter *parent)
{
    GDir *dir = g_dir_open(path, 0, NULL);
    if (!dir)
        return;

    const gchar *filename;
    while ((filename = g_dir_read_name(dir)) != NULL) {
        if (g_str_equal(filename, ".") || g_str_equal(filename, ".."))
            continue;

        gchar *full_path = g_build_filename(path, filename, NULL);

        GtkTreeIter iter;
        gtk_tree_store_append(treestore, &iter, parent);
        
        GFile *file = g_file_new_for_path(full_path);
        gtk_tree_store_set(treestore, &iter, COLUMN_FILE, file, COLUMN_NAME, filename, -1);

        if (g_file_test(full_path, G_FILE_TEST_IS_DIR)) {
            populate_tree_store(treestore, full_path, &iter);
        }

        g_free(full_path);
    }

    g_dir_close(dir);
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
    if(selectedScheme == NULL) {
        selectedScheme = "classic";
    }
    GtkSourceStyleScheme *style_scheme = gtk_source_style_scheme_manager_get_scheme(gtk_source_style_scheme_manager_get_default(), selectedScheme);
    gtk_source_buffer_set_style_scheme(tab->buffer, style_scheme);

    GtkCssProvider *cssProvider;
    GtkStyleContext *context;
    GError *error = NULL;
    /* new css provider */
    cssProvider = gtk_css_provider_new(); 


    /* widget name for css syntax */
    gtk_widget_set_name (GTK_WIDGET(tab->source_view), "cssView");   

    /* load css file */
    gtk_css_provider_load_from_path (cssProvider, "main.css"); 

    /* get GtkStyleContext from widget   */
    context = gtk_widget_get_style_context(GTK_WIDGET(tab->source_view));  

    /* finally load style provider */
    gtk_style_context_add_provider(context,    
                                GTK_STYLE_PROVIDER(cssProvider), 
                                GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);


    // append tab to list
    tabs = g_list_append(tabs, tab);

    // set current page to new page
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook)) - 1);


}

void on_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    // Get the model associated with the tree view
    model = gtk_tree_view_get_model(treeview);

    // Retrieve the data for the activated row
    if (gtk_tree_model_get_iter(model, &iter, path)) {
        gchar *filename;

        // Assuming the data for the file is stored as GFile in column 0
        GFile *file;
        gtk_tree_model_get(model, &iter, COLUMN_FILE, &file, COLUMN_NAME, &filename, -1);

        // Check if the GFile is valid
        if (G_IS_FILE(file)) {
            // Get the path from the GFile
            gchar *filePath = g_file_get_path(file);

            GError *error = NULL;
            char *contents = NULL;
            gsize length = 0;

            // Use g_file_load_contents to read the file contents
            if (g_file_load_contents(file, NULL, &contents, &length, NULL, &error)) {
                // Assuming new_notebook_page is a function you have to create for your application
                new_notebook_page(filePath, contents, length);
            } else {
                g_print("Error loading file contents: %s\n", error->message);
                g_error_free(error);
            }

            // Now you have the filename, you can perform actions like opening the file
            g_print("Activated: %s\n", filename);

            // Don't forget to free the allocated memory
            g_free(filePath);
            g_free(contents);
        } else {
            g_print("Invalid GFile\n");
        }

        g_free(filename);
    }
}
static void open_file(GtkDialog *dialog, gint response_id, gpointer user_data) {
    if (response_id == GTK_RESPONSE_ACCEPT) {
        GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        char *filePath = g_file_get_path(file);
        
        //check if file is a directory
        if(g_file_query_file_type(file, G_FILE_QUERY_INFO_NONE, NULL) == G_FILE_TYPE_DIRECTORY){
            g_print("its a directory\n");
                // Set up the tree view (add columns, populate the store with GFile, etc.)
                GtkTreeStore *treestore = gtk_tree_store_new(NUM_COLUMNS, G_TYPE_POINTER, G_TYPE_STRING);
                populate_tree_store(treestore, filePath, NULL);
                gtk_tree_view_set_model(tree_view, GTK_TREE_MODEL(treestore));
                g_object_unref(treestore);

                //get the name of the directory
                gchar *filename = g_file_get_basename(file);

                GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
                GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(filename, renderer, "text", COLUMN_NAME, NULL);
                gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
                g_signal_connect(tree_view, "row-activated", G_CALLBACK(on_row_activated), NULL);
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
    tree_view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tree_view"));
    
    //populate the tree view
    /*
    GtkTreeStore *treestore = gtk_tree_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_INT);
    populate_tree_store(treestore, ".", NULL);
    gtk_tree_view_set_model(tree_view, GTK_TREE_MODEL(treestore));
    g_object_unref(treestore);


    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("File Name", renderer, "text", COLUMN_NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);*/

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
