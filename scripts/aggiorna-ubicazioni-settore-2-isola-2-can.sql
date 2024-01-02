-- modifica colonne per nuove ubicazioni_can CAN isola 2 settore 2
-- 

-- ------------------------------------------------------------------------------------
-- isola 2 settore 2 : modulo 9
-----------------------
update ubicazioni set modulo_can=9 where settore=2 and isola=2;

-- colonne
update ubicazioni set colonna_can=1  where colonna=  9 and settore=2 and isola=2;
update ubicazioni set colonna_can=2  where colonna= 10 and settore=2 and isola=2;
update ubicazioni set colonna_can=3  where colonna= 11 and settore=2 and isola=2;
update ubicazioni set colonna_can=4  where colonna= 12 and settore=2 and isola=2;
update ubicazioni set colonna_can=5  where colonna= 13 and settore=2 and isola=2;
update ubicazioni set colonna_can=6  where colonna= 14 and settore=2 and isola=2;
update ubicazioni set colonna_can=7  where colonna=  8 and settore=2 and isola=2;
update ubicazioni set colonna_can=8  where colonna= 16 and settore=2 and isola=2;

-- righe
update ubicazioni set riga_can= 1  where riga= 25 and settore=2 and isola=2;
update ubicazioni set riga_can= 2  where riga= 26 and settore=2 and isola=2;
update ubicazioni set riga_can= 3  where riga= 27 and settore=2 and isola=2;
update ubicazioni set riga_can= 4  where riga= 28 and settore=2 and isola=2;
update ubicazioni set riga_can= 5  where riga= 29 and settore=2 and isola=2;
update ubicazioni set riga_can= 6  where riga= 30 and settore=2 and isola=2;
update ubicazioni set riga_can= 7  where riga= 31 and settore=2 and isola=2;
update ubicazioni set riga_can= 8  where riga= 32 and settore=2 and isola=2;

-- ------------------------------------------------------------------------------------
