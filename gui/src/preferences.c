#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include "text_editor.h"
#include "main.h"

GtkWindow *preferences_dialog;
GtkStringList *scheme_list;
GtkDropDown *scheme_dropdown;
GtkButton *ok_button;
GtkStringList *global_theme_list;
GtkDropDown *global_theme_dropdown;

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

/*static void open_options() {
    GtkSourceStyleSchemeManager *scheme_manager = gtk_source_style_scheme_manager_get_default();
    const gchar *const *schemes = gtk_source_style_scheme_manager_get_scheme_ids(scheme_manager);

    // Print the available style schemes
    while (*schemes != NULL) {
        g_print("Available Style Scheme: %s\n", *schemes);
        schemes++;
    }
}*/

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

void preferences_handler(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
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