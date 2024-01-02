/*
* gtk-support.h
*/
#include <gnome.h>
/*
* rm : include per gestione configurazione
*/
#include <stdio.h>
#include <ep-common.h>
#include <ep-db.h>

GtkWidget *lookup_widget(GtkWidget *widget, const gchar *widget_name);

#define get_widget lookup_widget

void add_pixmap_directory(const gchar *directory);

GtkWidget*  create_pixmap(GtkWidget       *widget, const gchar     *filename, gboolean         gnome_pixmap);
GdkImlibImage* create_image(const gchar     *filename);

int set_pixmap(GtkWidget *widget, const gchar *filename);
int gtk_clist_get_first_selected_row(GtkCList *lst);
void gtk_label_printf(GtkWidget *w,char *fmt,...) __attribute__ ((format(printf,2,3)));
void gtk_text_printf(GdkColor *Color,GtkWidget *w,char *fmt,...) __attribute__ ((format(printf,3,4)));
void trace_debug(GdkColor *Color,gboolean bDate,gboolean bNewLine,GtkWidget *txt,char *fmt,...) __attribute__ ((format(printf,5,6)));
void gtk_text_read_file(GtkWidget *w,char *szFileName);
void gtk_update(void);
void gtk_widget_set_foreground(GtkWidget *w,GdkColor *Color);
void gtk_widget_set_background(GtkWidget *w,GdkColor *Color);
void ApplyCfgItems(GtkWidget *dlg,PCFGITEM pItems);
void SetCfgWidgets(GtkWidget *dlg,PCFGITEM pItems);
