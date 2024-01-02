/* 
* Modulo : listinterface.c
* ------------------
* funzioni di gestione dialog box per la 
* visualizzazione di liste circolari.
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1996-98
*/

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <varargs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <Kn/Knvas.h>
#include <Xbae/Matrix.h>
#include <X11/Xmu/Editres.h>
#include <Xm/XmAll.h>

#include <pmx_msq.h>
#include <msq_lib.h>
#include <shared.h>
#ifdef TRACE
	#include <trace.h>
#endif
#include <cnxToolbox.h>

#define BOOL_TYPE_DEFINED
#define BYTE_TYPE_DEFINED

#include <ca_file.h>
#include <proc_list.h>

#include "sin.h"
#include "sinlink.h"
#include "sinstruct.h"
#include "sinext.h"
#include "sinfun.h"




void UpdateListDlg(LINKED_LIST *list)
{
	Widget DlgWidget;
	Widget mt_pack_list;
	Widget lb_list_pos;
	Widget lb_bolcol;
	char szBuffer[80];
	int nIndex;
	PCOLLO pCollo;

	DlgWidget=XtNameToWidget(toplevel,"*dlg_pack_list");
	lb_list_pos=XtNameToWidget(DlgWidget,"*lb_list_pos");
	lb_bolcol=XtNameToWidget(DlgWidget,"*lb_bolcol");
	mt_pack_list=XtNameToWidget(DlgWidget,"*mt_pack_list");

	if(!link_prev(list,list->selected)){
		XmLabelPrintf(lb_list_pos,"FIRST / %2d",list->n_link);
	} else if(!link_next(list,list->selected)){
		XmLabelPrintf(lb_list_pos,"LAST / %2d",list->n_link);
	} else {
		XmLabelPrintf(lb_list_pos," %2d",list->n_link);
	}
	for(nIndex=0;nIndex<MAX_ARTICOLI;nIndex++){
		XbaeMatrixSetCell(mt_pack_list,nIndex,0, "");
		XbaeMatrixSetCell(mt_pack_list,nIndex,1, "");
	}
	if(list->selected){
		pCollo=(PCOLLO)list->selected;
		XmLabelPrintf(lb_bolcol, "Bolla :%s\nCollo :%s\nFormato :%c\nDestinazione:\n%s\nRighe: %d",
			pCollo->szBolla,
			pCollo->szCollo,
			pCollo->cFormato,
			GetDestination(pCollo->cArea,szBuffer),
			pCollo->nArticoli);
		for(nIndex=0;nIndex<pCollo->nArticoli;nIndex++){
			XbaeMatrixSetCell(mt_pack_list,nIndex,0, pCollo->Articoli[nIndex].szProdCode);
			XbaeMatrixSetCell(mt_pack_list,nIndex,1, pCollo->Articoli[nIndex].szProdQty);
		}
	}
}

void EditNextCB(Widget w,XtPointer client,XtPointer call)
{
	LINKED_LIST *list=(LINKED_LIST *)client;	
	void *selection;

	if((selection=link_next(list,list->selected))!=NULL){
		list->selected=selection;
	}
	UpdateListDlg(list);
	return;
}

void EditPrevCB(Widget w,XtPointer client,XtPointer call)
{
	LINKED_LIST *list=(LINKED_LIST *)client;	
	void *selection;

	if(selection=link_prev(list,list->selected)){
		list->selected=selection;
	}
	UpdateListDlg(list);
	return;
}

void EditTopCB(Widget w,XtPointer client,XtPointer call)
{
	LINKED_LIST *list=(LINKED_LIST *)client;	

	list->selected=link_first(list);
	UpdateListDlg(list);
	return;
}

void EditBottomCB(Widget w,XtPointer client,XtPointer call)
{
	LINKED_LIST *list=(LINKED_LIST *)client;	

	list->selected=link_last(list);
	UpdateListDlg(list);
	return;
}

void EditCloseCB(Widget w,XtPointer client,XtPointer call)
{
	LINKED_LIST *list=(LINKED_LIST *)client;	
	Widget DlgWidget;
	char szDlgName[80];

	sprintf(szDlgName,"*dlg_pack_list");

	DlgWidget=XtNameToWidget(toplevel,szDlgName);
	XtUnmanageChild(XtNameToWidget(DlgWidget,"*fm_ec"));

	return;
}



/*
* InitListDlg()
* inizializza i dati per la gestione di un edit DataBase dialog
*
* Struttura dei widget utilizzati
*
	XmDialogShell  dlg_pack_list
		XmForm  fm_ec
			XmRowColumn  rc_ec
				XmPushButton  pb_top
				XmPushButton  pb_prev
				XmPushButton  pb_next
				XmPushButton  pb_bottom
				XmPushButton  pb_close
			XmSeparator  sp_ec
			XmForm  fm_pack_list
				XbaeMatrix  mt_pack_list
					XmScrollBar  horizScroll
					XmScrollBar  vertScroll
					XbaeClip  clip
					XqCell  textField
			XmLabel  lb_list_pos
*
*/
Widget InitListDlg(LINKED_LIST *list)
{
	int nIndex;
	Widget DlgWidget;
	Widget fm_ec;
	Widget rc_ec;
	Widget fm_pack_list;
	Widget mt_pack_list;
	char szDlgName[80];
	short nFieldLenghts[10];
	char *szRowLabels[256];
	char ***szFieldValues;
	char szBuffer[80];

	sprintf(szDlgName,"dlg_pack_list");

	/*
	* dialog shell
	*/
	DlgWidget=XtVaCreateWidget(szDlgName,xmDialogShellWidgetClass, toplevel,NULL);
	/*
	* form principale
	*/
	fm_ec=XtVaCreateWidget("fm_ec",xmFormWidgetClass, DlgWidget,NULL);
	/*
	* Button Bar
	*/
	rc_ec=XtVaCreateManagedWidget("rc_ec",xmRowColumnWidgetClass, fm_ec,NULL);
	XtVaCreateManagedWidget("pb_top",   xmPushButtonWidgetClass, rc_ec,NULL);
	XtVaCreateManagedWidget("pb_prev",  xmPushButtonWidgetClass, rc_ec,NULL);
	XtVaCreateManagedWidget("pb_next",  xmPushButtonWidgetClass, rc_ec,NULL);
	XtVaCreateManagedWidget("pb_bottom",xmPushButtonWidgetClass, rc_ec,NULL);
	XtVaCreateManagedWidget("pb_close", xmPushButtonWidgetClass, rc_ec,NULL);
	/*
	* separatore fra button bar e form fields
	*/
	XtVaCreateManagedWidget("sp_ec", xmSeparatorWidgetClass, fm_ec,NULL);
	/*
	* Form di contenimento pack list
	*/
	fm_pack_list=XtVaCreateManagedWidget("fm_pack_list", xmFormWidgetClass, fm_ec,NULL);

	/*
	* tabella packing list collo/i selezionato/i
	*/
	nIndex=0;
	nFieldLenghts[nIndex++]=12;	/* Codice Articolo*/
	nFieldLenghts[nIndex++]=8;	/* Quantita' */
	nFieldLenghts[nIndex++]=0;	

	for(nIndex=0;nIndex<MAX_ARTICOLI;nIndex++){
		sprintf(szBuffer,"%2d",nIndex);
		szRowLabels[nIndex]=strdup(szBuffer);
	}
	szFieldValues=xncAllocMatrix(MAX_ARTICOLI,2,nFieldLenghts);
	mt_pack_list=XtVaCreateManagedWidget("mt_pack_list",xbaeMatrixWidgetClass,fm_pack_list,
		XmNcolumns,2,
		XmNrows,MAX_ARTICOLI,
		XmNrowLabels,szRowLabels,
		XmNcolumnWidths,nFieldLenghts,
		XmNcells,szFieldValues,NULL);

	XtAddCallback(mt_pack_list,XmNmodifyVerifyCallback,DoNotEditCB,(XtPointer)NULL);

	/*
	* libero la memoria utilizzata
	* per la inizializzazione della matrice
	*/
	free(szFieldValues);
	for(nIndex=0;nIndex<Aree.nAree;nIndex++){
		free(szRowLabels[nIndex]);
	}
	/*
	* label di indicazione della bolla collo
	*/
	XtVaCreateManagedWidget("lb_bolcol", xmLabelWidgetClass, fm_pack_list,NULL);


	/*
	* Label di indicazione num.record/tot.records
	*/
	XtVaCreateManagedWidget("lb_list_pos", xmLabelWidgetClass, fm_ec,NULL);

	/*
	* inizializzo i bottoni con i pixmap
	*/
	AssignPixmapToWidget(XtNameToWidget(rc_ec,"*pb_next"),"next.xpm");
	AssignPixmapToWidget(XtNameToWidget(rc_ec,"*pb_prev"),"prev.xpm");
	AssignPixmapToWidget(XtNameToWidget(rc_ec,"*pb_top"),"top.xpm");
	AssignPixmapToWidget(XtNameToWidget(rc_ec,"*pb_bottom"),"bottom.xpm");
	AssignPixmapToWidget(XtNameToWidget(rc_ec,"*pb_close"),"close.xpm");
	/*
	* assegno le callbacks generiche di edit campo
	*/
	XtAddCallback(XtNameToWidget(rc_ec,"*pb_next"),
			XmNactivateCallback,EditNextCB, (XtPointer)list);

	XtAddCallback(XtNameToWidget(rc_ec,"*pb_prev"),
			XmNactivateCallback,EditPrevCB, (XtPointer)list);

	XtAddCallback(XtNameToWidget(rc_ec,"*pb_top"),
			XmNactivateCallback,EditTopCB, (XtPointer)list);

	XtAddCallback(XtNameToWidget(rc_ec,"*pb_bottom"),
			XmNactivateCallback,EditBottomCB, (XtPointer)list);

	XtAddCallback(XtNameToWidget(rc_ec,"*pb_close"),
			XmNactivateCallback,EditCloseCB, (XtPointer)list);


	return DlgWidget;
}


