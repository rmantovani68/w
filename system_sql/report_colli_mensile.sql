-- Query da usare con lo script di crontab /usr/local/bin/report_colli_mensile.sh
-- Si usa il mese precedente al corrente.
select lpad(ordprog,10,'0'),lpad(cpnmcol::text,4,'0'),lpad(cpprgcl::text,7,'0'),lpad(cppsrea::text,9,'0') from col_stor where substring(cpdteva from 1 for 6)=(to_char(CURRENT_TIMESTAMP - interval '1 month','YYYYMM'))
union
select lpad(ordprog,10,'0'),lpad(cpnmcol::text,4,'0'),lpad(cpprgcl::text,7,'0'),lpad(cppsrea::text,9,'0') from col_prod where substring(cpdteva from 1 for 6)=(to_char(CURRENT_TIMESTAMP - interval '1 month','YYYYMM'));

-- Query da usare nel caso si siano persi dei mesi, causa vacanze e pc spento
-- Cambiare la data e lanciare  il comando:
-- psql -A -t -F '' sap < /u/prj/wella/system_sql/report_colli_mensile.sql | unix2dos > /u/prj/wella/export_sap/report_colli_mensile_YYYY-MM-01.txt
--select lpad(ordprog,10,'0'),lpad(cpnmcol::text,4,'0'),lpad(cpprgcl::text,7,'0'),lpad(cppsrea::text,9,'0') from col_stor where substring(cpdteva from 1 for 6)='201701'
--union
--select lpad(ordprog,10,'0'),lpad(cpnmcol::text,4,'0'),lpad(cpprgcl::text,7,'0'),lpad(cppsrea::text,9,'0') from col_prod where substring(cpdteva from 1 for 6)='201701';
