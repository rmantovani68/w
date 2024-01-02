drop table if exists ric_ord_cancellati;
drop table if exists ric_art_cancellati;
drop table if exists ord_prod_cancellati;
drop table if exists col_prod_cancellati;
drop table if exists rig_prod_cancellati;

create table ric_ord_cancellati as select * from ric_ord where ronelab in  (
'2000064593',
'2000064594',
'2000064751',
'2000065477',
'2000065480');

create table  ric_art_cancellati as select * from ric_art where ordprog in (select ordprog from ric_ord_cancellati);
create table ord_prod_cancellati as select * from ord_prod where ordprog in (select ordprog from ric_ord_cancellati);
create table col_prod_cancellati as select * from col_prod where ordprog in (select ordprog from ric_ord_cancellati);
create table rig_prod_cancellati as select * from rig_prod where ordprog in (select ordprog from ric_ord_cancellati);

delete from  ric_ord where ordprog in (select ordprog from ric_ord_cancellati);
delete from  ric_art where ordprog in (select ordprog from ric_ord_cancellati);
delete from ord_prod where ordprog in (select ordprog from ric_ord_cancellati);
delete from col_prod where ordprog in (select ordprog from ric_ord_cancellati);
delete from rig_prod where ordprog in (select ordprog from ric_ord_cancellati);
