-- modifica colonne per nuove ubicazioni CAN isola 1 settore 5
-- 
-- ------------------------------------------------------------------------------------
-- settore 5 : modulo 5
-----------------------
update ubicazioni set modulo_can=5 where settore=5 and isola=1;
-- colonne

update ubicazioni set colonna_can=1  where colonna= 21 and settore=5 and isola=1;
update ubicazioni set colonna_can=2  where colonna= 22 and settore=5 and isola=1;
update ubicazioni set colonna_can=3  where colonna= 23 and settore=5 and isola=1;
update ubicazioni set colonna_can=4  where colonna= 24 and settore=5 and isola=1;
update ubicazioni set colonna_can=5  where colonna= 29 and settore=5 and isola=1;
update ubicazioni set colonna_can=6  where colonna= 30 and settore=5 and isola=1;
update ubicazioni set colonna_can=7  where colonna= 31 and settore=5 and isola=1;
update ubicazioni set colonna_can=8  where colonna= 32 and settore=5 and isola=1;

-- righe
update ubicazioni set riga_can= 1  where riga= 21 and settore=5 and isola=1;
update ubicazioni set riga_can= 2  where riga= 22 and settore=5 and isola=1;
update ubicazioni set riga_can= 3  where riga= 23 and settore=5 and isola=1;
update ubicazioni set riga_can= 4  where riga= 24 and settore=5 and isola=1;
update ubicazioni set riga_can= 5  where riga= 25 and settore=5 and isola=1;
update ubicazioni set riga_can= 6  where riga= 26 and settore=5 and isola=1;
update ubicazioni set riga_can= 7  where riga= 27 and settore=5 and isola=1;
update ubicazioni set riga_can= 8  where riga= 28 and settore=5 and isola=1;
update ubicazioni set riga_can= 9  where riga= 29 and settore=5 and isola=1;
update ubicazioni set riga_can=10  where riga= 30 and settore=5 and isola=1;

-- ------------------------------------------------------------------------------------
