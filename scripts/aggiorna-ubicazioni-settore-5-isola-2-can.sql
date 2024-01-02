-- modifica colonne per nuove ubicazioni_can CAN isola 2 settore 5
-- 

-- ------------------------------------------------------------------------------------
-- isola 2 settore 5 : modulo 12
-----------------------
update ubicazioni set modulo_can=12 where settore=5 and isola=2;

-- colonne
update ubicazioni set colonna_can= 1  where colonna= 21 and settore=5 and isola=2;
update ubicazioni set colonna_can= 2  where colonna= 22 and settore=5 and isola=2;
update ubicazioni set colonna_can= 3  where colonna= 23 and settore=5 and isola=2;
update ubicazioni set colonna_can= 4  where colonna= 24 and settore=5 and isola=2;
update ubicazioni set colonna_can= 5  where colonna= 29 and settore=5 and isola=2;
update ubicazioni set colonna_can= 6  where colonna= 30 and settore=5 and isola=2;
update ubicazioni set colonna_can= 7  where colonna= 31 and settore=5 and isola=2;
update ubicazioni set colonna_can= 8  where colonna= 32 and settore=5 and isola=2;

-- righe
update ubicazioni set riga_can= 1  where riga= 11 and settore=5 and isola=2;
update ubicazioni set riga_can= 2  where riga= 12 and settore=5 and isola=2;
update ubicazioni set riga_can= 3  where riga= 13 and settore=5 and isola=2;
update ubicazioni set riga_can= 4  where riga= 14 and settore=5 and isola=2;
update ubicazioni set riga_can= 5  where riga= 15 and settore=5 and isola=2;
update ubicazioni set riga_can= 6  where riga= 16 and settore=5 and isola=2;
update ubicazioni set riga_can= 7  where riga= 17 and settore=5 and isola=2;
update ubicazioni set riga_can= 8  where riga= 18 and settore=5 and isola=2;
update ubicazioni set riga_can= 9  where riga= 19 and settore=5 and isola=2;
update ubicazioni set riga_can=10  where riga= 20 and settore=5 and isola=2;

-- ------------------------------------------------------------------------------------
