-- Prima creare la tabella con dentro gli ordprog, poi inserimenti da storico
-- infine cancellare tutto
--begin;
--create table tmp_ordini_elab_2000041991 as select ordprog from ric_stor where ronelab='2000041991';
--insert into ric_ord select * from ric_stor where ordprog in (select ordprog from tmp_ordini_elab_2000041991);
--insert into col_prod select * from col_stor where ordprog in (select ordprog from tmp_ordini_elab_2000041991);
--insert into rig_prod select * from rig_stor where ordprog in (select ordprog from tmp_ordini_elab_2000041991);
--insert into ord_prod select * from ord_stor where ordprog in (select ordprog from tmp_ordini_elab_2000041991);
--update ric_ord set rostato='A' where ordprog in (select ordprog from tmp_ordini_elab_2000041991);
--update ord_prod set opstato='A' where ordprog in (select ordprog from tmp_ordini_elab_2000041991);
--commit;
delete from ric_ord   where ordprog in (select ordprog from tmp_ordini_elab_2000041991);
delete from col_prod  where ordprog in (select ordprog from tmp_ordini_elab_2000041991);
delete from rig_prod  where ordprog in (select ordprog from tmp_ordini_elab_2000041991);
delete from ord_prod  where ordprog in (select ordprog from tmp_ordini_elab_2000041991);
