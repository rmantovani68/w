<!doctype html public "-//w3c//dtd html 4.0 transitional//en">
<html>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
	<meta name="GENERATOR" content="Microsoft FrontPage Express 2.0">
	<title>A&L srl - Wella Italia - Impianto Paperless</title>
</head>

<body bgcolor="#C0C0C0">

<table border="0" width="100%" cols="3" nosave>
    <tr>
        <td nosave><img src="cni-ril-titolo.gif" height=80 width=171></td>
				<td><h1>WELLA ITALIA</h1><p><h2>Impianto Paperless</h2><p>Castiglione delle Stiviere (MN)<td>
        <td><img src="logo_wella.gif" align="right"></td>
		</tr>
</table>

<p>

<table>
<center>
	<table border=1 width=100%>
	<td bgcolor=#6090b0 align=center><Font color=white size=5><B>STATISTICA DI PRODUTTIVITA` MENSILE - SAP</B></td>
</center>
</table>


<?php

	require_once 'DB.php';
	$db =& DB::connect('pgsql://roberto:3zin@database/sap');
	if (PEAR::isError($db)) {
    die($db->getMessage());
	}

	date_default_timezone_set('Europe/Rome');

	$mesi[]="GENNAIO";
	$mesi[]="FEBBRAIO";
	$mesi[]="MARZO";
	$mesi[]="APRILE";
	$mesi[]="MAGGIO";
	$mesi[]="GIUGNO";
	$mesi[]="LUGLIO";
	$mesi[]="AGOSTO";
	$mesi[]="SETTEMBRE";
	$mesi[]="OTTOBRE";
	$mesi[]="NOVEMBRE";
	$mesi[]="DICEMBRE";

	$CB_MESE=$_GET['CB_MESE'];
	$CB_ANNO=$_GET['CB_ANNO'];

	if($CB_MESE=="" || $CB_ANNO==""){
		/*
		* Ricavo la data odierna
		*/
		$date = getdate();

		$month = date("m");
		$year = date("Y");
	} else {
		$month=$CB_MESE;
		$year=$CB_ANNO;
	}

	printf("<center><FORM method=\"get\" action=\"prod_impianto_sap.php\">");
	printf("<table><th><h3>ANNO</h3><th align=\"right\"><select name=\"CB_ANNO\" size=\"1\">");
	for($nIndex=2015;$nIndex<2025;$nIndex++){
		if($year==$nIndex){
			printf("<option selected value=$nIndex>$nIndex</option>");
		} else {
			printf("<option value=$nIndex>$nIndex</option>");
		}
	}
	printf("</select><tr><th><h3>MESE</h3></th><th align=\"right\"><select name=\"CB_MESE\" size=\"1\">");
	for($nIndex=0;$nIndex<=11;$nIndex++){
		if($month==$nIndex+1){
			printf("<option selected value=%d>%s</option>",$nIndex+1,$mesi[$nIndex]);
		} else {
			printf("<option value=%d>%s</option>",$nIndex+1,$mesi[$nIndex]);
		}
	}
	printf("</select></th><tr><td></td><th align=\"right\"><INPUT type=\"submit\" value=\"ESEGUI STATISTICA\"></th></form></table></center>");

	if($CB_ANNO>0){
		/********************************************************************************
		* Calcolo del tempo di funzionamento dell'impianto
		********************************************************************************/
		$szFromDate = sprintf("%04d%02d01",$year,$month);
		$szToDate   = sprintf("%04d%02d31",$year,$month);

		$TempoStart=0;

		$szBuffer = sprintf("select evento,date_part('epoch',data::timestamp)::int from eventi where date_part('year',data::timestamp)=%d and date_part('month',data::timestamp)=%d",$year,$month);
		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}
		$TempoOperativo=0;
		$TempoStart=0;
		while ($res->fetchInto($row)) {
			$evento=trim($row[0]);
			$epoch=trim($row[1]);
//			printf("<br>");
//			printf("TS: %s<br>",$TempoStart);
//			printf("row: |%s| |%s|<br>",$evento,$epoch);
			if(!strcmp($evento,"START")){
//				printf("START<br>");
				if($TempoStart==0){
					$TempoStart=$epoch;
				} else {
					$TempoOperativo+=($epoch-$TempoStart);
					$TempoStart=$epoch;
				}
			}
			if(!strcmp($evento,"STOP")){
//				printf("STOP<br>");
				if($TempoStart==0){
				} else {
					$TempoOperativo+=($epoch-$TempoStart);
					$TempoStart=0;
				}

			}
//			printf("TO: %s<br>",$TempoOperativo);
		}
		$res->free();
		printf("<Font size=\"5\"><P><B>TEMPO OPERATIVO MESE : %s  %02d:%02d:%02d</B>",$mesi[$month-1], $TempoOperativo/3600, ($TempoOperativo%3600)/60, $TempoOperativo%60);


		/********************************************************************************
		* Calcolo del tempo di funzionamento dell'impianto
		********************************************************************************/
		$szBuffer = sprintf("select distinct(ric_stor.ronelab), count(ric_stor.ronelab), sum(ric_stor.ropspre), sum(ric_stor.ropsrea) from ric_stor where (ric_stor.rodteva<='%s' and ric_stor.rodteva>='%s') group by ric_stor.ronelab;",$szToDate,$szFromDate);

		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}

		$nElab=$res->numRows();

		$nTotaleSpedizioni=0;
		$nTotalePSPre=0;
		$nTotalePSRea=0;
										
		while ($res->fetchInto($row)) {
				$nTotaleSpedizioni+=$row[1];
				$nTotalePSPre+=$row[2];
				$nTotalePSRea+=$row[3];
		}
		$res->free();

		if($year>0){ 
			printf("<Font size=\"5\"><P><B>PRELIEVI DEL MESE : %s  %4d</B>",$mesi[$month-1],$year);
		}

		if($nTotaleSpedizioni!=0){

			printf("<table COLS=2 WIDTH=\"40%%\">");
			printf("<tr><th align=\"left\">ELABORAZIONI EVASE<th align=\"left\">: %d<tr>",$nElab);
			printf("<th align=\"left\">ORDINI EVASI<th align=\"left\">: %d<tr>",$nTotaleSpedizioni);
			printf("<th align=\"left\">PESO CALCOLATO ORDINI (kg.)<th align=\"left\">: %15.3f<tr>",(float)((float)($nTotalePSPre)/(float)1000));
			printf("<th align=\"left\">PESO RILEVATO ORDINI (kg.)<th align=\"left\">: %15.3f<tr>",(float)((float)($nTotalePSRea)/(float)1000));
			printf("</table>");

			$szBuffer = sprintf("select distinct(ord_stor.opcdisl),count(ord_stor.opcdisl),sum(ord_stor.opnmcll),sum(ord_stor.opnmrgh),sum(ord_stor.opnmcpe) from ord_stor where (ord_stor.opdteva<='%s' and ord_stor.opdteva>='%s') group by ord_stor.opcdisl;",$szToDate,$szFromDate);
			$res =& $db->query($szBuffer);
			if (PEAR::isError($res)) {
				die($res->getMessage());
			}
			$nIsole=$res->numRows();
			printf("<h2 align=left>DATI SUDDIVISI PER ISOLA</h2>");
			printf("<table BORDER=1 COLS=5 WIDTH=\"50%%\" BGCOLOR=\"#4499aa\" >");
			printf("<tr><th align=\"left\">ISOLA<th align=\"left\">SOTTORDINI<th align=\"left\">COLLI<th align=\"left\">LINEE<th align=\"left\">PRESE<tr>");

			$nTotSottordini=0;
			$nTotColli=0;
			$nTotLinee=0;
			$nTotPrese=0;

			while ($res->fetchInto($row)) {
				printf("<tr><td>%d<td>%d<td>%d<td>%d<td>%d<tr>",$row[0],$row[1],$row[2],$row[3],$row[4]+$row[2]);
				$nTotSottordini+=$row[1];
				$nTotColli+=$row[2];
				$nTotLinee+=$row[3];
				$nTotPrese+=$row[4]+$row[2];
				$nColli[$nIndex]=$row[2];
			}
			$res->free();
			printf("<tr><th align=\"left\">%s<td>%d<td>%d<td>%d<td>%d<tr>",TOTALI,$nTotSottordini,$nTotColli,$nTotLinee,$nTotPrese);
			printf("</table>");

			for($nIsola=1;$nIsola<=2;$nIsola++){
				printf("<h2 align=left>PRESE EFFETTUATE SU ISOLA%d</h2>",$nIsola);
				$szBuffer = sprintf("select distinct(substr(rig_stor.rpcdubi,2,2)),sum(rig_stor.rpqtspe) from rig_stor,ord_stor where rig_stor.ordkey=ord_stor.ordkey and ord_stor.opcdisl=%d and (ord_stor.opdteva<='%s' and ord_stor.opdteva>='%s') group by substr(rig_stor.rpcdubi,2,2);",$nIsola,$szToDate,$szFromDate );
				$res =& $db->query($szBuffer);
				if (PEAR::isError($res)) {
					die($res->getMessage());
				}
				$nSettori=$res->numRows();
				printf("<table BORDER=\"1\" COLS=\"2\" WIDTH=\"40%%\" BGCOLOR=\"#44aa66\" >");
				for($nIndex=0;$nIndex<$nSettori;$nIndex++){
					if($res->fetchInto($row)){
						printf("<tr><td>SETTORE %d<td>%d<tr>",$nIndex+1,($nIndex==0)?($row[1]+$nColli[$nIsola-1]):($row[1]));
					}
				}
				printf("</table>");
				$res->free();
				/*
				* FORMATI UTILIZZATI PER ISOLA
				*/
				$szBuffer=sprintf("select distinct cptpfor, count(cptpfor) from col_stor where cpdteva>='%s' and cpdteva<='%s' and substr(ordkey,12,1)='%d' group by cptpfor;",$szFromDate,$szToDate,$nIsola);
				$res =& $db->query($szBuffer);
				if (PEAR::isError($res)) {
					die($res->getMessage());
				}
				$nImballi=0;
				if($res->fetchInto($row)){
					printf("<br><h3>IMBALLI UTILIZZATI IN ISOLA %d</h3>",$nIsola);
					printf("<table BORDER=1 COLS=2 WIDTH=\"40%%\" BGCOLOR=\"#44AA66\" >");
					printf("<tr><th align=left>FORMATO</td>");
					printf("<th align=left>QUANTITA'</td><tr>");
					do {
						printf("<tr><th align=left>%s<td>%d<tr>",$row[0],$row[1]);
						$nImballi+=$row[1];
					} while($res->fetchInto($row));
					printf("<tr><th align=left>TOTALE<th align=left>%d",$nImballi);
					printf("</table>");
				}
				$res->free();
			}
		} else {
			printf("<Font size=\"5\"><B> NON DISPONIBILI</B>");
		}
	}

	$db->disconnect();
?>

<p><h3 align="center"><a href="index.html">Menu Principale</p>

</body>
</html>
