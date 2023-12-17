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
GtkSpinButton *font_size_spinbutton;





const gchar *load_config() {
    
    GKeyFile *keyfile = g_key_file_new();
    gchar *configData = NULL;

    GtkCssProvider *cssProvider;
    GtkStyleContext *context;
    GError *error = NULL;
    /* new css provider */
    cssProvider = gtk_css_provider_new();


    if (g_file_get_contents("config.ini", &configData, NULL, NULL)) {


        if (g_key_file_load_from_data(keyfile, configData, -1, G_KEY_FILE_NONE, NULL)) {

 


            selectedScheme = g_key_file_get_string(keyfile, "Preferences", "SelectedScheme", NULL); 
            selectedGlobalTheme = g_key_file_get_string(keyfile, "Preferences", "SelectedGlobalTheme", NULL);
            selectedFontSize = g_key_file_get_string(keyfile, "Preferences", "FontSize", NULL);


            if(selectedScheme == NULL) {
                selectedScheme = "classic";
            }
            if(selectedGlobalTheme == NULL) {
                selectedGlobalTheme = "none";
            }
            if(selectedFontSize == NULL) {
                selectedFontSize = "12";
            }

            g_free(configData);
            g_key_file_free(keyfile);
            //load the css file

             GString *css = g_string_new(NULL);
             GFile *cssFile = g_file_new_for_path("main.css");



            if(strcmp(selectedGlobalTheme, "Hotpink") == 0) {
            gchar *selectedGlobalTheme_css = "Hotpink";
            g_string_append_printf(css, "#window { background-color: %s;  color:black;}\n", selectedGlobalTheme_css);
            } else if(strcmp(selectedGlobalTheme, "Cyan") == 0) {
            gchar *selectedGlobalTheme_css = "Cyan";
            g_string_append_printf(css, "#window { background-color: %s; color:black;}\n", selectedGlobalTheme_css); 
            }else{
                //remove the #window from the css file
                g_print("initialize");
                g_string_append_printf(css, "#window {background-color:black; color:white;}\n");
            }




            if(selectedFontSize != NULL) {
            g_string_append_printf(css, "#source_view { font-size: %spx; }\n", selectedFontSize);
            }else{
            selectedFontSize = "12";
            g_string_append_printf(css, "#source_view { font-size: %spx; }\n", selectedFontSize);
            }

            g_print(css->str);


            //write to a file called main.css
            g_file_set_contents("main.css", css->str, -1, NULL);

            gtk_css_provider_load_from_path(cssProvider, "main.css");
            gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
            // set the font
            

            g_string_free(css, TRUE);
            g_object_unref(cssProvider);
            
            

            return selectedScheme;
        }
    }




    /* free css provider */
    g_object_unref(cssProvider);
    
    g_free(configData);
    g_key_file_free(keyfile);
    return NULL;
}

void save_config(const gchar *value , const gchar *setting) {
    GKeyFile *keyfile = g_key_file_new();

    // Load existing keys
    if (g_file_test("config.ini", G_FILE_TEST_EXISTS)) {
        g_key_file_load_from_file(keyfile, "config.ini", G_KEY_FILE_NONE, NULL);
    }

    g_key_file_set_string(keyfile, "Preferences", setting, value);
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
static void change_font_size(GtkSpinButton *spin_button, gpointer user_data) {
    // Get the default settings
    GtkSettings *settings = gtk_settings_get_default();

    // Get the selected font size from the spin button
    gint font_size = gtk_spin_button_get_value_as_int(spin_button);

    save_config(g_strdup_printf("%d", font_size) , "FontSize");
}

static void change_theme(GtkStringList *string_list, const gchar *item, gpointer user_data) {
    // Handle the selected theme here
    GtkSourceStyleScheme *style_scheme = gtk_source_style_scheme_manager_get_scheme(gtk_source_style_scheme_manager_get_default(), item);

    // Save the selected theme to the config file
    save_config(item , "SelectedScheme");


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

    if (g_strcmp0(item, "Hotpink") == 0) {
        g_object_set(settings, "gtk-application-prefer-dark-theme", TRUE, NULL);
        save_config(item , "SelectedGlobalTheme");
    } else {
        g_object_set(settings, "gtk-application-prefer-dark-theme", FALSE, NULL);
        save_config(item , "SelectedGlobalTheme");
        g_print("Light theme selected\n");
    }
}

typedef struct {
    GtkDropDown *scheme_dropdown;
    GtkDropDown *global_theme_dropdown;
    GtkSpinButton *font_size_spinbutton;
} Settings;

static void ok_button_clicked(GtkButton *button, gpointer user_data) {
    if(user_data == NULL) {
        g_print("User data is null\n");
    }else{
    Settings *settings_data = (Settings *)user_data;

    // Get the selected scheme from the dropdown
    gint index = gtk_drop_down_get_selected(settings_data->scheme_dropdown);
    const gchar *selected_pointer = gtk_string_list_get_string(scheme_list, index);
    change_theme(scheme_list, selected_pointer, NULL);

    // Get the selected font size from the spin button
    if(settings_data->font_size_spinbutton == NULL) {
        g_print("Font size spin button is null\n");
    }
    gint font_size = gtk_spin_button_get_value_as_int(settings_data->font_size_spinbutton);
    change_font_size(settings_data->font_size_spinbutton, NULL);

    // Get the selected global theme from the dropdown
    gint global_theme_index = gtk_drop_down_get_selected(settings_data->global_theme_dropdown);
    const gchar *global_theme_pointer = gtk_string_list_get_string(global_theme_list, global_theme_index);
    change_global_theme(global_theme_pointer, NULL);
    }   
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
    settings_data->font_size_spinbutton = font_size_spinbutton;
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


    //get the global theme list
    //list of global themes
    const gchar *global_themes[] = {"Hotpink", "Cyan", "None"};


    gint global_theme_index = 0;
    for(gint i = 0; i < g_strv_length((gchar **)global_themes); i++) {
        if(g_strcmp0(global_themes[i], selectedGlobalTheme) == 0) {
            g_print("%d\n", i);
            gtk_drop_down_set_selected(scheme_dropdown, i);
        }
    }



    if(selectedScheme == NULL) {
        selectedScheme = "classic";
    }

    gint index = 0;

    for(gint i = 0; i < g_strv_length((gchar **)schemes); i++) {
        if(g_strcmp0(schemes[i], selectedScheme) == 0) {
            //get the index of the selected scheme
            index=i;
        }
        if (gtk_string_list_get_string(scheme_list,i) == NULL) {
            gtk_string_list_append(scheme_list, schemes[i]);
        }
    }


    //get the global theme list
    

    //set spin button value 
    if(selectedFontSize == NULL) {
        selectedFontSize = "12";
    }
    gtk_spin_button_set_value(font_size_spinbutton, atoi(selectedFontSize));

    gtk_drop_down_set_selected(scheme_dropdown, index);
    g_signal_connect(preferences_dialog, "response", G_CALLBACK(preferences_delete_event_handler), NULL);
}