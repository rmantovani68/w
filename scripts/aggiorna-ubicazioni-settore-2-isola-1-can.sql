-- modifica colonne per nuove ubicazioni CAN isola 1 settore 2
-- 
-- ------------------------------------------------------------------------------------
-- settore 2 : modulo 2
-----------------------
update ubicazioni set modulo_can=2 where settore=2 and isola=1;
-- colonne

update ubicazioni set colonna_can=1  where colonna=  9 and settore=2 and isola=1;
update ubicazioni set colonna_can=2  where colonna= 10 and settore=2 and isola=1;
update ubicazioni set colonna_can=3  where colonna= 11 and settore=2 and isola=1;
update ubicazioni set colonna_can=4  where colonna=  5 and settore=2 and isola=1;
update ubicazioni set colonna_can=5  where colonna=  6 and settore=2 and isola=1;
update ubicazioni set colonna_can=6  where colonna= 12 and settore=2 and isola=1;
update ubicazioni set colonna_can=7  where colonna= 17 and settore=2 and isola=1;
update ubicazioni set colonna_can=8  where colonna= 18 and settore=2 and isola=1;
update ubicazioni set colonna_can=9  where colonna= 25 and settore=2 and isola=1;
update ubicazioni set colonna_can=10 where colonna= 26 and settore=2 and isola=1;

-- righe
update ubicazioni set riga_can= 1  where riga=  1 and settore=2 and isola=1;
update ubicazioni set riga_can= 2  where riga=  2 and settore=2 and isola=1;
update ubicazioni set riga_can= 3  where riga=  3 and settore=2 and isola=1;
update ubicazioni set riga_can= 4  where riga=  4 and settore=2 and isola=1;
update ubicazioni set riga_can= 5  where riga=  6 and settore=2 and isola=1;
update ubicazioni set riga_can= 6  where riga=  7 and settore=2 and isola=1;
update ubicazioni set riga_can= 7  where riga=  8 and settore=2 and isola=1;
update ubicazioni set riga_can= 8  where riga=  9 and settore=2 and isola=1;
update ubicazioni set riga_can= 9  where riga= 11 and settore=2 and isola=1;
update ubicazioni set riga_can=10  where riga= 12 and settore=2 and isola=1;
update ubicazioni set riga_can=11  where riga= 13 and settore=2 and isola=1;
update ubicazioni set riga_can=12  where riga= 14 and settore=2 and isola=1;
update ubicazioni set riga_can=13  where riga= 16 and settore=2 and isola=1;
update ubicazioni set riga_can=14  where riga= 17 and settore=2 and isola=1;
update ubicazioni set riga_can=15  where riga= 18 and settore=2 and isola=1;
update ubicazioni set riga_can=16  where riga= 19 and settore=2 and isola=1;
update ubicazioni set riga_can=17  where riga= 21 and settore=2 and isola=1;
update ubicazioni set riga_can=18  where riga= 22 and settore=2 and isola=1;
update ubicazioni set riga_can=19  where riga= 31 and settore=2 and isola=1;
update ubicazioni set riga_can=20  where riga= 32 and settore=2 and isola=1;

-- ------------------------------------------------------------------------------------
