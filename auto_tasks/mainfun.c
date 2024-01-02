/*
* printfun.c : Stampa Etichette : funzioni di utilita'
* 
* Progetto Easy Picking 2.0 
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1999-2000
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termio.h>
#include <sys/stat.h>

#include <libpq-fe.h>
#include <pmx_msq.h>
#include <msq_lib.h>
#include <shared.h>
#ifdef TRACE
	#include <trace.h>
#endif
#include <ca_file.h>
#include <dbfun.h>
#include <proc_list.h>

#include	<picking.h>

#include	"main.h"
#include	"mainstruct.h"
#include	"mainext.h"
#include	"mainfun.h"



/*
* ReadConfiguration()
* lettura della configurazione dal file cni.cfg
*/
void ReadConfiguration(BOOL bReadProcInfo)
{
	char szParagraph[128];
	char szCurrentDirectory[128];

	/*
	* leggo il file di configurazione dalla directory corrente
	*/
	getcwd(szCurrentDirectory,sizeof(szCurrentDirectory));
	
	strcpy(Cfg.szCniCfg,szCurrentDirectory);
	strcat(Cfg.szCniCfg,"/cni.cfg");
	/*
	* lettura del file di configurazione
	*/
	strcpy(szParagraph,"General Settings");

	/* SAP */
	Cfg.nSAP = xncGetFileInt(szParagraph, "SAP", 0, Cfg.szCniCfg, NULL);

	Cfg.nDebugVersion=xncGetFileInt(szParagraph,"DebugVersion",0,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"Language",    "ita",        Cfg.szLanguage,     80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathData",    "../data",    Cfg.szPathData,     80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathExport",  "../export",  Cfg.szPathExport,   80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathExe",     "./",         Cfg.szPathExe,      80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathStampe",  "../stampe",  Cfg.szPathStampe,   80,Cfg.szCniCfg,NULL);

	/*
	* lettura del file di configurazione del DataBase (PostgreSQL)
	*/
	strcpy(szParagraph,"DataBase Settings");

	xncGetFileString(szParagraph,"PGHost",    "localhost",Cfg.szPGHost,     80,Cfg.szCniCfg,NULL); 
	xncGetFileString(szParagraph,"PGPort",    "5432",     Cfg.szPGPort,     80,Cfg.szCniCfg,NULL); 
	xncGetFileString(szParagraph,"PGDataBase","picking",  Cfg.szPGDataBase, 80,Cfg.szCniCfg,NULL); 

	if(bReadProcInfo){
		ReadProcInfo(Cfg.szCniCfg);
	}

}

/*
* ReadProcInfo()
* Lettura dei parametri relativi ai processi 
*/
void ReadProcInfo(char *szFileConfig)
{
	char szProcIndex[128];
	char szProcBuffer[128];
	int nIndex;

	for (nIndex=0; nIndex<NUM_PROC; nIndex++) {
		sprintf(szProcIndex,"Proc_%02d",nIndex);
		xncGetFileString("procinfo",szProcIndex, "", szProcBuffer, 80,szFileConfig,NULL);
		sscanf(szProcBuffer, "%[^,],%d,%d,%d",
				ProcList[nIndex].szProcName,
				&ProcList[nIndex].nQKey,
				&ProcList[nIndex].nPriority,
				&ProcList[nIndex].bExecute);

		ProcList[nIndex].nGId=nIndex+1;

		/*
		* resetto il numero coda messaggi del processo
		*/
		ProcList[nIndex].nQNumber=0;
	}
}

