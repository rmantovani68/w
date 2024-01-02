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

	$DA_ANNO=$_GET['CB_DA_ANNO'];
	$DA_MESE=$_GET['CB_DA_MESE'];
	$DA_GIORNO=$_GET['CB_DA_GIORNO'];
	$A_ANNO=$_GET['CB_A_ANNO'];
	$A_MESE=$_GET['CB_A_MESE'];
	$A_GIORNO=$_GET['CB_A_GIORNO'];

	$szBuffer=sprintf("LISTA CODICI PRELEVATI DAL %s", $DA_ANNO);
	$worksheet->write(0, 0, $_GET['CB_DA_ANNO']);

	$szBuffer=sprintf("LISTA CODICI PRELEVATI DAL %s", $DA_ANNO);
	$worksheet->write(0, 0, $szBuffer);

	if($DA_ANNO=="" && $DA_MESE=="" && $DA_GIORNO=="" &&
	   $A_ANNO=="" && $A_MESE=="" && $A_GIORNO==""){
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
		//$worksheet->write(0, 1, '1');
	} else {
		$da_anno=$DA_ANNO;
		$da_mese=$DA_MESE;
		$da_giorno=$DA_GIORNO;
		$a_anno=$A_ANNO;
		$a_mese=$A_MESE;
		$a_giorno=$A_GIORNO;
		//$worksheet->write(0, 1, '2');
	}


//$worksheet->write(0, 1, 'Age');
//$worksheet->write(1, 0, 'John Smith');
//$worksheet->write(1, 1, 30);
//$worksheet->write(2, 0, 'Johann Schmidt');
//$worksheet->write(2, 1, 31);
//$worksheet->write(3, 0, 'Juan Herrera');
//$worksheet->write(3, 1, 32);

	// $szBuffer=sprintf("LISTA CODICI PRELEVATI DAL %02d-%02d-%04d AL %02d-%02d-%04d</B>",$da_giorno,$da_mese,$da_anno,$a_giorno,$a_mese,$a_anno);

	//$worksheet->write(1, 0, $szBuffer);

//	if($DA_ANNO>0){
//
//		$szFromDate = sprintf("%04d%02d%02d",$da_anno,$da_mese,$da_giorno);
//		$szToDate   = sprintf("%04d%02d%02d",$a_anno,$a_mese,$a_giorno);
//
//
//		/*
//		* rm 27-02-2001 : versione che NON prevede l'uso dei campi data e ora prelievo in righe di produzione
//		*                 (e' retroattiva)
//		*/
//		$szBuffer=sprintf(" with prodotti_prelevati as (select isola(r.rpcdubi) as isola,settore(r.rpcdubi) as settore,r.rpcdpro as prodotto,ubicazione(r.rpcdubi) as ubicazione,a.prdescr as titolo,sum(r.rpqtspe) as qt 
//												from rig_stor r,articoli a where r.rpcdpro=a.prcdpro and r.rpdtpre>='%s' and r.rpdtpre<='%s' 
//												group by isola(r.rpcdubi),settore(r.rpcdubi),r.rpcdpro,r.rpcdubi,a.prdescr),
//												prodotti_non_prelevati as (select u.isola as isola,u.settore as settore,u.codprod as prodotto,ubicazione(u.ubicazione) as ubicazione,a.prdescr as titolo ,0 as qt
//												from ubicazioni u,articoli a where u.codprod=a.prcdpro)  
//												select prodotti_non_prelevati.isola,prodotti_non_prelevati.settore,prodotti_non_prelevati.prodotto,prodotti_non_prelevati.ubicazione,prodotti_non_prelevati.titolo,prodotti_non_prelevati.qt+coalesce(prodotti_prelevati.qt,0) as qta from
//												prodotti_non_prelevati left outer join prodotti_prelevati on (prodotti_non_prelevati.ubicazione=prodotti_prelevati.ubicazione and prodotti_non_prelevati.prodotto=prodotti_prelevati.prodotto)
//												order by qta desc; ",$szFromDate,$szToDate);
//
//		$res =& $db->query($szBuffer);
//		if (PEAR::isError($res)) {
//			die($res->getMessage());
//		}
//
//		/*
//		* controllo presenza codice elaborazione in archivio storico
//		*/
//		if($res->fetchInto($row)){
//		}
//		$isola=$row[0];
//		$settore=$row[1];
//		$old_isola=-1;
//		$old_settore=0;
//		if($row[5]==0){
//
//			printf("<Font size=\"5\"><B> NON DISPONIBILE</B>");
//		
//		} else {
//
//
//			printf("<br><br><table BORDER=1 COLS=5 WIDTH=\"70%%\" BGCOLOR=\"#999999\"><tr><th align=left>ISOLA</th><th align=left>SETTORE</th><th align=left>CODICE</th><th align=left>
//			UBICAZIONE</th><th align=left>DESCRIZIONE</th><th align=left>QUANTITA' PRELEVATA</th></tr>");
//
//			//$worksheet->write(2, 0, 'ISOLA');
//			//$worksheet->write(2, 1, 'SETTORE');
//			//$worksheet->write(2, 2, 'CODICE');
//			//$worksheet->write(2, 3, 'UBICAZIONE');
//			//$worksheet->write(2, 4, 'DESCRIZIONE');
//			//$worksheet->write(2, 5, 'QUANTITA\' PRELEVATA');
//
//			do{
//				if($old_isola!=$isola){
//					// printf("<tr>Isola numero %d</tr>",$isola);
//					$old_isola=$isola;
//				}
//				if($old_settore!=$settore){
//					// printf("<tr>Settore numero %d</tr>",$settore);
//					$old_settore=$settore;
//				}
//				// printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%d</td></tr>",$row[0],$row[1],$row[2],$row[3],$row[4],$row[5]);
//
//				// $worksheet->write(0, 0, $da_anno);
//
//				$isola=$row[0];
//				$settore=$row[1];
//			} while($res->fetchInto($row));
//
//		}
//		$res->free();
//	}




	// We still need to explicitly close the workbook
	$workbook->close();
?>
