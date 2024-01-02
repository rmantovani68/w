-- 2002 
drop table ric_stor_2002;create table ric_stor_2002 as select * from ric_stor where substr(rodteva,1,4)='2002';
drop table ord_stor_2002;create table ord_stor_2002 as select * from ord_stor where substr(opdteva,1,4)='2002';
drop table col_stor_2002;create table col_stor_2002 as select * from col_stor where substr(cpdteva,1,4)='2002';
drop table rig_stor_2002;create table rig_stor_2002 as select * from rig_stor where ordprog in (select ordprog from ric_stor_2002);

delete from ric_stor where substr(rodteva,1,4)='2002';
delete from ord_stor where substr(opdteva,1,4)='2002';
delete from col_stor where substr(cpdteva,1,4)='2002';
delete from rig_stor where ordprog in (select ordprog from ric_stor_2002);
-- 2003 
drop table ric_stor_2003;create table ric_stor_2003 as select * from ric_stor where substr(rodteva,1,4)='2003';
drop table ord_stor_2003;create table ord_stor_2003 as select * from ord_stor where substr(opdteva,1,4)='2003';
drop table col_stor_2003;create table col_stor_2003 as select * from col_stor where substr(cpdteva,1,4)='2003';
drop table rig_stor_2003;create table rig_stor_2003 as select * from rig_stor where ordprog in (select ordprog from ric_stor_2003);

delete from ric_stor where substr(rodteva,1,4)='2003';
delete from ord_stor where substr(opdteva,1,4)='2003';
delete from col_stor where substr(cpdteva,1,4)='2003';
delete from rig_stor where ordprog in (select ordprog from ric_stor_2003);
-- 2004 
drop table ric_stor_2004;create table ric_stor_2004 as select * from ric_stor where substr(rodteva,1,4)='2004';
drop table ord_stor_2004;create table ord_stor_2004 as select * from ord_stor where substr(opdteva,1,4)='2004';
drop table col_stor_2004;create table col_stor_2004 as select * from col_stor where substr(cpdteva,1,4)='2004';
drop table rig_stor_2004;create table rig_stor_2004 as select * from rig_stor where ordprog in (select ordprog from ric_stor_2004);

delete from ric_stor where substr(rodteva,1,4)='2004';
delete from ord_stor where substr(opdteva,1,4)='2004';
delete from col_stor where substr(cpdteva,1,4)='2004';
delete from rig_stor where ordprog in (select ordprog from ric_stor_2004);
-- 2005 
drop table ric_stor_2005;create table ric_stor_2005 as select * from ric_stor where substr(rodteva,1,4)='2005';
drop table ord_stor_2005;create table ord_stor_2005 as select * from ord_stor where substr(opdteva,1,4)='2005';
drop table col_stor_2005;create table col_stor_2005 as select * from col_stor where substr(cpdteva,1,4)='2005';
drop table rig_stor_2005;create table rig_stor_2005 as select * from rig_stor where ordprog in (select ordprog from ric_stor_2005);

delete from ric_stor where substr(rodteva,1,4)='2005';
delete from ord_stor where substr(opdteva,1,4)='2005';
delete from col_stor where substr(cpdteva,1,4)='2005';
delete from rig_stor where ordprog in (select ordprog from ric_stor_2005);
-- 2006 
drop table ric_stor_2006;create table ric_stor_2006 as select * from ric_stor where substr(rodteva,1,4)='2006';
drop table ord_stor_2006;create table ord_stor_2006 as select * from ord_stor where substr(opdteva,1,4)='2006';
drop table col_stor_2006;create table col_stor_2006 as select * from col_stor where substr(cpdteva,1,4)='2006';
drop table rig_stor_2006;create table rig_stor_2006 as select * from rig_stor where ordprog in (select ordprog from ric_stor_2006);

delete from ric_stor where substr(rodteva,1,4)='2006';
delete from ord_stor where substr(opdteva,1,4)='2006';
delete from col_stor where substr(cpdteva,1,4)='2006';
delete from rig_stor where ordprog in (select ordprog from ric_stor_2006);
-- 2007 
drop table ric_stor_2007;create table ric_stor_2007 as select * from ric_stor where substr(rodteva,1,4)='2007';
drop table ord_stor_2007;create table ord_stor_2007 as select * from ord_stor where substr(opdteva,1,4)='2007';
drop table col_stor_2007;create table col_stor_2007 as select * from col_stor where substr(cpdteva,1,4)='2007';
drop table rig_stor_2007;create table rig_stor_2007 as select * from rig_stor where ordprog in (select ordprog from ric_stor_2007);

delete from ric_stor where substr(rodteva,1,4)='2007';
delete from ord_stor where substr(opdteva,1,4)='2007';
delete from col_stor where substr(cpdteva,1,4)='2007';
delete from rig_stor where ordprog in (select ordprog from ric_stor_2007);

vacuum analyze;
