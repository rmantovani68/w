--select to_char(lptmspc,'YYYYMM') as "Mese",lpad(sum(lpqtlet)::text,10,'0') as "Spedite" from rese_letture_prodotti where lpstato!='S' and lptplet='S' and to_char(lptmspc,'YYYYMM')=(select to_char(CURRENT_TIMESTAMP - interval '1 month','YYYYMM'))  group by to_char(lptmspc,'YYYYMM') order by to_char(lptmspc,'YYYYMM');
select lpad(ordprog,10,'0'),lpad(cpnmcol::text,4,'0'),lpad(cpprgcl::text,7,'0'),lpad(cppsrea::text,9,'0') from col_stor where substring(cpdteva from 1 for 6)>='201510' and substring(cpdteva from 1 for 6)<='201610'
union
select lpad(ordprog,10,'0'),lpad(cpnmcol::text,4,'0'),lpad(cpprgcl::text,7,'0'),lpad(cppsrea::text,9,'0') from col_prod where substring(cpdteva from 1 for 6)>='201510' and substring(cpdteva from 1 for 6)<='201610';
