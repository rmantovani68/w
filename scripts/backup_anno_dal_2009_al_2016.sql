-- create table ric_stor_backup_2009 as select * from ric_stor where substr(rodteva,1,4)='2009';
-- create table rig_stor_backup_2009 as select * from rig_stor where ordprog in (select ordprog from ric_stor_backup_2009);
-- create table col_stor_backup_2009 as select * from col_stor where ordprog in (select ordprog from ric_stor_backup_2009);
-- create table ord_stor_backup_2009 as select * from ord_stor where ordprog in (select ordprog from ric_stor_backup_2009);
-- 
-- create table ric_stor_backup_2010 as select * from ric_stor where substr(rodteva,1,4)='2010';
-- create table rig_stor_backup_2010 as select * from rig_stor where ordprog in (select ordprog from ric_stor_backup_2010);
-- create table col_stor_backup_2010 as select * from col_stor where ordprog in (select ordprog from ric_stor_backup_2010);
-- create table ord_stor_backup_2010 as select * from ord_stor where ordprog in (select ordprog from ric_stor_backup_2010);
-- 
-- create table ric_stor_backup_2011 as select * from ric_stor where substr(rodteva,1,4)='2011';
-- create table rig_stor_backup_2011 as select * from rig_stor where ordprog in (select ordprog from ric_stor_backup_2011);
-- create table col_stor_backup_2011 as select * from col_stor where ordprog in (select ordprog from ric_stor_backup_2011);
-- create table ord_stor_backup_2011 as select * from ord_stor where ordprog in (select ordprog from ric_stor_backup_2011);
-- 
-- create table ric_stor_backup_2012 as select * from ric_stor where substr(rodteva,1,4)='2012';
-- create table rig_stor_backup_2012 as select * from rig_stor where ordprog in (select ordprog from ric_stor_backup_2012);
-- create table col_stor_backup_2012 as select * from col_stor where ordprog in (select ordprog from ric_stor_backup_2012);
-- create table ord_stor_backup_2012 as select * from ord_stor where ordprog in (select ordprog from ric_stor_backup_2012);
-- 
-- create table ric_stor_backup_2013 as select * from ric_stor where substr(rodteva,1,4)='2013';
-- create table rig_stor_backup_2013 as select * from rig_stor where ordprog in (select ordprog from ric_stor_backup_2013);
-- create table col_stor_backup_2013 as select * from col_stor where ordprog in (select ordprog from ric_stor_backup_2013);
-- create table ord_stor_backup_2013 as select * from ord_stor where ordprog in (select ordprog from ric_stor_backup_2013);
-- 
-- create table ric_stor_backup_2014 as select * from ric_stor where substr(rodteva,1,4)='2014';
-- create table rig_stor_backup_2014 as select * from rig_stor where ordprog in (select ordprog from ric_stor_backup_2014);
-- create table col_stor_backup_2014 as select * from col_stor where ordprog in (select ordprog from ric_stor_backup_2014);
-- create table ord_stor_backup_2014 as select * from ord_stor where ordprog in (select ordprog from ric_stor_backup_2014);
-- 
-- create table ric_stor_backup_2015 as select * from ric_stor where substr(rodteva,1,4)='2015';
-- create table rig_stor_backup_2015 as select * from rig_stor where ordprog in (select ordprog from ric_stor_backup_2015);
-- create table col_stor_backup_2015 as select * from col_stor where ordprog in (select ordprog from ric_stor_backup_2015);
-- create table ord_stor_backup_2015 as select * from ord_stor where ordprog in (select ordprog from ric_stor_backup_2015);
-- 
-- create table ric_stor_backup_2016 as select * from ric_stor where substr(rodteva,1,4)='2016';
-- create table rig_stor_backup_2016 as select * from rig_stor where ordprog in (select ordprog from ric_stor_backup_2016);
-- create table col_stor_backup_2016 as select * from col_stor where ordprog in (select ordprog from ric_stor_backup_2016);
-- create table ord_stor_backup_2016 as select * from ord_stor where ordprog in (select ordprog from ric_stor_backup_2016);
-- 
delete from ric_stor where ordprog in (select ordprog from ric_stor_backup_2008);
delete from rig_stor where ordprog in (select ordprog from ric_stor_backup_2008);
delete from col_stor where ordprog in (select ordprog from ric_stor_backup_2008);
delete from ord_stor where ordprog in (select ordprog from ric_stor_backup_2008);

delete from ric_stor where ordprog in (select ordprog from ric_stor_backup_2009);
delete from rig_stor where ordprog in (select ordprog from ric_stor_backup_2009);
delete from col_stor where ordprog in (select ordprog from ric_stor_backup_2009);
delete from ord_stor where ordprog in (select ordprog from ric_stor_backup_2009);

delete from ric_stor where ordprog in (select ordprog from ric_stor_backup_2010);
delete from rig_stor where ordprog in (select ordprog from ric_stor_backup_2010);
delete from col_stor where ordprog in (select ordprog from ric_stor_backup_2010);
delete from ord_stor where ordprog in (select ordprog from ric_stor_backup_2010);

delete from ric_stor where ordprog in (select ordprog from ric_stor_backup_2011);
delete from rig_stor where ordprog in (select ordprog from ric_stor_backup_2011);
delete from col_stor where ordprog in (select ordprog from ric_stor_backup_2011);
delete from ord_stor where ordprog in (select ordprog from ric_stor_backup_2011);

delete from ric_stor where ordprog in (select ordprog from ric_stor_backup_2012);
delete from rig_stor where ordprog in (select ordprog from ric_stor_backup_2012);
delete from col_stor where ordprog in (select ordprog from ric_stor_backup_2012);
delete from ord_stor where ordprog in (select ordprog from ric_stor_backup_2012);

delete from ric_stor where ordprog in (select ordprog from ric_stor_backup_2013);
delete from rig_stor where ordprog in (select ordprog from ric_stor_backup_2013);
delete from col_stor where ordprog in (select ordprog from ric_stor_backup_2013);
delete from ord_stor where ordprog in (select ordprog from ric_stor_backup_2013);

delete from ric_stor where ordprog in (select ordprog from ric_stor_backup_2014);
delete from rig_stor where ordprog in (select ordprog from ric_stor_backup_2014);
delete from col_stor where ordprog in (select ordprog from ric_stor_backup_2014);
delete from ord_stor where ordprog in (select ordprog from ric_stor_backup_2014);

delete from ric_stor where ordprog in (select ordprog from ric_stor_backup_2015);
delete from rig_stor where ordprog in (select ordprog from ric_stor_backup_2015);
delete from col_stor where ordprog in (select ordprog from ric_stor_backup_2015);
delete from ord_stor where ordprog in (select ordprog from ric_stor_backup_2015);

delete from ric_stor where ordprog in (select ordprog from ric_stor_backup_2016);
delete from rig_stor where ordprog in (select ordprog from ric_stor_backup_2016);
delete from col_stor where ordprog in (select ordprog from ric_stor_backup_2016);
delete from ord_stor where ordprog in (select ordprog from ric_stor_backup_2016);
