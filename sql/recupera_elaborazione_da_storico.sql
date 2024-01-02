-- drop table re_ord;
-- create table re_ord as select ordprog from ric_stor where ronelab = '2000065675';
-- insert into ric_ord select * from ric_stor where ordprog in (select ordprog from re_ord);
-- insert into ord_prod select * from ord_stor where ordprog in (select ordprog from re_ord);
-- insert into col_prod select * from col_stor where ordprog in (select ordprog from re_ord);
-- insert into rig_prod select * from rig_stor where ordprog in (select ordprog from re_ord);


delete from ric_ord where ordprog in (select ordprog from re_ord);
delete from ord_prod where ordprog in (select ordprog from re_ord);
delete from col_prod where ordprog in (select ordprog from re_ord);
delete from rig_prod where ordprog in (select ordprog from re_ord);
