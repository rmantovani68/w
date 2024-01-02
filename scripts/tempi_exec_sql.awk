#!/bin/awk -f

#
# script per verificare le query sql con tempi di esecuzione
# maggiori di un tempo stabilito
#
BEGIN {
	exec=0
	tempo_exec=200
}

{ 
	ora=$2
	comando=$4
	comando_1=$5
	comando_2=$6
	secondi=substr(ora,8,2)
	millisecondi=substr(ora,11,3)

	if(exec==1 && (comando=="PGRES_COMMAND_OK" || comando=="PGRES_TUPLES_OK")){
		secondi_passati=secondi-secondi_start
		millisecondi_passati=millisecondi-millisecondi_start
		if(secondi_passati>=1){
			if(millisecondi>=millisecondi_start){
				millisecondi_passati=millisecondi-millisecondi_start
			} else {
				millisecondi_passati=(1000+millisecondi)-millisecondi_start
				secondi_passati=secondi_passati-1;
			}
		}
		if(secondi_passati*1000+millisecondi_passati>=tempo_exec){
			printf "tempo : %02d.%03d - ",secondi_passati,millisecondi_passati
			printf "comando   : %s\n",exec_sql 
		}
		exec=0
	}

	if(comando=="EXEC"){
		# printf "comando : %s\n",$0
		# printf "ora inizio : %s\n",ora 
		exec_sql=$0;
		secondi_start=substr(ora,8,2)
		millisecondi_start=substr(ora,11,3)
		exec=1
	}
}
