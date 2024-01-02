/*
* labelprint.c
* ------------
* Filtro  di Stampa Etichette
* viene lanciato dal Daemon di stampa lpd
* su richiesta di lpr
* 
* Progetto Easy Picking 2.0 : Europharco
* Data creazione 29/03/1999
*
* Autore : Roberto Mantovani
*
* Storia delle modifiche
* S. DATA         DESCRIZIONE
* -- ----------   ----------------------------------------
* -- ----------   ----------------------------------------
*
* Copyright CNI srl 1999
*/
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	char string[128];
	int nSleepTime=1;

	if(argc==2){
		nSleepTime=atoi(argv[1]);
	}

	while(gets(string)){
		puts(string);
		if(!strncmp(string,"BRK",3)){
			sleep(nSleepTime);
		}
	}
	return 0;
}

