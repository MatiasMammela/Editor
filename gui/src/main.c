#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include "text_editor.h"
#include "preferences.h"
#include "explorer.h"
GtkWindow *window;

static void activate(GtkApplication *app, gpointer user_data) {
    GtkBuilder *builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "window.ui", NULL);

    window = GTK_WINDOW(gtk_builder_get_object(builder, "window"));
    gtk_window_set_title(window, "Notepad++ mutta huonompi.");
    notebook = GTK_NOTEBOOK(gtk_builder_get_object(builder, "notebook"));
    preferences_dialog = GTK_WINDOW(gtk_builder_get_object(builder, "preferences_dialog"));
    scheme_list = GTK_STRING_LIST(gtk_builder_get_object(builder, "scheme_list"));
    scheme_dropdown = GTK_DROP_DOWN(gtk_builder_get_object(builder, "scheme_dropdown"));
    ok_button = GTK_BUTTON(gtk_builder_get_object(builder, "ok_button"));
    global_theme_list = GTK_STRING_LIST(gtk_builder_get_object(builder, "global_theme_list"));
    global_theme_dropdown = GTK_DROP_DOWN(gtk_builder_get_object(builder, "global_theme_dropdown"));
    tree_view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tree_view"));
    font_size_spinbutton=GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "font_size_spin"));
    struct fileTab *tab = NULL;
    const GActionEntry app_entries[] = {
        {"open", open_file_handler, NULL, (gpointer)tab, NULL},
        {"open_folder", open_folder_handler, NULL, (gpointer)tab, NULL},
        {"saveas", saveAs_handler, NULL, (gpointer)tab, NULL},
        {"save", save_file, NULL, (gpointer)tab, NULL},
        {"new", new_file, NULL, (gpointer)tab, NULL},
        {"preferences", preferences_handler, NULL, (gpointer)tab, NULL},
        {"exit", exit_handler, NULL, (gpointer)tab, NULL},
    };





    g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS(app_entries), app);

    gtk_application_set_accels_for_action(app, "app.open", (const gchar *[]){"<Ctrl>o", NULL});
    gtk_application_set_accels_for_action(app, "app.open_folder", (const gchar *[]){"<Ctrl><Shift>o", NULL});
    gtk_application_set_accels_for_action(app, "app.saveas", (const gchar *[]){"<Ctrl><Shift>s", NULL});
    gtk_application_set_accels_for_action(app, "app.save", (const gchar *[]){"<Ctrl>s", NULL});
    gtk_application_set_accels_for_action(app, "app.new", (const gchar *[]){"<Ctrl>n", NULL});
    gtk_application_set_accels_for_action(app, "app.preferences", (const gchar *[]){"<Ctrl>p", NULL});
    gtk_application_set_accels_for_action(app, "app.exit", (const gchar *[]){"<Ctrl>q", NULL});

    GMenuModel *menubar = G_MENU_MODEL(gtk_builder_get_object(builder, "menubar"));
    gtk_application_set_menubar(GTK_APPLICATION(app), menubar);

    load_config();


    update_actions(notebook, G_ACTION_MAP(app));
    g_signal_connect(notebook, "page-added", G_CALLBACK(update_actions_handler), app);
    g_signal_connect(notebook, "page-removed", G_CALLBACK(update_actions_handler), app);

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
