--<!doctype html public "-//w3c//dtd html 4.0 transitional//en">
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
	<td bgcolor=#6090b0 align=center><Font color=white size=5><B>STATISTICA ERRORI DI PRELIEVO MENSILE - SAP</B></td>
</center>
</table>


<?php
	/*
	* Sta roba e' da eliminare, serve solo qui in AeL
	*/
	//require("db_pgsql.inc");  
	//require("table.inc");    
	/*
	*/
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

	printf("<center><FORM method=\"get\" action=\"errori_mensile.php3\">");
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

		$nTotErrori=0;
		$szBuffer = sprintf("select epcdopr,count(*) as errori from errori_prelievo where date_part('year',eptmprl)='%s' and date_part('month',eptmprl)='%s' and eptmprl is not null group by epcdopr order by errori desc;",$year,$month);
		$db -> query($szBuffer);
		$nRighe=$db->num_rows();
		printf("<h2 align=left>Errori per operatore</h2>");
		printf("<table BORDER=1 COLS=2 WIDTH=\"50%%\" BGCOLOR=\"#4499aa\" >");
		printf("<tr><th align=\"left\">OPERATORE<th align=\"left\">ERRORI<tr>");
		for($nIndex=0;$nIndex<$nRighe;$nIndex++){
			if ($db->next_record()){
				printf("<tr><td>%s<td>%d<tr>",$db->f(0),$db->f(1));
				$nTotErrori+=$db->f(1);
			}
		}
		printf("<tr><th align=\"left\">%s<td>%d<tr>",TOTALI,$nTotErrori);
		printf("</table>");
		$db->free();

		$nTotErrori=0;
		$szBuffer = sprintf("select epcderr,count(*) as errori from errori_prelievo where date_part('year',eptmprl)='%s' and date_part('month',eptmprl)='%s' and eptmprl is not null group by epcderr order by errori desc;",$year,$month);
		$db -> query($szBuffer);
		$nRighe=$db->num_rows();
		printf("<h2 align=left>Errori per tipo</h2>");
		printf("<table BORDER=1 COLS=2 WIDTH=\"50%%\" BGCOLOR=\"#4499aa\" >");
		printf("<tr><th align=\"left\">TIPO ERRORE<th align=\"left\">ERRORI<tr>");
		for($nIndex=0;$nIndex<$nRighe;$nIndex++){
			if ($db->next_record()){
				printf("<tr><td>%s<td>%d<tr>",$db->f(0),$db->f(1));
				$nTotErrori+=$db->f(1);
			}
		}
		printf("<tr><th align=\"left\">%s<td>%d<tr>",TOTALI,$nTotErrori);
		printf("</table>");
		$db->free();

		$nTotErrori=0;
		$szBuffer = sprintf("select epcdopr,epcderr,count(*) as errori from errori_prelievo where date_part('year',eptmprl)='%s' and date_part('month',eptmprl)='%s' and eptmprl is not null group by epcdopr,epcderr order by epcdopr,epcderr;",$year,$month);
		$db -> query($szBuffer);
		$nRighe=$db->num_rows();
		printf("<h2 align=left>Dettaglio Errori per operatore</h2>");
		printf("<table BORDER=1 COLS=3 WIDTH=\"50%%\" BGCOLOR=\"#4499aa\" >");
		printf("<tr><th align=\"left\">OPERATORE<th align=\"left\">TIPO ERRORE<th align=\"left\">ERRORI<tr>");
		for($nIndex=0;$nIndex<$nRighe;$nIndex++){
			if ($db->next_record()){
				printf("<tr><td>%s<td>%s<td>%d<tr>",$db->f(0),$db->f(1),$db->f(2));
				$nTotErrori+=$db->f(2);
			}
		}
		printf("<tr><th align=\"left\">%s<td>%d<tr>",TOTALI,$nTotErrori);
		printf("</table>");
		$db->free();

	}

?>

<p><h3 align="center"><a href="index.html">Menu Principale</p>

</body>
</html>
