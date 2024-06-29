#!/bin/sh

if zenity --forms --title="Stampa etichetta collo" --text="Inserire dati etichetta" \
   --add-entry="Inserire codice ordine" \
   --add-entry="Inserire sotto ordine" \
   --add-entry="Inserire numero collo" \
   --add-entry="Inserire nome stampante" > print_label.dat
  then \
  	echo 'processing';
	data=`cat print_label.dat`;
	ordine=`echo $data | cut -d "|" -f 1`;
	sotto_ordine=`echo $data | cut -d "|" -f 2`;
	collo=`echo $data | cut -d "|" -f 3`;
	stampante=`echo $data | cut -d "|" -f 4`;
	echo cd ../exe; echo ./printlabel -a -o $ordine.$sotto_ordine.$collo -p $stampante;

  else echo "cancel"
fi


