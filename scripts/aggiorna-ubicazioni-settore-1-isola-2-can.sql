-- modifica colonne per nuove ubicazioni_can CAN isola 2 settore 1
-- 

-- ------------------------------------------------------------------------------------
-- isola 2 settore 1 : modulo 9
-----------------------
update ubicazioni set modulo_can=9 where settore=1 and isola=2;

-- colonne
update ubicazioni set colonna_can=12  where colonna= 17 and settore=1 and isola=2;
update ubicazioni set colonna_can=10  where colonna= 18 and settore=1 and isola=2;

-- righe
update ubicazioni set riga_can= 9  where riga= 31 and settore=1 and isola=2;
update ubicazioni set riga_can=10  where riga= 32 and settore=1 and isola=2;

-- ------------------------------------------------------------------------------------
