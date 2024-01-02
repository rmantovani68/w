/*
 *   Copyright (C) 2001  mohamed Elassadi <mohamed@users.sourceforge.net>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*changes made to v0.0.9b1*/


#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
// #include <gnome.h>
// #include "gm-enums.h"
#include "gm-clist.h"
#include "arrows.xpm"

/*	
extern GmApp *global_app;
*/



#define CELL_SPACING 1
#define COLUMN_INSET 3
#define STRING_FIELD 500


enum {
	ROW_ACTIVATED,
	RESORTED,
	LAST_SIGNAL
};

static guint mlist_signals[LAST_SIGNAL] = { 0 };

typedef struct _GmSearchData GmSearchData;

struct _GmSearchData
{
	GString *search_string;
	gboolean wrap_around;
	gboolean case_sensitive;
	gint hit_row;
	gint hit_col;
};



static void get_cell_style(GtkCList * clist, GtkCListRow * clist_row, gint state, gint column, GtkStyle ** style, GdkGC ** fg_gc, GdkGC ** bg_gc);
static gint draw_cell_pixmap(GdkWindow * window, GdkRectangle * clip_rectangle, GdkGC * fg_gc, GdkPixmap * pixmap, GdkBitmap * mask, gint x, gint y, gint width, gint height);
static void gm_clist_class_init(GmClistClass * klass);
static void gm_clist_init(GmClist * mlist);
static gint gm_clist_button_press(GtkWidget * widget, GdkEventButton * event);
static gint gm_clist_key_press(GtkWidget * widget, GdkEventKey * event);
static void gm_clist_size_allocate(GtkWidget * widget, GtkAllocation * allocation);
static void gm_clist_size_request(GtkWidget * widget, GtkRequisition * requisition);
static void gm_clist_destroy(GtkObject * object);
static void draw_row(GtkCList * clist, GdkRectangle * area, gint row, GtkCListRow * clist_row);
static void gm_clist_realize(GtkWidget * widget);
static void gm_clist_click_column(GtkCList * clist, gint column);
static GtkWidget *clist_create_arrow_widget(GtkWidget * widget, gboolean down);
static gint compare_rows(GtkCList * clist, gconstpointer ptr1, gconstpointer ptr2);
static gint clist_button_press(GtkWidget * widget, GdkEventButton * event, gpointer data);
static gpointer gm_utils_get_data(gpointer obj, gchar * key);
static gboolean gm_utils_istrstr(gchar * src, gchar * str);


static GtkWidgetClass *parent_class = NULL;

guint gm_clist_get_type(void)
{
	static guint clist_type = 0;

	if (!clist_type) {
		static const GtkTypeInfo clist_info = {
			"GmClist",
			sizeof(GmClist),
			sizeof(GmClistClass),
			(GtkClassInitFunc) gm_clist_class_init,
			(GtkObjectInitFunc) gm_clist_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		clist_type = gtk_type_unique(gtk_clist_get_type(), &clist_info);
	}

	return clist_type;
}



static void gm_clist_class_init(GmClistClass * class)
{
	GtkWidgetClass *widget_class;
	GtkObjectClass *object_class;
	GtkCListClass *klass;

	klass = GTK_CLIST_CLASS(class);
	widget_class = (GtkWidgetClass *) class;
	parent_class = gtk_type_class(gtk_clist_get_type());
	object_class = (GtkObjectClass *) class;

	widget_class->button_press_event = gm_clist_button_press;
	widget_class->key_press_event = gm_clist_key_press;
	widget_class->size_request = gm_clist_size_request;
	widget_class->size_allocate = gm_clist_size_allocate;
	widget_class->realize = gm_clist_realize;

	klass->draw_row = draw_row;
	klass->click_column = gm_clist_click_column;
	object_class->destroy = gm_clist_destroy;

	mlist_signals[ROW_ACTIVATED] = gtk_signal_new("row_activated", GTK_RUN_FIRST, object_class->type, GTK_SIGNAL_OFFSET(GmClistClass, row_activated), gtk_marshal_NONE__INT, GTK_TYPE_NONE, 1, GTK_TYPE_INT);

	mlist_signals[RESORTED] = gtk_signal_new("resorted", GTK_RUN_FIRST, object_class->type, GTK_SIGNAL_OFFSET(GmClistClass, resorted), gtk_marshal_NONE__INT_INT, GTK_TYPE_NONE, 2, GTK_TYPE_INT, GTK_TYPE_INT);

	gtk_object_class_add_signals(object_class, mlist_signals, LAST_SIGNAL);

}


static void gm_clist_init(GmClist * mlist)
{

	mlist->sort_column = -1;
	mlist->mode = GM_MLIST_DEFAULT_MODE;
	mlist->hint_widget = NULL;
	mlist->sdata = NULL;
	GTK_CLIST(mlist)->compare = compare_rows;
}

GtkWidget *gm_clist_new(gint columns)
{

	GmClist *gmclist;
	GtkCList *clist;
	GmColumnInfo *cinfo;
	gint i;

	gmclist = gtk_type_new(gm_clist_get_type());
	clist = GTK_CLIST(gmclist);



	cinfo = g_new(GmColumnInfo, columns);

	for (i = 0; i < columns; i++) {
		cinfo[i].sort_data_type = GMCLIST_TYPE_STRING;
		cinfo[i].current_sort_direction = SORT_NONE;
		cinfo[i].down_arrow_widget = NULL;
		cinfo[i].up_arrow_widget = NULL;
	}
	gmclist->cinfo = cinfo;
	gtk_clist_construct(clist, columns, NULL);
	return GTK_WIDGET(gmclist);
}

static void clist_activated(GtkCList * clist)
{
	GList *list;
	gint row;

	g_return_if_fail(clist != NULL);
	g_return_if_fail(clist->selection != NULL);


	list = g_list_first(clist->selection);
	row = GPOINTER_TO_INT(list->data);
	gtk_signal_emit(GTK_OBJECT(clist), mlist_signals[ROW_ACTIVATED], row);
}

static void gm_clist_move_to_first_row(GtkCList * clist)
{
	clist->focus_row = 0;
	gtk_clist_unselect_all(clist);
	gtk_clist_select_row(clist, 0, 0);
	gtk_clist_moveto(clist, 0, 0, 0.5, 0.0);
}

static void gm_clist_move_to_last_row(GtkCList * clist)
{
	clist->focus_row = clist->rows - 1;
	gtk_clist_unselect_all(clist);
	gtk_clist_select_row(clist, clist->rows - 1, 0);
	gtk_clist_moveto(clist, clist->rows - 1, 0, 0.5, 0.0);
}


static gint gm_clist_key_press(GtkWidget * widget, GdkEventKey * event)
{
	GtkCList *clist;
	GmClist *mlist;
	gint key;
	key = 0;

	clist = GTK_CLIST(widget);
	mlist = GM_CLIST(widget);

	/* control mask */
	if (event->state & GDK_CONTROL_MASK) {
		switch (event->keyval) {
		case GDK_Begin:
		case GDK_Home:
			{
				gm_clist_move_to_first_row(clist);
				gtk_signal_emit_stop_by_name(GTK_OBJECT(widget), "key_press_event");
				return TRUE;
				break;
			}
		case GDK_End:
			{
				gm_clist_move_to_last_row(clist);
				gtk_signal_emit_stop_by_name(GTK_OBJECT(widget), "key_press_event");
				return TRUE;
				break;
			}
		}
	}

	/* no mask */
	switch (event->keyval) {
		case GDK_Return:
			if (!clist->selection)
				return FALSE;
			clist_activated(clist);
			return TRUE;
		break;
	}
	if ((*GTK_WIDGET_CLASS(parent_class)->key_press_event) (widget, event)) {
		return TRUE;
	}
	return FALSE;
}


static gint gm_clist_button_press(GtkWidget * widget, GdkEventButton * event)
{


	g_return_val_if_fail(widget != NULL, FALSE);
	g_return_val_if_fail(IS_GM_CLIST(widget), FALSE);
	if ((*GTK_WIDGET_CLASS(parent_class)->button_press_event) (widget, event))
		return TRUE;
	if (event) {


		switch (event->type) {
		case GDK_2BUTTON_PRESS:
			{
				clist_activated(GTK_CLIST(widget));
				return TRUE;
				break;
			}
		case GDK_BUTTON_PRESS:
			{
				gint row, column;
				gint x, y;
				gchar *text;

				if (event->button != 3)
					break;

				if (event->state & GDK_CONTROL_MASK) {
					x = event->x;
					y = event->y;
					if (!gtk_clist_get_selection_info(GTK_CLIST(widget), x, y, &row, &column))
						return FALSE;
					gtk_clist_get_text(GTK_CLIST(widget), row, column, &text);

					if (text) {
					/*

						TODO : clipboard management
						
						if (global_app->char_clipboard)
							g_free(global_app->char_clipboard);
						global_app->char_clipboard = g_strdup(text);
					*/
					}

				}

				break;
			}

		default:
			{
				break;
			}
		}
	}
	return TRUE;
}


static void gm_clist_size_allocate(GtkWidget * widget, GtkAllocation * allocation)
{

	if (GTK_WIDGET_CLASS(parent_class)->size_allocate)
		(*GTK_WIDGET_CLASS(parent_class)->size_allocate) (widget, allocation);
}


static void gm_clist_size_request(GtkWidget * widget, GtkRequisition * requisition)
{

	if (GTK_WIDGET_CLASS(parent_class)->size_request)
		(*GTK_WIDGET_CLASS(parent_class)->size_request) (widget, requisition);

}

static GmSearchData *gm_clist_new_search_data(void)
{
	GmSearchData *sdata;
	sdata = g_new(GmSearchData, 1);
	sdata->search_string = NULL;
	sdata->wrap_around = FALSE;
	sdata->case_sensitive = FALSE;
	sdata->hit_row = -1;
	sdata->hit_col = -1;
	return sdata;
}


static void gm_clist_free_search_data(GmSearchData * sdata)
{
	if (sdata->search_string)
		g_string_free(sdata->search_string, TRUE);
	g_free(sdata);
}



static void gm_clist_destroy(GtkObject * object)
{
	GmClist *mlist;
	mlist = GM_CLIST(object);

	if (mlist->sdata) {
		gm_clist_free_search_data((GmSearchData *) mlist->sdata);

	}
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}



static void draw_row(GtkCList * clist, GdkRectangle * area, gint row, GtkCListRow * clist_row)
{
	GtkWidget *widget;
	GdkRectangle *rect;
	GdkRectangle row_rectangle;
	GdkRectangle cell_rectangle;
	GdkRectangle clip_rectangle;
	GdkRectangle intersect_rectangle;
	gint last_column;
	gint state;
	gint i;






	g_return_if_fail(clist != NULL);
	/* bail now if we arn't drawable yet */
	if (!GTK_WIDGET_DRAWABLE(clist) || row < 0 || row >= clist->rows)
		return;

	widget = GTK_WIDGET(clist);


	/* if the function is passed the pointer to the row instead of null,
	 * it avoids this expensive lookup */
	if (!clist_row)
		clist_row = ROW_ELEMENT(clist, row)->data;

	/* rectangle of the entire row */
	row_rectangle.x = 0;
	row_rectangle.y = ROW_TOP_YPIXEL(clist, row);
	row_rectangle.width = clist->clist_window_width;
	row_rectangle.height = clist->row_height;

	/* rectangle of the cell spacing above the row */
	cell_rectangle.x = 0;
	cell_rectangle.y = row_rectangle.y - CELL_SPACING;
	cell_rectangle.width = row_rectangle.width;
	cell_rectangle.height = CELL_SPACING;

	/* rectangle used to clip drawing operations, its y and height
	 * positions only need to be set once, so we set them once here. 
	 * the x and width are set withing the drawing loop below once per
	 * column */
	clip_rectangle.y = row_rectangle.y;
	clip_rectangle.height = row_rectangle.height;

	if (clist_row->state == GTK_STATE_NORMAL) {
		if (clist_row->fg_set)
			gdk_gc_set_foreground(clist->fg_gc, &clist_row->foreground);
		if (clist_row->bg_set)
			gdk_gc_set_foreground(clist->bg_gc, &clist_row->background);
	}






	state = clist_row->state;

	/* draw the cell borders and background */
	if (area) {
		rect = &intersect_rectangle;
		if (gdk_rectangle_intersect(area, &cell_rectangle, &intersect_rectangle)) {
			gdk_draw_rectangle(clist->clist_window, widget->style->base_gc[GTK_STATE_ACTIVE], FALSE, intersect_rectangle.x, intersect_rectangle.y, intersect_rectangle.width, intersect_rectangle.height);
		}
		/* the last row has to clear its bottom cell spacing too */
		if (clist_row == clist->row_list_end->data) {
			cell_rectangle.y += clist->row_height + CELL_SPACING;

			if (gdk_rectangle_intersect(area, &cell_rectangle, &intersect_rectangle))
				gdk_draw_rectangle(clist->clist_window, widget->style->base_gc[GTK_STATE_ACTIVE], TRUE, intersect_rectangle.x, intersect_rectangle.y, intersect_rectangle.width, intersect_rectangle.height);
		}

		if (!gdk_rectangle_intersect(area, &row_rectangle, &intersect_rectangle))
			return;

	} else {
		rect = &clip_rectangle;
		gdk_draw_rectangle(clist->clist_window, widget->style->base_gc[GTK_STATE_ACTIVE], TRUE, cell_rectangle.x, cell_rectangle.y, cell_rectangle.width, cell_rectangle.height);


		/* the last row has to clear its bottom cell spacing too */
		if (clist_row == clist->row_list_end->data) {
			cell_rectangle.y += clist->row_height + CELL_SPACING;

			gdk_draw_rectangle(clist->clist_window, widget->style->base_gc[GTK_STATE_ACTIVE], TRUE, cell_rectangle.x, cell_rectangle.y, cell_rectangle.width, cell_rectangle.height);
		}
	}

	for (last_column = clist->columns - 1; last_column >= 0 && !clist->column[last_column].visible; last_column--);

	/* iterate and draw all the columns (row cells) and draw their contents */
	for (i = 0; i < clist->columns; i++) {
		GtkStyle *style;
		GdkGC *fg_gc;
		GdkGC *bg_gc;

		gint width;
		gint height;
		gint pixmap_width;
		gint offset = 0;
		gint row_center_offset;

		if (!clist->column[i].visible)
			continue;

		get_cell_style(clist, clist_row, state, i, &style, &fg_gc, &bg_gc);

		clip_rectangle.x = clist->column[i].area.x + clist->hoffset;
		clip_rectangle.width = clist->column[i].area.width;

		/* calculate clipping region clipping region */
		clip_rectangle.x -= COLUMN_INSET + CELL_SPACING;
		clip_rectangle.width += (2 * COLUMN_INSET + CELL_SPACING + (i == last_column) * CELL_SPACING);

		if (area && !gdk_rectangle_intersect(area, &clip_rectangle, &intersect_rectangle))
			continue;


		gdk_draw_rectangle(clist->clist_window, bg_gc, TRUE, rect->x, rect->y, rect->width, rect->height);




		clip_rectangle.x += COLUMN_INSET + CELL_SPACING;
		clip_rectangle.width -= (2 * COLUMN_INSET + CELL_SPACING + (i == last_column) * CELL_SPACING);

		/* calculate real width for column justification */
		pixmap_width = 0;
		offset = 0;
		switch (clist_row->cell[i].type) {
		case GTK_CELL_TEXT:
			width = gdk_string_width(style->font, GTK_CELL_TEXT(clist_row->cell[i])->text);
			break;
		case GTK_CELL_PIXMAP:
			gdk_window_get_size(GTK_CELL_PIXMAP(clist_row->cell[i])->pixmap, &pixmap_width, &height);
			width = pixmap_width;
			break;
		case GTK_CELL_PIXTEXT:
			gdk_window_get_size(GTK_CELL_PIXTEXT(clist_row->cell[i])->pixmap, &pixmap_width, &height);
			width = (pixmap_width + GTK_CELL_PIXTEXT(clist_row->cell[i])->spacing + gdk_string_width(style->font, GTK_CELL_PIXTEXT(clist_row->cell[i])->text));
			break;
		default:
			continue;
			break;
		}

		switch (clist->column[i].justification) {
		case GTK_JUSTIFY_LEFT:
			offset = clip_rectangle.x + clist_row->cell[i].horizontal;
			break;
		case GTK_JUSTIFY_RIGHT:
			offset = (clip_rectangle.x + clist_row->cell[i].horizontal + clip_rectangle.width - width);
			break;
		case GTK_JUSTIFY_CENTER:
		case GTK_JUSTIFY_FILL:
			offset = (clip_rectangle.x + clist_row->cell[i].horizontal + (clip_rectangle.width / 2) - (width / 2));
			break;
		};

		/* Draw Text and/or Pixmap */
		switch (clist_row->cell[i].type) {
		case GTK_CELL_PIXMAP:
			draw_cell_pixmap(clist->clist_window, &clip_rectangle, fg_gc, GTK_CELL_PIXMAP(clist_row->cell[i])->pixmap, GTK_CELL_PIXMAP(clist_row->cell[i])->mask, offset, clip_rectangle.y + clist_row->cell[i].vertical + (clip_rectangle.height - height) / 2, pixmap_width, height);
			break;
		case GTK_CELL_PIXTEXT:
			offset = draw_cell_pixmap(clist->clist_window, &clip_rectangle, fg_gc, GTK_CELL_PIXTEXT(clist_row->cell[i])->pixmap, GTK_CELL_PIXTEXT(clist_row->cell[i])->mask, offset, clip_rectangle.y + clist_row->cell[i].vertical + (clip_rectangle.height - height) / 2, pixmap_width, height);
			offset += GTK_CELL_PIXTEXT(clist_row->cell[i])->spacing;
		case GTK_CELL_TEXT:
			if (style != GTK_WIDGET(clist)->style)
				row_center_offset = (((clist->row_height - style->font->ascent - style->font->descent - 1) / 2) + 1.5 + style->font->ascent);
			else
				row_center_offset = clist->row_center_offset;

			gdk_gc_set_clip_rectangle(fg_gc, &clip_rectangle);
			if (state != GTK_STATE_SELECTED)
				gdk_draw_string(clist->clist_window, style->font, fg_gc, offset, row_rectangle.y + row_center_offset + clist_row->cell[i].vertical, (clist_row->cell[i].type == GTK_CELL_PIXTEXT) ? GTK_CELL_PIXTEXT(clist_row->cell[i])->text : GTK_CELL_TEXT(clist_row->cell[i])->text);
			else
				gdk_draw_string(clist->clist_window, style->font, style->white_gc, offset, row_rectangle.y + row_center_offset + clist_row->cell[i].vertical, (clist_row->cell[i].type == GTK_CELL_PIXTEXT) ? GTK_CELL_PIXTEXT(clist_row->cell[i])->text : GTK_CELL_TEXT(clist_row->cell[i])->text);
			gdk_gc_set_clip_rectangle(fg_gc, NULL);
			break;
		default:
			break;
		}
	}

	/* draw focus rectangle */
	if (clist->focus_row == row && GTK_WIDGET_CAN_FOCUS(widget) && GTK_WIDGET_HAS_FOCUS(widget)) {
		if (!area)
			gdk_draw_rectangle(clist->clist_window, clist->xor_gc, FALSE, row_rectangle.x, row_rectangle.y, row_rectangle.width - 1, row_rectangle.height - 1);
		else if (gdk_rectangle_intersect(area, &row_rectangle, &intersect_rectangle)) {
			gdk_gc_set_clip_rectangle(clist->xor_gc, &intersect_rectangle);
			gdk_draw_rectangle(clist->clist_window, clist->xor_gc, FALSE, row_rectangle.x, row_rectangle.y, row_rectangle.width - 1, row_rectangle.height - 1);
			gdk_gc_set_clip_rectangle(clist->xor_gc, NULL);
		}
	}
}







static void get_cell_style(GtkCList * clist, GtkCListRow * clist_row, gint state, gint column, GtkStyle ** style, GdkGC ** fg_gc, GdkGC ** bg_gc)
{
	gint fg_state;

	if ((state == GTK_STATE_NORMAL) && (GTK_WIDGET(clist)->state == GTK_STATE_INSENSITIVE))
		fg_state = GTK_STATE_INSENSITIVE;
	else
		fg_state = state;

	if (clist_row->cell[column].style) {
		if (style)
			*style = clist_row->cell[column].style;
		if (fg_gc)
			*fg_gc = clist_row->cell[column].style->fg_gc[fg_state];
		if (bg_gc) {
			if (state == GTK_STATE_SELECTED)
				*bg_gc = clist_row->cell[column].style->bg_gc[state];
			else
				*bg_gc = clist_row->cell[column].style->base_gc[state];
		}
	} else if (clist_row->style) {
		if (style)
			*style = clist_row->style;
		if (fg_gc)
			*fg_gc = clist_row->style->fg_gc[fg_state];
		if (bg_gc) {
			if (state == GTK_STATE_SELECTED)
				*bg_gc = clist_row->style->bg_gc[state];
			else
				*bg_gc = clist_row->style->base_gc[state];
		}
	} else {
		if (style)
			*style = GTK_WIDGET(clist)->style;
		if (fg_gc)
			*fg_gc = GTK_WIDGET(clist)->style->fg_gc[fg_state];
		if (bg_gc) {
			if (state == GTK_STATE_SELECTED)
				*bg_gc = GTK_WIDGET(clist)->style->bg_gc[state];
			else
				*bg_gc = GTK_WIDGET(clist)->style->base_gc[state];
		}

		if (state != GTK_STATE_SELECTED) {
			if (fg_gc && clist_row->fg_set)
				*fg_gc = clist->fg_gc;
			if (bg_gc && clist_row->bg_set)
				*bg_gc = clist->bg_gc;
		}
	}
}


static gint draw_cell_pixmap(GdkWindow * window, GdkRectangle * clip_rectangle, GdkGC * fg_gc, GdkPixmap * pixmap, GdkBitmap * mask, gint x, gint y, gint width, gint height)
{
	gint xsrc = 0;
	gint ysrc = 0;


	if (mask) {
		gdk_gc_set_clip_mask(fg_gc, mask);
		gdk_gc_set_clip_origin(fg_gc, x, y);
	}

	if (x < clip_rectangle->x) {
		xsrc = clip_rectangle->x - x;
		width -= xsrc;
		x = clip_rectangle->x;
	}
	if (x + width > clip_rectangle->x + clip_rectangle->width)
		width = clip_rectangle->x + clip_rectangle->width - x;

	if (y < clip_rectangle->y) {
		ysrc = clip_rectangle->y - y;
		height -= ysrc;
		y = clip_rectangle->y;
	}
	if (y + height > clip_rectangle->y + clip_rectangle->height)
		height = clip_rectangle->y + clip_rectangle->height - y;

	gdk_draw_pixmap(window, fg_gc, pixmap, xsrc, ysrc, x, y, width, height);
	gdk_gc_set_clip_origin(fg_gc, 0, 0);
	if (mask)
		gdk_gc_set_clip_mask(fg_gc, NULL);

	return x + MAX(width, 0);
}


static void gm_clist_realize(GtkWidget * widget)
{

	GdkPixmap *arrow_down_pixmap;
	GdkPixmap *arrow_up_pixmap;
	GdkBitmap *arrow_t_mask;
	GmClist *clist;

	if (GTK_WIDGET_CLASS(parent_class)->realize)
		(*GTK_WIDGET_CLASS(parent_class)->realize) (widget);


	clist = GM_CLIST(widget);
	arrow_down_pixmap = gdk_pixmap_create_from_xpm_d(widget->window, &arrow_t_mask, &widget->style->bg[GTK_STATE_NORMAL], (char **)
																									 arrow_down_vector);
	if (arrow_down_pixmap)
		clist->down_arrow_widget = gtk_pixmap_new(arrow_down_pixmap, arrow_t_mask);
	else
		clist->down_arrow_widget = NULL;

	arrow_up_pixmap = gdk_pixmap_create_from_xpm_d((GTK_WIDGET(clist))->window, &arrow_t_mask, &widget->style->bg[GTK_STATE_NORMAL], (char **) arrow_up_vector);

	if (arrow_up_pixmap)
		clist->up_arrow_widget = gtk_pixmap_new(arrow_up_pixmap, arrow_t_mask);
	else
		clist->up_arrow_widget = NULL;


}

static GtkWidget *clist_create_arrow_widget(GtkWidget * widget, gboolean down)
{
	GdkPixmap *pixmap;
	GdkBitmap *mask;

	if (down)
		pixmap = gdk_pixmap_create_from_xpm_d(widget->window, &mask, &widget->style->bg[GTK_STATE_NORMAL], (char **) arrow_down_vector);
	else
		pixmap = gdk_pixmap_create_from_xpm_d(widget->window, &mask, &widget->style->bg[GTK_STATE_NORMAL], (char **) arrow_up_vector);


	return gtk_pixmap_new(pixmap, mask);
}


void gm_clist_sort_by_column(GtkCList * clist, gint column, gint direction)
{

	GmClist *mlist;

	g_return_if_fail(clist != NULL);
	mlist = GM_CLIST(clist);
	if (column <= 0)
		column = 0;
	if (column > clist->columns)
		column = 0;

	if (GTK_WIDGET_REALIZED(GTK_WIDGET(clist))) {
		/* FIXME:
		 * resorting the list cause the focused row to be lost */
		mlist->cinfo[column].current_sort_direction = direction;
		gm_clist_click_column(clist, column);
	}

}

static void gm_clist_click_column(GtkCList * clist, gint column)
{

	GmClist *mlist;
	GtkWidget *hbox;
	GtkWidget *arrow;
	GtkWidget *down_arrow_widget;
	GtkWidget *up_arrow_widget;

	mlist = GM_CLIST(clist);

	hbox = gtk_clist_get_column_widget(clist, column);

	down_arrow_widget = mlist->cinfo[column].down_arrow_widget;
	if (!down_arrow_widget) {
		down_arrow_widget = clist_create_arrow_widget(GTK_WIDGET(clist), TRUE);
		gtk_box_pack_start(GTK_BOX(hbox), down_arrow_widget, FALSE, FALSE, 2);
		mlist->cinfo[column].down_arrow_widget = down_arrow_widget;
	}

	up_arrow_widget = mlist->cinfo[column].up_arrow_widget;
	if (!up_arrow_widget) {
		up_arrow_widget = clist_create_arrow_widget(GTK_WIDGET(clist), FALSE);
		gtk_box_pack_start(GTK_BOX(hbox), up_arrow_widget, FALSE, FALSE, 2);
		mlist->cinfo[column].up_arrow_widget = up_arrow_widget;
	}
	gtk_widget_hide(down_arrow_widget);
	gtk_widget_hide(up_arrow_widget);

	if (mlist->sort_column != column) {
		/* hide the last column arrows */
		if (mlist->sort_column >= 0) {
			if (mlist->cinfo[mlist->sort_column].up_arrow_widget)
				gtk_widget_hide(mlist->cinfo[mlist->sort_column].up_arrow_widget);
			if (mlist->cinfo[mlist->sort_column].down_arrow_widget)
				gtk_widget_hide(mlist->cinfo[mlist->sort_column].down_arrow_widget);
		}

		if (!mlist->cinfo[column].current_sort_direction) {
			mlist->cinfo[column].current_sort_direction = SORT_UP;
		}
		if (mlist->cinfo[column].current_sort_direction == SORT_UP) {
			arrow = up_arrow_widget;
			clist->sort_type = 0;
		} else {
			arrow = down_arrow_widget;
			clist->sort_type = 1;
		}
	} else {
		if (mlist->cinfo[column].current_sort_direction == SORT_UP) {
			mlist->cinfo[column].current_sort_direction = SORT_DOWN;
			arrow = down_arrow_widget;
			clist->sort_type = 1;
		} else {
			mlist->cinfo[column].current_sort_direction = SORT_UP;
			arrow = up_arrow_widget;
			clist->sort_type = 0;
		}
	}
	if (arrow->parent != hbox) {
		gtk_widget_unparent(arrow);
		gtk_box_pack_start(GTK_BOX(hbox), arrow, FALSE, FALSE, 2);
	}
	gtk_widget_show(arrow);
	mlist->sort_column = column;
	gtk_signal_emit(GTK_OBJECT(clist), mlist_signals[RESORTED], column, (clist->sort_type) ? SORT_DOWN : SORT_UP);
	gtk_clist_set_sort_column(clist, column);
	gtk_clist_sort(clist);
	gm_clist_show_selected_row(clist);
}


void gm_clist_set_column_widget(GmClist * mlist, gint column, GtkWidget * widget)
{

	GtkWidget *hbox;

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_object_set_data(GTK_OBJECT(hbox), "label", widget);
	gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, 0);
	gtk_clist_set_column_widget(GTK_CLIST(mlist), column, hbox);
	gtk_signal_connect(GTK_OBJECT(GTK_CLIST(mlist)->column[column].button), "button_press_event", GTK_SIGNAL_FUNC(clist_button_press), GTK_CLIST(mlist));
}


void gm_clist_set_column_sortdata_type(GmClist * mlist, gint column, gint type)
{

	g_return_if_fail(mlist != NULL);
	g_return_if_fail(column >= 0 && column <= GTK_CLIST(mlist)->columns);
	mlist->cinfo[column].sort_data_type = type;
}

static gint compare_rows(GtkCList * clist, gconstpointer ptr1, gconstpointer ptr2)
{
	char *text1 = NULL;
	char *text2 = NULL;
	char *lc_text1 = NULL;
	char *lc_text2 = NULL;
	int c = 0, sort_column;
	long long1 = 0;
	long long2 = 0;
	double double1 = (double)0;
	double double2 = (double)0;
	GmClist *mlist;
	GtkCListRow *row1, *row2;

	mlist = GM_CLIST(clist);
	sort_column = mlist->sort_column;


	row1 = (GtkCListRow *) ptr1;
	row2 = (GtkCListRow *) ptr2;


	if (row1->data) {
		return 0;
	}

	/*Fixme: some thing is wrong her  */

	text1 = GTK_CELL_TEXT(row1->cell[sort_column])->text;
	text2 = GTK_CELL_TEXT(row2->cell[sort_column])->text;

	if (!text2)
		return (text1 != NULL);

	if (!text1)
		return -1;
	if (strlen(text1) < 1) {
		if (strlen(text2) > 0)
			return -1;
		return 0;
	}
	if (strlen(text2) < 1)
		return 1;

	switch (mlist->cinfo[sort_column].sort_data_type) {

		case GMCLIST_TYPE_NONE:
		return 0;

		case GMCLIST_TYPE_DATETIME:
		case GMCLIST_TYPE_STRING:
			lc_text1 = g_strdup(text1);
			lc_text2 = g_strdup(text2);
			g_strdown(lc_text1);
			g_strdown(lc_text2);
			c = strcmp(lc_text1, lc_text2);
			g_free(lc_text1);
			g_free(lc_text2);
			return c;
		break;

		case GMCLIST_TYPE_INTEGER:
			long1 = atol(text1);
			long2 = atol(text2);
			if (long1 > long2) return 1;
			if (long1 < long2) return -1;
			return 0;
		case GMCLIST_TYPE_FLOAT:
			double1 = atof(text1);
			double2 = atof(text2);
			if (double1 > double2) return 1;
			if (double1 < double2) return -1;
			return 0;
		break;

	}

	return 0;
}



void gm_clist_show_selected_row(GtkCList * clist)
{

	GList *list;
	gint sel_row;
	list = clist->selection;
	if (!clist->selection)
		return;
	sel_row = GPOINTER_TO_INT(list->data);
	if (gtk_clist_row_is_visible(clist, sel_row) != GTK_VISIBILITY_FULL)
		gtk_clist_moveto(clist, sel_row, 0, 0.0, 0.0);
}



void gm_clist_select_row(GtkCList * clist, gint row_pos, gint column)
{

	gint visible;

	clist->focus_row = row_pos;
	gtk_clist_select_row(clist, row_pos, column);
	visible = gtk_clist_row_is_visible(clist, row_pos);

	if (visible != GTK_VISIBILITY_FULL)
		gtk_clist_moveto(clist, row_pos, column, 0.0, 0.0);

}



gint gm_clist_find_row_from_column(GtkCList * clist, gchar * data, gint column)
{

	GList *l;
	gint row = 0;

	g_return_val_if_fail(clist != NULL, -1);
	g_return_val_if_fail(data != NULL, -1);
	l = clist->row_list;

	while (l) {
		gchar *cdata = NULL;
		gtk_clist_get_text(clist, row, 0, &cdata);
		if (cdata && !strcmp(cdata, data))
			return row;
		l = l->next;
		row++;
	}
	return -1;
}

void gm_clist_set_hint_widget(GmClist * mlist, GtkWidget * hwidget)
{

	g_return_if_fail(hwidget != NULL);
	g_return_if_fail(mlist != NULL);

	mlist->hint_widget = hwidget;

}





static gboolean clist_real_search(GmClist * mlist)
{

	gint hit_row = 0, hit_col = 0;
	gint max_cols, max_rows;
	GtkCList *clist;
	static GList *list;
	GmSearchData *sdata;
	gboolean case_sensitive;
	gboolean ret_value;
	gchar *str;


	clist = GTK_CLIST(mlist);
	max_cols = clist->columns;
	max_rows = clist->rows;
	sdata = mlist->sdata;
	str = sdata->search_string->str;
	case_sensitive = sdata->case_sensitive;

	hit_row = (sdata->hit_row >= 0) ? sdata->hit_row : 0;
	hit_col = (sdata->hit_col >= 0) ? sdata->hit_col : 0;

	if (hit_col >= max_cols) {
		hit_col = 0;
		hit_row++;
		if (hit_row >= max_rows)
			hit_row = 0;
	}



	list = g_list_nth(clist->row_list, hit_row);
	ret_value = FALSE;
	while (list) {
		gchar *cell_text = NULL;
		gboolean found;
		while (1) {
			gtk_clist_get_text(clist, hit_row, hit_col, &cell_text);
			if (cell_text) {
				if (case_sensitive) {
					if (strstr(cell_text, str))
						found = TRUE;
					else
						found = FALSE;
				} else {
					found = gm_utils_istrstr(cell_text, str);
				}

				if (found) {
					gtk_clist_unselect_all(clist);
					gm_clist_select_row(clist, hit_row, hit_col);
					sdata->hit_row = hit_row;
					sdata->hit_col = hit_col;
					ret_value = TRUE;
					break;
				}
			}
			hit_col++;
			if (hit_col >= max_cols) {
				hit_col = 0;
				break;
			}
		}
		if (ret_value)
			break;
		hit_row++;
		if (hit_row >= clist->rows)
			hit_row = 0;
		list = list->next;
	}
	return ret_value;
}


gboolean gm_clist_search(GmClist * mlist, gchar * str, gboolean wrap_around, gboolean case_sensitive)
{

	GmSearchData *sdata;

	g_return_val_if_fail(str != NULL, FALSE);

	sdata = mlist->sdata;

	if (!mlist->sdata) {
		sdata = mlist->sdata = gm_clist_new_search_data();
		sdata->search_string = g_string_new(str);
	}
	if (strcmp(sdata->search_string->str, str)) {
		gm_clist_free_search_data(sdata);
		sdata = mlist->sdata = gm_clist_new_search_data();
		sdata->search_string = g_string_new(str);
	} else {
		sdata->hit_col++;
	}

	sdata->wrap_around = wrap_around;
	sdata->case_sensitive = case_sensitive;

	if (clist_real_search(mlist))
		return TRUE;

	if (sdata->wrap_around) {
		if (sdata->hit_row != -1 && sdata->hit_col != -1) {
			sdata->hit_row = -1;
			sdata->hit_col = -1;
			return clist_real_search(mlist);
		}
	}
	return FALSE;
}

static gint clist_button_press(GtkWidget * widget, GdkEventButton * event, gpointer data)
{


	gint i, column = 0;
	GtkCList *clist;
	GList *l;
	GString *string;
	gint row = 0;
	gboolean extended = FALSE;

	g_return_val_if_fail(widget != NULL, FALSE);
	g_return_val_if_fail(GTK_IS_CLIST(data), FALSE);




	clist = GTK_CLIST(data);

	if (event->button != 3)
		return FALSE;

	for (i = 0; i < clist->columns; i++)
		if (clist->column[i].button == widget)
			break;
	column = i;

	if (event->state & GDK_CONTROL_MASK)
		extended = TRUE;


	l = clist->row_list;
	string = g_string_new("");
	while (l) {
		if (extended) {
			for (i = 0; i < column + 1; i++) {
				gchar *cdata = NULL;
				gtk_clist_get_text(clist, row, i, &cdata);

				if (cdata)
					g_string_sprintfa(string, "%s\t", cdata);
				else
					g_string_append(string, "\t");
			}
			g_string_append(string, "\n");
		} else {
			gchar *cdata = NULL;
			gtk_clist_get_text(clist, row, column, &cdata);

			if (cdata)
				g_string_sprintfa(string, "%s \n", cdata);
			else
				g_string_append(string, "\n");
		}


		l = l->next;
		row++;
	}

	/*

	TODO : clipboard management

	if (global_app->char_clipboard)
		g_free(global_app->char_clipboard);
	global_app->char_clipboard = string->str;
	*/


	g_string_free(string, FALSE);
	return TRUE;
}


gchar *gm_clist_get_column_title(GtkCList * clist, gint i)
{
	GtkLabel *label;
	GtkWidget *hbox;
	g_return_val_if_fail(clist != NULL, NULL);

	hbox = gtk_clist_get_column_widget(clist, i);
	label = GTK_LABEL(gm_utils_get_data(hbox, "label"));
	if (!label)
		return NULL;

	return label->label;

}


void gm_clist_invert_selection(GtkCList * clist)
{
	GtkCListRow *clist_row;
	GList *selection, *list;
	gint row = 0;

	selection = clist->selection;
	if (!selection) {
		gtk_clist_select_all(clist);
		return;
	}

	for (row = 0; row < clist->rows; row++) {
		list = ROW_ELEMENT(clist, row);
		g_assert(list != NULL);
		clist_row = list->data;

		if (clist_row->state == GTK_STATE_SELECTED)
			gtk_clist_unselect_row(clist, row, 0);
		else
			gtk_clist_select_row(clist, row, 0);
	}
}

gpointer gm_utils_get_data(gpointer obj, gchar * key)
{
	gpointer result;
	if (!obj) {
		g_warning ("gm_utils_get_data() : Object is a Null Pointer for the given key [%s]", key);
		return NULL;
	}
	if (!key) {
		g_warning ("gm_utils_get_data() : No key provided");
		return NULL;
	}

	result = gtk_object_get_data(GTK_OBJECT(obj), key);
	if (!result) {
		g_warning ("Could not find Pointer for the given key [%s]", key);
	}

	return result;
}

gboolean gm_utils_istrstr(gchar * src, gchar * str)
{
	gchar *src_cp;
	gboolean ret = FALSE;

	g_return_val_if_fail(src != NULL, FALSE);
	g_return_val_if_fail(str != NULL, FALSE);

	src_cp = g_strdup(src);
	g_strdown(src_cp);
	if (strstr(src_cp, str)) {
		ret = TRUE;
	}
	g_free(src_cp);

	return ret;
}
