/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <gnome.h>
#include <gtk/gtk.h>
#include <gtk/gtktext.h>

#include <trace.h>
#include <gtk-support.h>

/* This is an internally used function to check if a pixmap file exists. */
static gchar *check_file_exists(const gchar *directory, const gchar *filename);

#ifdef OLD_VERSION
/* This is an internally used function to create pixmaps. */
static GtkWidget* create_dummy_pixmap(GtkWidget       *widget, gboolean         gnome_pixmap);
#endif

GtkWidget* lookup_widget (GtkWidget       *widget, const gchar     *widget_name)
{
	GtkWidget *parent, *found_widget;

	if(widget==NULL)
		return NULL;

	if((found_widget = (GtkWidget*) g_object_get_data (G_OBJECT (widget), widget_name))){
		return found_widget;
	}

	for (;;) {
		if (GTK_IS_MENU (widget))
			parent = gtk_menu_get_attach_widget (GTK_MENU (widget));
		else
			parent = widget->parent;
		if (!parent)
			parent = g_object_get_data (G_OBJECT (widget), "GladeParentKey");
		if (parent == NULL)
			break;
		widget = parent;
	}

	found_widget = (GtkWidget*) g_object_get_data (G_OBJECT (widget), widget_name);
	if (!found_widget)
		g_warning ("Widget not found: %s", widget_name);

	return found_widget;
}

/* This is a dummy pixmap we use when a pixmap can't be found. */
static char *dummy_pixmap_xpm[] = {
/* columns rows colors chars-per-pixel */
"1 1 1 1",
"  c None",
/* pixels */
" "
};

GList *pixmaps_directories = NULL;

/* Use this function to set the directory containing installed pixmaps. */
void add_pixmap_directory (const gchar *directory)
{
#ifdef TRACE_DEBUG
	printf("directory=[%s]\n",directory);
#endif
  pixmaps_directories = g_list_prepend (pixmaps_directories, g_strdup (directory));
}


/* derived from code build by glade 
*/
gchar *find_pixmap_file (const gchar *filename)
{
	GList *elem;

	if (!filename)
	return NULL;

	/* We step through each of the pixmaps directory to find it. */
	elem = pixmaps_directories;
	while (elem) {
		gchar *pathname = g_strdup_printf ("%s%s%s", (gchar*)elem->data, G_DIR_SEPARATOR_S, filename);

		if (g_file_test (pathname, G_FILE_TEST_EXISTS)){
			return pathname;
		}
		g_free (pathname);
		elem = elem->next;
	}
	return NULL;
}

GdkPixbuf *pixbuf_new(gchar *filename)
{
	GdkPixbuf *pix = NULL;
	gchar *imagefile = NULL;
	gchar *ptr;
	GError *error = NULL;

	g_return_val_if_fail(filename != NULL, NULL);

	if ((g_ascii_strcasecmp(filename, "") == 0)) return NULL;


	if((ptr=rindex(filename,'/'))){
		filename=(ptr+1);
	}

	imagefile = find_pixmap_file(filename);

	if (!imagefile) {
		g_warning ("Couldn't find pixmap file: %s", filename);
		return NULL;
	}

	pix = gdk_pixbuf_new_from_file(imagefile, &error);
	if (!pix) {
		fprintf (stderr, "Failed to load pixbuf file: %s - error: %s\n", filename, error->message);
		g_error_free (error);
	}  

	g_free(imagefile);

	return  pix;
}


BOOL set_pixmap(GtkWidget *widget, gchar *filename)
{
	GdkPixbuf *pixbuf;

	if((pixbuf=pixbuf_new(filename))){
		gtk_image_set_from_pixbuf(GTK_IMAGE(widget),pixbuf);
		return TRUE;
	}
	return FALSE;
}


/* This is an internally used function to check if a pixmap file exists. */
gchar* check_file_exists (const gchar *directory, const gchar *filename)
{
  gchar *full_filename;
  struct stat s;
  gint status;

  full_filename = (gchar*) g_malloc (strlen (directory) + 1 + strlen (filename) + 1);
  strcpy (full_filename, directory);
  strcat (full_filename, G_DIR_SEPARATOR_S);
  strcat (full_filename, filename);

  status = stat (full_filename, &s);
  if (status == 0 && S_ISREG (s.st_mode))
    return full_filename;

  g_free (full_filename);
  return NULL;
}


/*
* funzione di stampa formattata in un gtk label widget
*/
void gtk_label_printf(GtkWidget *w,char *fmt,...)
{
	va_list args;
	char msgbuf[1024];

	va_start(args,fmt);
	(void)vsprintf(msgbuf,fmt,args);
	va_end(args);

	gtk_label_set_text (GTK_LABEL(w), g_utf8_normalize(msgbuf,-1,G_NORMALIZE_DEFAULT));
}

/*
* funzione di stampa formattata in un gtk text widget
*/
void gtk_text_printf(char *szColor,GtkWidget *w,char *fmt,...)
{
	va_list args;
	char msgbuf[1024];
	GtkTextBuffer *buffer;
	GtkTextTag *tag;
	char szTagName[128];
	GtkTextIter iter; 
	BOOL bOK=FALSE;


	va_start(args,fmt);
	(void)vsprintf(msgbuf,fmt,args);
	va_end(args);
	/*
	gtk_text_insert (GTK_TEXT(w), NULL, Color, NULL, msgbuf, -1);
	gtk_editable_insert_text (GTK_EDITABLE(w), msgbuf, strlen(msgbuf),&nPos);
	if(NULL!=Color){
		gtk_widget_modify_text (w, GTK_STATE_NORMAL, Color);
	}
	*/

	/* ricavo il buffer */
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (w));

	if(szColor && strlen(szColor)){
		sprintf(szTagName,"%s_fg",szColor);
		tag = gtk_text_buffer_create_tag (buffer, NULL, "foreground", szColor, NULL);  
		if(tag){
			gtk_text_buffer_get_iter_at_mark (buffer, &iter, gtk_text_buffer_get_mark (buffer, "insert"));
			gtk_text_buffer_insert_with_tags (buffer, &iter, msgbuf, -1, tag,NULL);
			bOK=TRUE;
		}
	}
	/*
	gtk_text_buffer_set_text (buffer, msgbuf, -1);
	*/
	if(!bOK){
		gtk_text_buffer_insert_at_cursor (buffer, g_utf8_normalize(msgbuf,-1,G_NORMALIZE_DEFAULT), -1);
	}

	gtk_text_buffer_get_iter_at_mark (buffer, &iter, gtk_text_buffer_get_mark (buffer, "insert"));
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW (w), &iter, 0.0, FALSE, 0.0, 0.0);
}

/*
* gtk_text_read_file(w,szFileName)
* legge il file szfilename e lo inserisce nella gtk_text specificata in input
*/
void gtk_text_read_file(GtkWidget *w,char *szFileName)
{
	int fd, num_bytes;
	char *buf;
	struct stat file_status;

	if ((fd = open(szFileName, O_RDONLY)) <= 0)
		return;
	else {
		if (fstat(fd, &file_status) == -1)
			return;
		if ((buf = malloc(file_status.st_size)) == (char *)NULL)
			return;
		if ((num_bytes = read(fd, buf, file_status.st_size)) <= 0)
			return;
	}
	buf[file_status.st_size]='\0';
	close(fd);
	/* gtk_text_insert (GTK_TEXT(w), NULL, NULL, NULL, buf, -1); */
	free(buf);
}


/*
* void gtk_update(void)
*/
void gtk_update(void)
{
  while (gtk_events_pending())
    gtk_main_iteration();
}

void gtk_widget_set_foreground(GtkWidget *w,GdkColor *Color)
{
#ifdef DA_FARE
	GtkStyle *style;

	if(w==(GtkWidget *)NULL){
		return;
	}
	/* set foreground and text to color */
	style = gtk_style_copy(gtk_widget_get_style(w));
	gtk_style_ref(style);
	style->text[GTK_STATE_NORMAL] = *Color;
	style->fg[GTK_STATE_NORMAL] = *Color;
	gtk_widget_set_style(w, style);
	gtk_style_unref(style);
#endif
	gtk_widget_modify_fg(w,GTK_STATE_NORMAL,Color);
}

void gtk_widget_set_background(GtkWidget *w,GdkColor *Color)
{
#ifdef DA_FARE
	GtkStyle *style;

	if(w==(GtkWidget *)NULL){
		return;
	}

	/* set background to color */
	style = gtk_style_copy(gtk_widget_get_style(w));
	gtk_style_ref(style);
	style->bg[GTK_STATE_NORMAL] = *Color;
	gtk_widget_set_style(w, style);
	gtk_style_unref(style);
#endif
	gtk_widget_modify_bg(w,GTK_STATE_NORMAL,Color);
}


void trace_debug(char *szColor,gboolean bDate,gboolean bNewLine,GtkWidget *txt,char *fmt,...)
{
  va_list args;
  char szMessage[1024];
	char tbuf[80];
	char dbuf[80];

  va_start(args,fmt);
  (void)vsprintf(szMessage,fmt,args);
  va_end(args);
#ifdef TRACE
	if(bNewLine){
		trace_out_vstr_date(1,szMessage);
	} else {
		trace_out_vstr(0,szMessage);
	}
#endif
	if(txt){
		strcat(szMessage,"\n");
		gtk_text_printf(szColor,txt,"[%s] [%s] : ", trace_getdate(time((long *)0), dbuf), trace_gettime_msec(time((long *)0), tbuf));
		gtk_text_printf(szColor,txt,szMessage);
	}
}

void SetCfgWidgets(GtkWidget *dlg,PCFGITEM pItems)
{
	int nIndex=0;
	char szTmp[256];
	int nTmp;
	GtkWidget *w;


	while(pItems[nIndex].pszItemName){
		switch(pItems[nIndex].ItemType){
			case CFG_TYPE_BOOL:
				switch(pItems[nIndex].WidgetType){
					case CFG_TOGGLE_BUTTON_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),*(pItems[nIndex].ItemValue.pbBool));
						}
					break;
					case CFG_SPIN_BUTTON_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							gtk_spin_button_set_value(GTK_SPIN_BUTTON(w),*(pItems[nIndex].ItemValue.pbBool));
						}
					break;
					case CFG_ENTRY_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							sprintf(szTmp,"%d", *(pItems[nIndex].ItemValue.pbBool));
							gtk_entry_set_text(GTK_ENTRY(w),szTmp);
						}
					break;
					case CFG_NO_WIDGET:
						/* eliminate compiler warning */
					break;
				}
			break;
			case CFG_TYPE_INT:
				switch(pItems[nIndex].WidgetType){
					case CFG_TOGGLE_BUTTON_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),*(pItems[nIndex].ItemValue.pnInt));
						}
					break;
					case CFG_SPIN_BUTTON_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							gtk_spin_button_set_value(GTK_SPIN_BUTTON(w),*(pItems[nIndex].ItemValue.pnInt));
						}
					break;
					case CFG_ENTRY_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							sprintf(szTmp,"%d", *(pItems[nIndex].ItemValue.pnInt));
							gtk_entry_set_text(GTK_ENTRY(w),szTmp);
						}
					break;
					case CFG_NO_WIDGET:
						/* eliminate compiler warning */
					break;
				}
			break;
			case CFG_TYPE_STRING:
				switch(pItems[nIndex].WidgetType){
					case CFG_TOGGLE_BUTTON_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							nTmp=atoi(pItems[nIndex].ItemValue.pszString);
							gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),nTmp);
						}
					break;
					case CFG_SPIN_BUTTON_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							nTmp=atoi(pItems[nIndex].ItemValue.pszString);
							gtk_spin_button_set_value(GTK_SPIN_BUTTON(w),nTmp);
						}
					break;
					case CFG_ENTRY_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							gtk_entry_set_text(GTK_ENTRY(w),pItems[nIndex].ItemValue.pszString);
						}
					break;
					case CFG_NO_WIDGET:
						/* eliminate compiler warning */
					break;
				}
			break;
			case CFG_TYPE_NONE:
				/* eliminate compiler warning */
			break;
		}
		nIndex++;
	}
}

void ApplyCfgItems(GtkWidget *dlg,PCFGITEM pItems)
{
	int nIndex=0;
	char szTmp[256];
	GtkWidget *w;


	while(pItems[nIndex].pszItemName){
		switch(pItems[nIndex].ItemType){
			case CFG_TYPE_BOOL:
				switch(pItems[nIndex].WidgetType){
					case CFG_TOGGLE_BUTTON_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							*(pItems[nIndex].ItemValue.pbBool) = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w));
						}
					break;
					case CFG_SPIN_BUTTON_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							*(pItems[nIndex].ItemValue.pbBool) = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
						}
					break;
					case CFG_ENTRY_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							strcpy(szTmp,gtk_entry_get_text(GTK_ENTRY(w)));
							*(pItems[nIndex].ItemValue.pbBool) = atoi(szTmp);
						}
					break;
					case CFG_NO_WIDGET:
						/* eliminate compiler warning */
					break;
				}
			break;
			case CFG_TYPE_INT:
				switch(pItems[nIndex].WidgetType){
					case CFG_TOGGLE_BUTTON_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							*(pItems[nIndex].ItemValue.pnInt) = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w));
						}
					break;
					case CFG_SPIN_BUTTON_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							*(pItems[nIndex].ItemValue.pnInt) = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
						}
					break;
					case CFG_ENTRY_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							strcpy(szTmp,gtk_entry_get_text(GTK_ENTRY(w)));
							*(pItems[nIndex].ItemValue.pnInt) = atoi(szTmp);
						}
					break;
					case CFG_NO_WIDGET:
						/* eliminate compiler warning */
					break;
				}
			break;
			case CFG_TYPE_STRING:
				switch(pItems[nIndex].WidgetType){
					case CFG_TOGGLE_BUTTON_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							sprintf(pItems[nIndex].ItemValue.pszString,"%d",gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)));
						}
					break;
					case CFG_SPIN_BUTTON_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							sprintf(pItems[nIndex].ItemValue.pszString,"%d",gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w)));
						}
					break;
					case CFG_ENTRY_WIDGET:
						if((w=get_widget(dlg,pItems[nIndex].pszWidgetName))){
							strcpy(pItems[nIndex].ItemValue.pszString,gtk_entry_get_text(GTK_ENTRY(w)));
						}
					break;
					case CFG_NO_WIDGET:
						/* eliminate compiler warning */
					break;
				}
			break;
			case CFG_TYPE_NONE:
				/* eliminate compiler warning */
			break;
		}
		nIndex++;
	}
}

GtkWidget* create_pixmap(GtkWidget *widget, gchar *filename)
{
	GdkPixbuf *pixbuf;
	GtkWidget *w;

  w=gtk_image_new ();

	if((pixbuf=pixbuf_new(filename))){
		gtk_image_set_from_pixbuf(GTK_IMAGE(w),pixbuf);
	}
	return w;
}

/* This is an internally used function to create pixmaps. */
GdkPixbuf* create_pixbuf                          (const gchar     *filename)
{
	gchar *pathname = NULL;
	GdkPixbuf *pixbuf;
	GError *error = NULL;

	if (!filename || !filename[0])
		return NULL;

	pathname = gnome_program_locate_file (NULL, GNOME_FILE_DOMAIN_APP_PIXMAP, filename, TRUE, NULL);

	if (!pathname) {
		g_warning ("Couldn't find pixmap file: %s", filename);
		return NULL;
	}

	pixbuf = gdk_pixbuf_new_from_file (pathname, &error);
	if (!pixbuf) {
		fprintf (stderr, "Failed to load pixbuf file: %s: %s\n",
		pathname, error->message);
		g_error_free (error);
	}
	g_free (pathname);

	return pixbuf;
}

/* This is used to set ATK action descriptions. */
void glade_set_atk_action_description       (AtkAction       *action, const gchar     *action_name, const gchar     *description)
{
	gint n_actions, i;

	n_actions = atk_action_get_n_actions (action);
	for (i = 0; i < n_actions; i++) {
		if (!strcmp (atk_action_get_name (action, i), action_name))
			atk_action_set_description (action, i, description);
	}
}

void gtk_tree_model_get_text(GtkTreeModel *tree_model, GtkTreeIter *iter, gint column, char **value)
{
	GType type;
	int nValue;
	float fValue;
	char *pszValue;

	type=gtk_tree_model_get_column_type(tree_model, column);

	switch(type){
		case G_TYPE_STRING: gtk_tree_model_get(tree_model,iter,column,&pszValue,-1);*value=g_strdup(pszValue);g_free(pszValue);break;
		case G_TYPE_FLOAT:  gtk_tree_model_get(tree_model,iter,column,&fValue,-1);*value=g_strdup_printf("%.2f",fValue);break;
		case G_TYPE_INT:    gtk_tree_model_get(tree_model,iter,column,&nValue,-1);*value=g_strdup_printf("%d",nValue);break;
		default: *value=g_strdup("NULL");break;
	}
}

void gtk_label_pango_printf(GtkLabel *label,char *pszCfgFileName,char *fmt,...)
{
  va_list args;
  char szMessage[1024];
	char szBuffer[1024];
	char szPangoItem[128];
	char szPangoValue[256];

  va_start(args,fmt);
  (void)vsprintf(szMessage,fmt,args);
  va_end(args);
	
	sprintf(szPangoItem,"%s_pango_prefix",gtk_widget_get_name(GTK_WIDGET(label)));
	GetFileString("pango markups", szPangoItem, "", szPangoValue, 128,pszCfgFileName,NULL); 
	strcpy(szBuffer,szPangoValue);
	strcat(szBuffer,szMessage);
	sprintf(szPangoItem,"%s_pango_postfix",gtk_widget_get_name(GTK_WIDGET(label)));
	GetFileString("pango markups", szPangoItem, "", szPangoValue, 128,pszCfgFileName,NULL); 
	strcat(szBuffer,szPangoValue);
	gtk_label_set_markup (label,szBuffer);

}
