#!/bin/bash
echo "
begin;
insert into ric_ord select * from ric_stor where ordprog='$1';
insert into ord_prod select * from ord_stor where ordprog='$1';
insert into col_prod select * from col_stor where ordprog='$1';
insert into rig_prod select * from rig_stor where ordprog='$1';
delete from ric_stor where ordprog='$1';
delete from ord_stor where ordprog='$1';
delete from col_stor where ordprog='$1';
delete from rig_stor where ordprog='$1';
update ric_ord set rostato='A' where ordprog='$1';
update ord_prod set opstato='A' where ordprog='$1' and opcdisl=$2;
update col_prod set cpstato='A' where ordprog='$1';
update rig_prod set rpstato='A' where ordprog='$1';
commit;
" | psql sap
