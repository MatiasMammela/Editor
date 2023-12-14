#include "explorer.h"
#include "text_editor.h"
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

GtkTreeView *tree_view;

void populate_tree_store(GtkTreeStore *treestore, const gchar *path, GtkTreeIter *parent) {
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

void on_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data) {
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
            // g_free(filePath);
            g_free(contents);
        } else {
            g_print("Invalid GFile\n");
        }

        g_free(filename);
    }
}