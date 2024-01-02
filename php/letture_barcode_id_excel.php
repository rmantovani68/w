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
	$workbook->send('letture_barcode_id.xls');

	$worksheet =& $workbook->addWorksheet('Letture Barcode');

	$TXT_PRODOTTO=$_GET['TXT_PRODOTTO'];
	$TXT_BARCODE_ID=$_GET['TXT_BARCODE_ID'];
	$TXT_ORDINE=$_GET['TXT_ORDINE'];
	$TXT_TITOLO=$_GET['TXT_TITOLO'];
	$CB_DA_ANNO=$_GET['CB_DA_ANNO'];
	$CB_DA_MESE=$_GET['CB_DA_MESE'];
	$CB_DA_GIORNO=$_GET['CB_DA_GIORNO'];
	$CB_A_ANNO=$_GET['CB_A_ANNO'];
	$CB_A_MESE=$_GET['CB_A_MESE'];
	$CB_A_GIORNO=$_GET['CB_A_GIORNO'];

	//$szBuffer=sprintf("LISTA CODICI PRELEVATI DAL %s-%s-%s AL %s-%s-%s", $CB_DA_ANNO, $CB_DA_MESE, $CB_DA_GIORNO, $CB_A_ANNO, $CB_A_MESE, $CB_A_GIORNO);
	$szBuffer=sprintf("LETTURE BARCODE ID DAL %s-%s-%s AL %s-%s-%s, ORDINE: %s, PRODOTTO: %s, BARCODE_ID: %s, TITOLO: %s", $CB_DA_ANNO, $CB_DA_MESE, $CB_DA_GIORNO, $CB_A_ANNO, $CB_A_MESE, $CB_A_GIORNO,$TXT_ORDINE,$TXT_PRODOTTO,$TXT_BARCODE_ID,$TXT_TITOLO);
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
		$szBuffer = sprintf("select lbprbri,lbcdbri,lbcdpro,lbdstit,ordkey,lbnmcol,lbnmcpe,to_char(lbtmlet,'YYYY-MM-DD HH24:MI') from letture_barcode_id_prodotto where lbcdpro~*'%s' and lbcdbri~*'%s' and ordkey~*'%s' and lbdstit~*'%s' and to_char(lbtmlet,'YYYYMMDD')>='%s' and to_char(lbtmlet,'YYYYMMDD')<='%s';",$TXT_PRODOTTO,$TXT_BARCODE_ID,$TXT_ORDINE,$TXT_TITOLO,$szFromDate,$szToDate);

		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}

		/*
		* controllo presenza in archivio
		*/
		if($res->fetchInto($row)){
		}

		$worksheet->write(2, 0, 'PROGR');
		$worksheet->write(2, 1, 'BARCODE_ID');
		$worksheet->write(2, 2, 'CODICE');
		$worksheet->write(2, 3, 'TITOLO');
		$worksheet->write(2, 4, 'ORDINE');
		$worksheet->write(2, 5, 'COLLO');
		$worksheet->write(2, 6, 'COPIE');
		$worksheet->write(2, 7, 'DATA');

		$nRiga=0;
		do{
			$worksheet->write(3+$nRiga, 0,$row[0]);
			$worksheet->write(3+$nRiga, 1,"\"".$row[1]."\"");
			//$worksheet->write(3+$nRiga, 1,"".$row[1]);
			$worksheet->write(3+$nRiga, 2,$row[2]);
			$worksheet->write(3+$nRiga, 3,$row[3]);
			$worksheet->write(3+$nRiga, 4,$row[4]);
			$worksheet->write(3+$nRiga, 5,$row[5]);
			$worksheet->write(3+$nRiga, 6,$row[6]);
			$worksheet->write(3+$nRiga, 7,$row[7]);

			$nRiga++;
		} while($res->fetchInto($row));

		$res->free();
	}
	// We still need to explicitly close the workbook
	$workbook->close();
?>
