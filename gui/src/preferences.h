
#ifndef PREFERENCES_H
#define PREFERENCES_H

extern GtkWindow *preferences_dialog;
extern GtkStringList *scheme_list;
extern GtkDropDown *scheme_dropdown;
extern GtkButton *ok_button;
extern GtkStringList *global_theme_list;
extern GtkDropDown *global_theme_dropdown;

const gchar *load_config();
void save_config(const gchar *selectedScheme);
void preferences_handler(GSimpleAction *action, GVariant *parameter, gpointer user_data);


#endif // PREFERENCES_H
