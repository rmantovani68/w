delete from ordini_bertello where ordprog in (select ordprog from ric_ord where ronelab = '2000065480');
delete from colli_bertello where ordprog in (select ordprog from ric_ord where ronelab = '2000065480');
delete from righe_bertello where ordprog in (select ordprog from ric_ord where ronelab = '2000065480');

delete from ord_prod where ordprog in (select ordprog from ric_ord where ronelab = '2000065480');
delete from col_prod where ordprog in (select ordprog from ric_ord where ronelab = '2000065480');
delete from rig_prod where ordprog in (select ordprog from ric_ord where ronelab = '2000065480');
delete from ric_art where ordprog in (select ordprog from ric_ord where ronelab = '2000065480');
delete from ric_ord where ordprog in (select ordprog from ric_ord where ronelab = '2000065480');

