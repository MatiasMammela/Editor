#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

struct fileTab {
    char *filePath;
    GtkWidget *scrolled_window;
    GtkWidget *source_view;
    GtkSourceBuffer *buffer;
    GtkWidget *label;
};

extern GtkNotebook *notebook;
extern gchar *selectedScheme;
extern GList *tabs;

void new_notebook_page(char *filePath, char *contents, gsize length);
void new_file(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void saveAs_handler(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void save_file(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void open_file_handler(GSimpleAction *action, GVariant *parameter, gpointer user_data);


#endif // TEXT_EDITOR_H