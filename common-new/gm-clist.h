#ifndef __GTK_GMCLIST_H__
#define __GTK_GMCLIST_H__






#define GM_CLIST(obj)          GTK_CHECK_CAST (obj, gm_clist_get_type (), GmClist)
#define GM_CLIST_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gm_clist_get_type (), GmClistClass)
#define IS_GM_CLIST(obj)       GTK_CHECK_TYPE (obj, gm_clist_get_type ())


#define	ROW_ELEMENT(clist, row)	(((row) == (clist)->rows - 1) ? \
				 (clist)->row_list_end : \
				 g_list_nth ((clist)->row_list, (row)))

#define ROW_TOP_YPIXEL(clist, row) (((clist)->row_height * (row)) + \
				    (((row) + 1) * CELL_SPACING) + \
				    (clist)->voffset)

enum {
	GMCLIST_TYPE_NONE,
	GMCLIST_TYPE_STRING,
	GMCLIST_TYPE_INTEGER,
	GMCLIST_TYPE_FLOAT,
	GMCLIST_TYPE_DATE,
	GMCLIST_TYPE_TIME,
	GMCLIST_TYPE_DATETIME,
	GMCLIST_TYPE_TIMESTAMP,
	GMCLIST_TYPE_YEAR
};

enum {
    SORT_NONE = 0,
    SORT_UP,
    SORT_DOWN
};

enum{
    SORT_STRINGS = 0,
    SORT_NUMBERS,
};

typedef struct _GmClist       GmClist;
typedef struct _GmClistClass  GmClistClass;
typedef struct _GmColumnInfo GmColumnInfo;

enum {
    GM_MLIST_DEFAULT_MODE=0,
    GM_MLIST_SEARCH_MODE
};


struct _GmClist {
    GtkCList clist;
    GtkWidget * down_arrow_widget;
    GtkWidget * up_arrow_widget;
    GtkWidget * hint_widget;
    gint sort_column;

    gboolean remove_newlines;
    GmColumnInfo *cinfo;

    gint mode;
    gpointer sdata;    /* search data */
};

struct _GmClistClass {
    GtkCListClass parent_class;
    void   (*row_activated)          (GmClist       *mlist,
                                      gint            row);
    void   (*resorted)          (GmClist       *mlist,
                                 gint            col);
};

struct _GmColumnInfo {
    gint  current_sort_direction;
    gint  sort_data_type;
    GtkWidget *down_arrow_widget;
    GtkWidget *up_arrow_widget;
};



guint      gm_clist_get_type (void);
GtkWidget* gm_clist_new      (gint columns);
void gm_clist_set_column_widget (GmClist  *mlist,
                                 gint       column,
                                 GtkWidget *widget);

void gm_clist_set_column_sortdata_type (GmClist  *mlist,
                                        gint       column,
                                        gint type);

void gm_clist_sort_by_column       (GtkCList * clist, gint column, gint direction);
void gm_clist_show_selected_row    (GtkCList * clist);
void gm_clist_select_row           (GtkCList *clist, gint row_pos, gint column);
gint gm_clist_find_row_from_column (GtkCList * clist, gchar * data, gint column);
void gm_clist_set_hint_widget      (GmClist * mlist, GtkWidget * hwidget );

gboolean gm_clist_search (GmClist * mlist, gchar * str, gboolean wrap_around, gboolean case_sensitive);
gchar  * gm_clist_get_column_title (GtkCList * clist, gint i) ;
void gm_clist_invert_selection (GtkCList * clist );




#endif /* __GTK_GMCLIST_H__ */








