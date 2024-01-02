begin;

insert into ric_ord select * from ric_stor where ordprog='2415124119';
insert into ric_ord select * from ric_stor where ordprog='2415124121';
insert into ric_ord select * from ric_stor where ordprog='2415124122';
insert into ric_ord select * from ric_stor where ordprog='2415124125';
insert into ric_ord select * from ric_stor where ordprog='2415124129';
insert into ric_ord select * from ric_stor where ordprog='2415124133';
insert into ric_ord select * from ric_stor where ordprog='2415124139';
insert into ric_ord select * from ric_stor where ordprog='2415124140';
insert into ric_ord select * from ric_stor where ordprog='2415124145';
insert into ric_ord select * from ric_stor where ordprog='2415124146';

insert into ord_prod select * from ord_stor where ordprog='2415124119';
insert into ord_prod select * from ord_stor where ordprog='2415124121';
insert into ord_prod select * from ord_stor where ordprog='2415124122';
insert into ord_prod select * from ord_stor where ordprog='2415124125';
insert into ord_prod select * from ord_stor where ordprog='2415124129';
insert into ord_prod select * from ord_stor where ordprog='2415124133';
insert into ord_prod select * from ord_stor where ordprog='2415124139';
insert into ord_prod select * from ord_stor where ordprog='2415124140';
insert into ord_prod select * from ord_stor where ordprog='2415124145';
insert into ord_prod select * from ord_stor where ordprog='2415124146';

insert into col_prod select * from col_stor where ordprog='2415124119';
insert into col_prod select * from col_stor where ordprog='2415124121';
insert into col_prod select * from col_stor where ordprog='2415124122';
insert into col_prod select * from col_stor where ordprog='2415124125';
insert into col_prod select * from col_stor where ordprog='2415124129';
insert into col_prod select * from col_stor where ordprog='2415124133';
insert into col_prod select * from col_stor where ordprog='2415124139';
insert into col_prod select * from col_stor where ordprog='2415124140';
insert into col_prod select * from col_stor where ordprog='2415124145';
insert into col_prod select * from col_stor where ordprog='2415124146';

insert into rig_prod select * from rig_stor where ordprog='2415124119';
insert into rig_prod select * from rig_stor where ordprog='2415124121';
insert into rig_prod select * from rig_stor where ordprog='2415124122';
insert into rig_prod select * from rig_stor where ordprog='2415124125';
insert into rig_prod select * from rig_stor where ordprog='2415124129';
insert into rig_prod select * from rig_stor where ordprog='2415124133';
insert into rig_prod select * from rig_stor where ordprog='2415124139';
insert into rig_prod select * from rig_stor where ordprog='2415124140';
insert into rig_prod select * from rig_stor where ordprog='2415124145';
insert into rig_prod select * from rig_stor where ordprog='2415124146';

delete from ric_stor where ordprog='2415124119';
delete from ric_stor where ordprog='2415124121';
delete from ric_stor where ordprog='2415124122';
delete from ric_stor where ordprog='2415124125';
delete from ric_stor where ordprog='2415124129';
delete from ric_stor where ordprog='2415124133';
delete from ric_stor where ordprog='2415124139';
delete from ric_stor where ordprog='2415124140';
delete from ric_stor where ordprog='2415124145';
delete from ric_stor where ordprog='2415124146';

delete from ord_stor where ordprog='2415124119';
delete from ord_stor where ordprog='2415124121';
delete from ord_stor where ordprog='2415124122';
delete from ord_stor where ordprog='2415124125';
delete from ord_stor where ordprog='2415124129';
delete from ord_stor where ordprog='2415124133';
delete from ord_stor where ordprog='2415124139';
delete from ord_stor where ordprog='2415124140';
delete from ord_stor where ordprog='2415124145';
delete from ord_stor where ordprog='2415124146';

delete from col_stor where ordprog='2415124119';
delete from col_stor where ordprog='2415124121';
delete from col_stor where ordprog='2415124122';
delete from col_stor where ordprog='2415124125';
delete from col_stor where ordprog='2415124129';
delete from col_stor where ordprog='2415124133';
delete from col_stor where ordprog='2415124139';
delete from col_stor where ordprog='2415124140';
delete from col_stor where ordprog='2415124145';
delete from col_stor where ordprog='2415124146';

delete from rig_stor where ordprog='2415124119';
delete from rig_stor where ordprog='2415124121';
delete from rig_stor where ordprog='2415124122';
delete from rig_stor where ordprog='2415124125';
delete from rig_stor where ordprog='2415124129';
delete from rig_stor where ordprog='2415124133';
delete from rig_stor where ordprog='2415124139';
delete from rig_stor where ordprog='2415124140';
delete from rig_stor where ordprog='2415124145';
delete from rig_stor where ordprog='2415124146';

update ric_ord set rostato='E' where ordprog in (
'2415124119',
'2415124121',
'2415124122',
'2415124125',
'2415124129',
'2415124133',
'2415124139',
'2415124140',
'2415124145',
'2415124146');

update ord_prod set opstato='E' where ordprog in (
'2415124119',
'2415124121',
'2415124122',
'2415124125',
'2415124129',
'2415124133',
'2415124139',
'2415124140',
'2415124145',
'2415124146');

update col_prod set cpstato='E' where ordprog in (
'2415124119',
'2415124121',
'2415124122',
'2415124125',
'2415124129',
'2415124133',
'2415124139',
'2415124140',
'2415124145',
'2415124146');

update rig_prod set rpstato='E' where ordprog in (
'2415124119',
'2415124121',
'2415124122',
'2415124125',
'2415124129',
'2415124133',
'2415124139',
'2415124140',
'2415124145',
'2415124146');

commit;
