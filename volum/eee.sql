with test as 
(
select sum(raqtord) as copie_ordinate,0 as copie_spedite,ordprog from ric_art where ordprog in (select ordprog from ric_ord where rostato='A') group by ordprog 
union 
select 0 as copie_ordinate,sum(rpqtspe) as copie_spedite,ordprog from rig_prod where ordprog in (select ordprog from ric_ord where rostato='A') group by ordprog
) 
select sum(copie_ordinate),sum(copie_spedite),ordprog from test group by ordprog;
