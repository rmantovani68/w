-- Innanzitutto creo le tabelle

--create table ord_prod_2016_isola1 as 
--select * from ord_stor where opcdisl=1 and substring(opdteva from 1 for 6)>='201601' and substring(opdteva from 1 for 6)<='201612'
--union
--select * from ord_prod where opcdisl=1 and substring(opdteva from 1 for 6)>='201601' and substring(opdteva from 1 for 6)<='201612';
--
--create table ord_prod_2016_isola2 as 
--select * from ord_stor where opcdisl=2 and substring(opdteva from 1 for 6)>='201601' and substring(opdteva from 1 for 6)<='201612'
--union
--select * from ord_prod where opcdisl=2 and substring(opdteva from 1 for 6)>='201601' and substring(opdteva from 1 for 6)<='201612';
--
--create table col_prod_2016_isola1 as 
--select * from col_stor where ordkey in (select ordkey from ord_prod_2016_isola1) and substring(cpdteva from 1 for 6)>='201601' and substring(cpdteva from 1 for 6)<='201612'
--union
--select * from col_prod where ordkey in (select ordkey from ord_prod_2016_isola1) and substring(cpdteva from 1 for 6)>='201601' and substring(cpdteva from 1 for 6)<='201612';
--
--create table col_prod_2016_isola2 as 
--select * from col_stor where ordkey in (select ordkey from ord_prod_2016_isola2) and substring(cpdteva from 1 for 6)>='201601' and substring(cpdteva from 1 for 6)<='201612'
--union
--select * from col_prod where ordkey in (select ordkey from ord_prod_2016_isola2) and substring(cpdteva from 1 for 6)>='201601' and substring(cpdteva from 1 for 6)<='201612';
--create table ord_prod_2016_isola1_isola2 as 
--select * from ord_prod_2016_isola1 where ordprog in (select ordprog from ord_prod_2016_isola2); 
insert into ord_prod_2016_isola1_isola2 select * from ord_prod_2016_isola2 where ordprog in (select ordprog from ord_prod_2016_isola1);
--
--create table col_prod_2016_isola1_isola2 as 
--select * from (select * from col_prod_2016_isola1  where ordprog in (select ordprog from ord_prod_2016_isola1_isola2) union select * from col_prod_2016_isola2 where ordprog in (select ordprog from ord_prod_2016_isola1_isola2)) as tt;

select count(distinct (ordprog)) as tot_ordini_isola1, sum(opnmcll) as tot_colli_lav_su_isola1,sum(opnmcpe) as tot_copie_prel_su_isola1 from ord_prod_2016_isola1;
select count(distinct (ordprog)) as tot_ordini_isola2, sum(opnmcll) as tot_colli_lav_su_isola2,sum(opnmcpe) as tot_copie_prel_su_isola2 from ord_prod_2016_isola2;

select count(distinct (ordprog)) as ordini_solo_isola1, sum(opnmcll) as colli_solo_isola1,sum(opnmcpe) as copie_solo_isola1 from ord_prod_2016_isola1 where ordprog not in (select ordprog from ord_prod_2016_isola1_isola2);
select count(distinct (ordprog)) as ordini_solo_isola2, sum(opnmcll) as colli_solo_isola2,sum(opnmcpe) as copie_solo_isola2 from ord_prod_2016_isola2 where ordprog not in (select ordprog from ord_prod_2016_isola1_isola2);

select count(distinct (ordprog)) as ordini_misti, sum(opnmcll) as colli_misti,sum(opnmcpe) as copie_misti from ord_prod_2016_isola1_isola2 ;

select count(distinct (ordkey)) as ordini_misti_sottordini_isola1, sum(opnmcll) as colli_misti_sottordini_isola1,sum(opnmcpe) as copie_misti_sottordini_isola1 from ord_prod_2016_isola1_isola2 where opcdisl=1;
select count(distinct (ordkey)) as ordini_misti_sottordini_isola2, sum(opnmcll) as colli_misti_sottordini_isola2,sum(opnmcpe) as copie_misti_sottordini_isola2 from ord_prod_2016_isola1_isola2 where opcdisl=2;
