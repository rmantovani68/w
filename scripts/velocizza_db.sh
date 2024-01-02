# prima fase
echo 'BACKUP TABLES'
echo ric_ord   ;pg_dump sap -t ric_ord    | gzip -c > ric_ord.dump.gz
echo ric_art   ;pg_dump sap -t ric_art    | gzip -c > ric_art.dump.gz
echo ord_prod  ;pg_dump sap -t ord_prod   | gzip -c > ord_prod.dump.gz
echo col_prod  ;pg_dump sap -t col_prod   | gzip -c > col_prod.dump.gz
echo rig_prod  ;pg_dump sap -t rig_prod   | gzip -c > rig_prod.dump.gz
echo articoli  ;pg_dump sap -t articoli   | gzip -c > articoli.dump.gz
echo ubicazioni;pg_dump sap -t ubicazioni | gzip -c > ubicazioni.dump.gz

# seconda fase
echo 'DROP TABLES'
echo ric_ord   ;echo 'drop table ric_ord;' | psql sap
echo ric_art   ;echo 'drop table ric_art;' | psql sap
echo ord_prod  ;echo 'drop table ord_prod;' | psql sap
echo col_prod  ;echo 'drop table col_prod;' | psql sap
echo rig_prod  ;echo 'drop table rig_prod;' | psql sap
echo articoli  ;echo 'drop table articoli;' | psql sap
echo ubicazioni;echo 'drop table ubicazioni;' | psql sap

# terza fase
echo 'RESTORE TABLES'
echo ric_ord   ;gunzip -c ric_ord.dump.gz      | psql sap
echo ric_art   ;gunzip -c ric_art.dump.gz      | psql sap
echo ord_prod  ;gunzip -c ord_prod.dump.gz     | psql sap
echo col_prod  ;gunzip -c col_prod.dump.gz     | psql sap
echo rig_prod  ;gunzip -c rig_prod.dump.gz     | psql sap
echo articoli  ;gunzip -c articoli.dump.gz     | psql sap
echo ubicazioni;gunzip -c ubicazioni.dump.gz   | psql sap
