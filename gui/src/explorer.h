#ifndef EXPLORER_H
#define EXPLORER_H

#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

enum {
    COLUMN_FILE,
    COLUMN_NAME,
    COLUMN_ICON,
    NUM_COLUMNS,
    
};

extern GtkTreeView *tree_view;
GdkPixbuf* get_icon_for_file(GFile *file);
void populate_tree_store(GtkTreeStore *treestore, const gchar *path, GtkTreeIter *parent);
void on_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);

#endif // EXPLORER_H
