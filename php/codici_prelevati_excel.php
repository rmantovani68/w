<?php
	require_once 'DB.php';
	require_once 'Spreadsheet/Excel/Writer.php';


	$db =& DB::connect('pgsql://roberto:3zin@database/sap');
	if (PEAR::isError($db)) {
    die($db->getMessage());
	}


	// We give the path to our file here
	//$workbook = new Spreadsheet_Excel_Writer('/u/prj/wella/data/test.xls');
	$workbook = new Spreadsheet_Excel_Writer();

	// sending HTTP headers
	$workbook->send('test.xls');

	$worksheet =& $workbook->addWorksheet('My first worksheet');

	$CB_DA_ANNO=$_GET['CB_DA_ANNO'];
	$CB_DA_MESE=$_GET['CB_DA_MESE'];
	$CB_DA_GIORNO=$_GET['CB_DA_GIORNO'];
	$CB_A_ANNO=$_GET['CB_A_ANNO'];
	$CB_A_MESE=$_GET['CB_A_MESE'];
	$CB_A_GIORNO=$_GET['CB_A_GIORNO'];

	$szBuffer=sprintf("LISTA CODICI PRELEVATI DAL %s-%s-%s AL %s-%s-%s", $CB_DA_ANNO, $CB_DA_MESE, $CB_DA_GIORNO, $CB_A_ANNO, $CB_A_MESE, $CB_A_GIORNO);
	//printf("<Font size=\"5\"><B> %s </B>",$szBuffer);


	$worksheet->write(0, 0, $szBuffer);

	if($CB_DA_ANNO=="" && $CB_DA_MESE=="" && $CB_DA_GIORNO=="" &&
	   $CB_A_ANNO=="" && $CB_A_MESE=="" && $CB_A_GIORNO==""){
		/*
		* Ricavo la data odierna
		*/
		// $date = getdate();

		// $da_anno=date("Y");
		// $da_mese=date("m");
		// $da_giorno=date("d");
		// $a_anno=date("Y");
		// $a_mese=date("m");
		// $a_giorno=date("d");
		//$worksheet->write(1, 1, '1');
	} else {
		$da_anno=$CB_DA_ANNO;
		$da_mese=$CB_DA_MESE;
		$da_giorno=$CB_DA_GIORNO;
		$a_anno=$CB_A_ANNO;
		$a_mese=$CB_A_MESE;
		$a_giorno=$CB_A_GIORNO;
		//$worksheet->write(1, 1, '2');
	}

	if($da_anno>0){

		$szFromDate = sprintf("%04d%02d%02d",$da_anno,$da_mese,$da_giorno);
		$szToDate   = sprintf("%04d%02d%02d",$a_anno,$a_mese,$a_giorno);


		/*
		* rm 27-02-2001 : versione che NON prevede l'uso dei campi data e ora prelievo in righe di produzione
		*                 (e' retroattiva)
		*/
		$szBuffer=sprintf(" select prodotti_non_prelevati.isola,prodotti_non_prelevati.settore,prodotti_non_prelevati.prodotto,
												prodotti_non_prelevati.ubicazione,prodotti_non_prelevati.titolo,
												prodotti_non_prelevati.qt+coalesce(prodotti_prelevati.qt,0) as qta from
												(select isola(r.rpcdubi) as isola,settore(r.rpcdubi) as settore,r.rpcdpro as prodotto,
												ubicazione(r.rpcdubi) as ubicazione,a.prdescr as titolo,sum(r.rpqtspe) as qt 
												from rig_stor r,articoli a where r.rpcdpro=a.prcdpro and r.rpdtpre>='%s' and r.rpdtpre<='%s' 
												group by isola(r.rpcdubi),settore(r.rpcdubi),r.rpcdpro,r.rpcdubi,a.prdescr) as prodotti_non_prelevati
												left outer join
												(select u.isola as isola,u.settore as settore,u.codprod as prodotto,ubicazione(u.ubicazione) as ubicazione,
												a.prdescr as titolo ,0 as qt
												from ubicazioni u,articoli a where u.codprod=a.prcdpro) as prodotti_prelevati
												on (prodotti_non_prelevati.ubicazione=prodotti_prelevati.ubicazione
												and prodotti_non_prelevati.prodotto=prodotti_prelevati.prodotto)
												order by qta desc; ",$szFromDate,$szToDate);

		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}

		/*
		* controllo presenza codice elaborazione in archivio storico
		*/
		if($res->fetchInto($row)){
		}

		$worksheet->write(2, 0, 'ISOLA');
		$worksheet->write(2, 1, 'SETT');
		$worksheet->write(2, 2, 'CODICE');
		$worksheet->write(2, 3, 'UBIC');
		$worksheet->write(2, 4, 'DESCR');
		$worksheet->write(2, 5, 'QT');

		$nRiga=0;
		do{
			$worksheet->write(3+$nRiga, 0,$row[0]);
			$worksheet->write(3+$nRiga, 1,$row[1]);
			$worksheet->write(3+$nRiga, 2,$row[2]);
			$worksheet->write(3+$nRiga, 3,$row[3]);
			$worksheet->write(3+$nRiga, 4,$row[4]);
			$worksheet->write(3+$nRiga, 5,$row[5]);

			$nRiga++;
		} while($res->fetchInto($row));

		$res->free();
	}
	// We still need to explicitly close the workbook
	$workbook->close();
?>
