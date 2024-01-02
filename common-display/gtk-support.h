/*
* gtk-support.h
*/
#include <gtk/gtk.h>
/*
* rm : include per gestione configurazione
*/
#include <stdio.h>
#include <ep-common.h>
#include <ep-db.h>

GtkWidget *lookup_widget(GtkWidget *widget, const gchar *widget_name);

#define get_widget lookup_widget
// #define get_widget glade_xml_get_widget

#define EP_HOOKUP_OBJECT(component,widget,name) g_object_set_data_full (G_OBJECT (component), name, gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

void add_pixmap_directory(const gchar *directory);

/* GdkImlibImage* create_image(const gchar     *filename); */

BOOL set_pixmap(GtkWidget *widget, gchar *filename);
void gtk_label_printf(GtkWidget *w,char *fmt,...) __attribute__ ((format(printf,2,3)));
void gtk_text_printf(char *szColor,GtkWidget *w,char *fmt,...) __attribute__ ((format(printf,3,4)));
void trace_debug(char *szColor,gboolean bDate,gboolean bNewLine,GtkWidget *txt,char *fmt,...) __attribute__ ((format(printf,5,6)));
void gtk_text_read_file(GtkWidget *w,char *szFileName);
void gtk_update(void);
void gtk_widget_set_foreground(GtkWidget *w,GdkColor *Color);
void gtk_widget_set_background(GtkWidget *w,GdkColor *Color);
void ApplyCfgItems(GtkWidget *dlg,PCFGITEM pItems);
void SetCfgWidgets(GtkWidget *dlg,PCFGITEM pItems);
void gtk_label_pango_printf(GtkLabel *label,char *pszCfgFileName,char *fmt,...) __attribute__ ((format(printf,3,4)));;

/*
 * Private Functions.
 */

/* This is used to create the pixmaps used in the interface. */
GtkWidget*  create_pixmap              (GtkWidget       *widget, gchar     *filename);

/* This is used to create the pixbufs used in the interface. */
GdkPixbuf*  create_pixbuf              (const gchar     *filename);

/* This is used to set ATK action descriptions. */
void glade_set_atk_action_description (AtkAction       *action, const gchar     *action_name, const gchar     *description);

void gtk_tree_model_get_text(GtkTreeModel *tree_model, GtkTreeIter *iter, gint column, char **value);
