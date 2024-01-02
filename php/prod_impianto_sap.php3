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
	$db = new DB_Sql;
	$db -> Host = "database";
	$db -> Password = "3zin";
	$db -> User = "stefano";
	$db -> Database = "sap";
	$db -> Type = "pgsql";
	$db -> Debug = True;
	$db -> Halt_On_Error = "report";

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

	printf("<center><FORM method=\"get\" action=\"prod_impianto_sap.php3\">");
	printf("<table><th><h3>ANNO</h3><th align=\"right\"><select name=\"CB_ANNO\" size=\"1\">");
	for($nIndex=2000;$nIndex<2015;$nIndex++){
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
		$szFromDate = sprintf("%04d%02d01",$year,$month);
		$szToDate   = sprintf("%04d%02d31",$year,$month);

		$TempoStart=0;

		$szBuffer = sprintf("select evento,date_part('epoch',data::timestamp) from eventi where date_part('year',data::timestamp)=%d and date_part('month',data::timestamp)=%d;",$year,$month);
		$db -> query($szBuffer);
		$TempoOperativo=0;
		while($db->next_record()){
			if($db->f(0)=="START"){
				if($TempoStart==0){
					$TempoStart=$db->f(1);
				} else {
					$TempoOperativo+=($db->f(1)-$TempoStart);
					$TempoStart=$db->f(1);
				}
			}
			if($db->f(0)=="STOP"){
				$TempoOperativo+=($db->f(1)-$TempoStart);
				$TempoStart=0;
			}
		}
		printf("<Font size=\"5\"><P><B>TEMPO OPERATIVO MESE : %s  %02d:%02d:%02d</B>",$mesi[$month-1],
			$TempoOperativo/3600,
			($TempoOperativo%3600)/60,
			$TempoOperativo%60);

		$szBuffer = sprintf("select distinct(ric_stor.ronelab), count(ric_stor.ronelab), sum(ric_stor.ropspre), sum(ric_stor.ropsrea) from ric_stor where (ric_stor.rodteva<='%s' and ric_stor.rodteva>='%s') group by ric_stor.ronelab;",$szToDate,$szFromDate);
		$db -> query($szBuffer);

		$nElab=$db->num_rows();

		$nTotaleSpedizioni=0;
		$nTotalePSPre=0;
		$nTotalePSRea=0;
										
		while($db->next_record()){
				$nTotaleSpedizioni+=$db->f(1);
				$nTotalePSPre+=$db->f(2);
				$nTotalePSRea+=$db->f(3);
		}

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

			$db->free();

			$szBuffer = sprintf("select distinct(ord_stor.opcdisl),count(ord_stor.opcdisl),sum(ord_stor.opnmcll),sum(ord_stor.opnmrgh),sum(ord_stor.opnmcpe) from ord_stor where (ord_stor.opdteva<='%s' and ord_stor.opdteva>='%s') group by ord_stor.opcdisl;",$szToDate,$szFromDate);
			$db -> query($szBuffer);
			$nIsole=$db->num_rows();
			printf("<h2 align=left>DATI SUDDIVISI PER ISOLA</h2>");
			printf("<table BORDER=1 COLS=5 WIDTH=\"50%%\" BGCOLOR=\"#4499aa\" >");
			printf("<tr><th align=\"left\">ISOLA<th align=\"left\">SOTTORDINI<th align=\"left\">COLLI<th align=\"left\">LINEE<th align=\"left\">PRESE<tr>");

			$nTotSottordini=0;
			$nTotColli=0;
			$nTotLinee=0;
			$nTotPrese=0;

			for($nIndex=0;$nIndex<$nIsole;$nIndex++){
				if ($db->next_record()){
					printf("<tr><td>%d<td>%d<td>%d<td>%d<td>%d<tr>",$db->f(0),$db->f(1),$db->f(2),$db->f(3),$db->f(4)+$db->f(2));
					$nTotSottordini+=$db->f(1);
					$nTotColli+=$db->f(2);
					$nTotLinee+=$db->f(3);
					$nTotPrese+=$db->f(4)+$db->f(2);
					$nColli[$nIndex]=$db->f(2);
				}
			}
			printf("<tr><th align=\"left\">%s<td>%d<td>%d<td>%d<td>%d<tr>",TOTALI,$nTotSottordini,$nTotColli,$nTotLinee,$nTotPrese);
			printf("</table>");
			$db->free();

			for($nIsola=1;$nIsola<=2;$nIsola++){
				printf("<h2 align=left>PRESE EFFETTUATE SU ISOLA%d</h2>",$nIsola);
				$szBuffer = sprintf("select distinct(substr(rig_stor.rpcdubi,2,2)),sum(rig_stor.rpqtspe) from rig_stor,ord_stor where rig_stor.ordkey=ord_stor.ordkey and ord_stor.opcdisl=%d and (ord_stor.opdteva<='%s' and ord_stor.opdteva>='%s') group by substr(rig_stor.rpcdubi,2,2);",$nIsola,$szToDate,$szFromDate );
				$db -> query($szBuffer);
				$nSettori=$db->num_rows();
				printf("<table BORDER=\"1\" COLS=\"2\" WIDTH=\"40%%\" BGCOLOR=\"#44aa66\" >");
				for($nIndex=0;$nIndex<$nSettori;$nIndex++){
					if($db->next_record()){
						printf("<tr><td>SETTORE %d<td>%d<tr>",$nIndex+1,($nIndex==0)?($db->f(1)+$nColli[$nIsola-1]):($db->f(1)));
					}
				}
				printf("</table>");
				$db->free();
				/*
				* FORMATI UTILIZZATI PER ISOLA
				*/
				$szBuffer=sprintf("select distinct cptpfor, count(cptpfor) from col_stor where cpdteva>='%s' and cpdteva<='%s' and substr(ordkey,12,1)='%d' group by cptpfor;",$szFromDate,$szToDate,$nIsola);
				$db -> query($szBuffer);
				$nImballi=0;
				if($db->next_record()){
					printf("<br><h3>IMBALLI UTILIZZATI IN ISOLA %d</h3>",$nIsola);
					printf("<table BORDER=1 COLS=2 WIDTH=\"40%%\" BGCOLOR=\"#44AA66\" >");
					printf("<tr><th align=left>FORMATO</td>");
					printf("<th align=left>QUANTITA'</td><tr>");
					do {
						printf("<tr><th align=left>%s<td>%d<tr>",$db->f(0),$db->f(1));
						$nImballi+=$db->f(1);
					} while($db->next_record());
					printf("<tr><th align=left>TOTALE<th align=left>%d",$nImballi);
					printf("</table>");
				}
				$db->free();
			}
		} else {
			printf("<Font size=\"5\"><B> NON DISPONIBILI</B>");
		}
	}

?>

<p><h3 align="center"><a href="index.html">Menu Principale</p>

</body>
</html>
