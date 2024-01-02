drop index col_stor_key;
drop index col_stor_ordprog;
drop index ord_stor_ordkey;
drop index ord_stor_ordprog;
drop index ric_stor_key;
drop index rig_stor_key;
drop index rig_stor_ordprog;
drop index rig_tmp_key;
drop index rig_tmp_ordprog;

create index col_stor_key on col_stor(ordkey);
create index col_stor_ordprog on col_stor(ordprog);
create unique index ord_stor_ordkey on ord_stor(ordkey);
create index ord_stor_ordprog on ord_stor(ordprog);
create index ric_stor_key on ric_stor(ordprog);
create index rig_stor_key on rig_stor(ordkey);
create index rig_stor_ordprog on rig_stor(ordprog);
create index rig_tmp_key on rig_tmp(ordkey);
create index rig_tmp_ordprog on rig_tmp(ordprog);
