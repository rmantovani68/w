-- modifica colonne per nuove ubicazioni CAN isola 1 settore 3
-- 
-- ------------------------------------------------------------------------------------
-- settore 3 : modulo 3
-----------------------
update ubicazioni set modulo_can=3 where settore=3 and isola=1;
-- colonne

update ubicazioni set colonna_can=1  where colonna= 17 and settore=3 and isola=1;
update ubicazioni set colonna_can=2  where colonna= 18 and settore=3 and isola=1;
update ubicazioni set colonna_can=3  where colonna= 19 and settore=3 and isola=1;
update ubicazioni set colonna_can=4  where colonna= 20 and settore=3 and isola=1;
update ubicazioni set colonna_can=5  where colonna= 25 and settore=3 and isola=1;
update ubicazioni set colonna_can=6  where colonna= 26 and settore=3 and isola=1;
update ubicazioni set colonna_can=7  where colonna= 27 and settore=3 and isola=1;
update ubicazioni set colonna_can=8  where colonna= 28 and settore=3 and isola=1;

-- righe
update ubicazioni set riga_can= 1  where riga= 11 and settore=3 and isola=1;
update ubicazioni set riga_can= 2  where riga= 12 and settore=3 and isola=1;
update ubicazioni set riga_can= 3  where riga= 13 and settore=3 and isola=1;
update ubicazioni set riga_can= 4  where riga= 14 and settore=3 and isola=1;
update ubicazioni set riga_can= 5  where riga= 16 and settore=3 and isola=1;
update ubicazioni set riga_can= 6  where riga= 17 and settore=3 and isola=1;
update ubicazioni set riga_can= 7  where riga= 18 and settore=3 and isola=1;
update ubicazioni set riga_can= 8  where riga= 19 and settore=3 and isola=1;
update ubicazioni set riga_can= 9  where riga=  1 and settore=3 and isola=1;
update ubicazioni set riga_can=10  where riga=  2 and settore=3 and isola=1;
update ubicazioni set riga_can=11  where riga=  3 and settore=3 and isola=1;
update ubicazioni set riga_can=12  where riga=  4 and settore=3 and isola=1;
update ubicazioni set riga_can=13  where riga=  6 and settore=3 and isola=1;
update ubicazioni set riga_can=14  where riga=  7 and settore=3 and isola=1;
update ubicazioni set riga_can=15  where riga=  8 and settore=3 and isola=1;
update ubicazioni set riga_can=16  where riga=  9 and settore=3 and isola=1;

-- ------------------------------------------------------------------------------------
