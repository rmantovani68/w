#!/bin/bash
#
# report_colli_mensile.sh
# Versione: 1.0
# Autore: Luca Guerrini
#
# Mettere i giusti permessi e fare link in /usr/local/bin per poterlo lanciare da riga di comando
#

DATE=`date +%Y-%m-01`
FILE_NAME=/u/prj/wella/export_sap/report_colli_mensile_$DATE.txt
echo $FILE_NAME

###Estrazione dei dati dei colli del mese precedente
psql -A -t -F '' sap < /u/prj/wella/system_sql/report_colli_mensile.sql | unix2dos > $FILE_NAME
### Nel caso serva piu' di un mese decommentare e andare a cambiare le date nel .sql
#psql -A -t -F '' sap < /u/prj/wella/system_sql/report_colli_mensile_from_date_to_date.sql | unix2dos > $FILE_NAME

#/usr/bin/smtp-cli --verbose --server=smtp.boxlinesrl.com --from=info@boxlinesrl.com  --to=luca.guerrini@poste.it --user=info@boxlinesrl.com --pass=bo
#/usr/bin/smtp-cli --verbose --server=smtp.boxlinesrl.com --from=info@boxlinesrl.com --to=enniogia@gmail.com --to=albertoperusi@boxlinesrl.com --to=lu
cp $FILE_NAME /u/prj/wella/export_sap/report_colli_mensile.txt

exit 0

