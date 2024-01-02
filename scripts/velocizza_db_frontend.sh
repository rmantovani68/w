#!/bin/bash


gdialog --title "Conferma Velocizzazione DataBase" --yesno 'Confermi velocizzazione DataBase ? \n
Tale operazione comporta un tempo di alcuni minuti in cui il sistema sara` inutilizzabile.' 300 300 

case $? in

0)
	/u/prj/wella/scripts/velocizza_db_backend.sh
	gdialog --title "Velocizzazione DataBase Terminata" --msgbox "Velocizzazione DataBase Terminata" 350 350
	;;
1)
		:
	;;
esac
