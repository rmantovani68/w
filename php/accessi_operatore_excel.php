<?php
	require_once 'DB.php';
	require_once 'Spreadsheet/Excel/Writer.php';


	$db =& DB::connect('pgsql://roberto:3zin@database/sap');
	if (PEAR::isError($db)) {
    die($db->getMessage());
	}


	// We give the path to our file here
	$workbook = new Spreadsheet_Excel_Writer();

	// sending HTTP headers
	$workbook->send('accessi_operatore.xls');

	$worksheet =& $workbook->addWorksheet('Accessi Operatore');

	$TXT_OPERATORE=$_GET['TXT_OPERATORE'];
	$CB_DA_ANNO=$_GET['CB_DA_ANNO'];
	$CB_DA_MESE=$_GET['CB_DA_MESE'];
	$CB_DA_GIORNO=$_GET['CB_DA_GIORNO'];
	$CB_A_ANNO=$_GET['CB_A_ANNO'];
	$CB_A_MESE=$_GET['CB_A_MESE'];
	$CB_A_GIORNO=$_GET['CB_A_GIORNO'];

	$szBuffer=sprintf("ACCESSI OPERATORE DAL %s-%s-%s AL %s-%s-%s, OPERATORE: %s", $CB_DA_ANNO, $CB_DA_MESE, $CB_DA_GIORNO, $CB_A_ANNO, $CB_A_MESE, $CB_A_GIORNO,$TXT_OPERATORE);


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
		*/
		$szBuffer = sprintf("select to_char(read_time,'YYYY-MM-DD HH24:MI'),isola_id,settore_id,stato_lettura  from badge_registry where operatore_id='%s' and to_char(read_time,'YYYYMMDD')>='%s' and to_char(read_time,'YYYYMMDD')<='%s' order by read_time;",$TXT_OPERATORE,$szFromDate,$szToDate);

		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}

		/*
		* controllo presenza in archivio
		*/
		if($res->fetchInto($row)){
		}

		$worksheet->write(2, 0, 'DATA/ORA');
		$worksheet->write(2, 1, 'ISOLA');
		$worksheet->write(2, 2, 'SETTORE');
		$worksheet->write(2, 3, 'STATO LETTURA');

		$nRiga=0;
		do{
			$worksheet->write(3+$nRiga, 0,$row[0]);
			$worksheet->write(3+$nRiga, 1,$row[1]);
			$worksheet->write(3+$nRiga, 2,$row[2]);
			$worksheet->write(3+$nRiga, 3,$row[3]);

			$nRiga++;
		} while($res->fetchInto($row));

		$res->free();
	}
	// We still need to explicitly close the workbook
	$workbook->close();
?>
