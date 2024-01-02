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
	$workbook->send('statistica_operatori.xls');

	$worksheet =& $workbook->addWorksheet('Statistica Operatori');

	$CB_DA_ANNO=$_GET['CB_DA_ANNO'];
	$CB_DA_MESE=$_GET['CB_DA_MESE'];
	$CB_DA_GIORNO=$_GET['CB_DA_GIORNO'];
	$CB_A_ANNO=$_GET['CB_A_ANNO'];
	$CB_A_MESE=$_GET['CB_A_MESE'];
	$CB_A_GIORNO=$_GET['CB_A_GIORNO'];

	$szBuffer=sprintf("STATISTICA PRELIEVO OPERATORI DAL %s-%s-%s AL %s-%s-%s", $CB_DA_ANNO, $CB_DA_MESE, $CB_DA_GIORNO, $CB_A_ANNO, $CB_A_MESE, $CB_A_GIORNO);


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
		$szBuffer = sprintf("select rpcdopr,righe,quantita,coalesce(errori,0) as errori_operatore, (coalesce(errori,0)*100)/quantita::float from (select rpcdopr,count(*) as righe,sum(rpqtspe) as quantita from (select * from rig_stor union select * from rig_prod) tt  where to_char(rptmprl,'YYYYMMDD')>='%s' and to_char(rptmprl,'YYYYMMDD')<='%s' and rptmprl is not null group by rpcdopr) t_righe left outer join (select epcdopr,count(*) as errori from errori_prelievo where to_char(eptmprl,'YYYYMMDD')>='%s' and to_char(eptmprl,'YYYYMMDD')<='%s'  and eptmprl is not null group by epcdopr) t_errori on (rpcdopr=epcdopr) order by rpcdopr; ",$szFromDate,$szToDate,$szFromDate,$szToDate);

		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}

		/*
		* controllo presenza in archivio
		*/
		if($res->fetchInto($row)){
		}

		$worksheet->write(2, 0, 'OPERATORE');
		$worksheet->write(2, 1, 'RIGHE');
		$worksheet->write(2, 2, 'PEZZI');
		$worksheet->write(2, 3, 'ERRORI');
		$worksheet->write(2, 4, 'PERCENTUALE');

		$nRiga=0;
		do{
			$worksheet->write(3+$nRiga, 0,$row[0]);
			$worksheet->write(3+$nRiga, 1,$row[1]);
			$worksheet->write(3+$nRiga, 2,$row[2]);
			$worksheet->write(3+$nRiga, 3,$row[3]);
			$worksheet->write(3+$nRiga, 4,$row[4]);

			$nRiga++;
		} while($res->fetchInto($row));

		$res->free();
	}
	// We still need to explicitly close the workbook
	$workbook->close();
?>
