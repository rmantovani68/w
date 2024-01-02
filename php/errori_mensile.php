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
	<td bgcolor=#6090b0 align=center><Font color=white size=5><B>STATISTICA ERRORI DI PRELIEVO MENSILE - SAP</B></td>
</center>
</table>


<?php
	require_once 'DB.php';
	require_once 'Spreadsheet/Excel/Writer.php';

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

	$CB_DA_ANNO   =$_GET['CB_DA_ANNO'];
	$CB_DA_MESE   =$_GET['CB_DA_MESE'];
	$CB_DA_GIORNO =$_GET['CB_DA_GIORNO'];
	$CB_A_ANNO    =$_GET['CB_A_ANNO'];
	$CB_A_MESE    =$_GET['CB_A_MESE'];
	$CB_A_GIORNO  =$_GET['CB_A_GIORNO'];

	if($CB_DA_ANNO=="" && $CB_DA_MESE=="" && $CB_DA_GIORNO=="" &&
	   $CB_A_ANNO=="" && $CB_A_MESE=="" && $CB_A_GIORNO==""){
		/*
		* Ricavo la data odierna
		*/
		// $date = getdate();

		$da_anno=date("Y");
		$da_mese=date("m");
		$da_giorno=date("d");
		$a_anno=date("Y");
		$a_mese=date("m");
		$a_giorno=date("d");
	} else {
		$da_anno=$CB_DA_ANNO;
		$da_mese=$CB_DA_MESE;
		$da_giorno=$CB_DA_GIORNO;
		$a_anno=$CB_A_ANNO;
		$a_mese=$CB_A_MESE;
		$a_giorno=$CB_A_GIORNO;
	}


	/* definizione tabella di inserimento date */
	printf("<div align=\"center\"><center> <form method=\"get\" action=\"errori_mensile.php\"> <table border=\"0\" cellpadding=\"5\" cellspacing=\"5\">");
	// printf("<div align=\"center\"><center> <form method=\"get\" action=\"excel.php\"> <table border=\"0\" cellpadding=\"5\" cellspacing=\"5\">");
	printf("<tr> <td><strong>DAL:</strong></td> <td align=\"center\"><strong>ANNO</strong></td> <td>");
	printf("<p><select name=\"CB_DA_ANNO\" size=\"1\">");
	for($nIndex=2015;$nIndex<2025;$nIndex++){
		if($da_anno==$nIndex){
			printf("<option selected value=$nIndex>$nIndex</option>");
		} else {
			printf("<option value=$nIndex>$nIndex</option>");
		}
	}
	printf("</select></p> </td> <td align=\"center\"><strong>MESE</strong></td> <td>");
	printf("<p><select name=\"CB_DA_MESE\" size=\"1\">");
	for($nIndex=0;$nIndex<=11;$nIndex++){
		if($da_mese==$nIndex+1){
			printf("<option selected value=%d>%s</option>",$nIndex+1,$mesi[$nIndex]);
		} else {
			printf("<option value=%d>%s</option>",$nIndex+1,$mesi[$nIndex]);
		}
	}
	printf("</select></p> </td> <td><strong>GIORNO</strong></td> <td>");
	printf("<p><select name=\"CB_DA_GIORNO\" size=\"1\">");
	for($nIndex=0;$nIndex<=30;$nIndex++){
		if($da_giorno==$nIndex+1){
			printf("<option selected value=%d>%s</option>",$nIndex+1,$nIndex+1);
		} else {
			printf("<option value=%d>%s</option>",$nIndex+1,$nIndex+1);
		}
	}
	printf("</select></p> </td> </tr> <tr>");
	printf("<td><strong>AL:</strong></td> <td align=\"center\"><strong>ANNO</strong></td> <td>");
	printf("<p><select name=\"CB_A_ANNO\" size=\"1\">");
	for($nIndex=2000;$nIndex<2030;$nIndex++){
		if($a_anno==$nIndex){
			printf("<option selected value=$nIndex>$nIndex</option>");
		} else {
			printf("<option value=$nIndex>$nIndex</option>");
		}
	}
	printf("</select></p> </td> <td align=\"center\"><strong>MESE</strong></td> <td>");
	printf("<p><select name=\"CB_A_MESE\" size=\"1\">");
	for($nIndex=0;$nIndex<=11;$nIndex++){
		if($a_mese==$nIndex+1){
			printf("<option selected value=%d>%s</option>",$nIndex+1,$mesi[$nIndex]);
		} else {
			printf("<option value=%d>%s</option>",$nIndex+1,$mesi[$nIndex]);
		}
	}
	printf("</select></p> </td> <td><strong>GIORNO</strong></td> <td>");
	printf("<p><select name=\"CB_A_GIORNO\" size=\"1\">");
	for($nIndex=0;$nIndex<=30;$nIndex++){
		if($a_giorno==$nIndex+1){
			printf("<option selected value=%d>%s</option>",$nIndex+1,$nIndex+1);
		} else {
			printf("<option value=%d>%s</option>",$nIndex+1,$nIndex+1);
		}
	}
	printf("</select></p> </td> </tr> <tr>");
	printf("<td><strong></strong>&nbsp;</td> <td align=\"right\"><strong></strong>&nbsp;</td> <td>&nbsp;</td>");
	printf("<td><strong></strong> <p><input type=\"submit\" value=\"ESEGUI STATISTICA\"></p></td>");
	printf("<td></td> <td>&nbsp;</td> <td>&nbsp;</td> </tr>");
	printf("</table></form> </center></div>");

	/* form per pulsante di conversione excel */
	/*
	printf("<div align=\"center\"><center> <form method=\"GET\" action=\"errori_mensile_excel.php\">");
	printf("<table><tr><td><strong></strong> <p><input type=\"submit\" value=\"EXCEL\">");
	*/

	/*
	printf("</select></p> </td></tr></table>");
	printf("<INPUT type=\"submit\" value=\"ESEGUI STATISTICA\"> </form>");
	*/


	/* form per pulsante di conversione excel */
	/*
	printf("<div align=\"center\"><center> <form method=\"GET\" action=\"errori_mensile_excel.php\">");
	printf("<table><tr><td><strong></strong> <p><input type=\"submit\" value=\"EXCEL\">");
	printf("</p></td></tr></table></form></center></div>");
	*/


	/* form per pulsante di conversione excel */
	printf("<div align=\"center\"><center> <form method=\"GET\" action=\"errori_mensile_excel.php\">");
	printf("<table><tr><td><strong></strong> <p><input type=\"submit\" value=\"EXCEL\">");

	printf("<input type=\"hidden\" name=\"CB_DA_ANNO\" value=\"%s\">",      $da_anno      );
	printf("<input type=\"hidden\" name=\"CB_DA_MESE\" value=\"%s\">",      $da_mese      );
	printf("<input type=\"hidden\" name=\"CB_DA_GIORNO\" value=\"%s\">",    $da_giorno    );
	printf("<input type=\"hidden\" name=\"CB_A_ANNO\" value=\"%s\">",       $a_anno       );
	printf("<input type=\"hidden\" name=\"CB_A_MESE\" value=\"%s\">",       $a_mese       );
	printf("<input type=\"hidden\" name=\"CB_A_GIORNO\" value=\"%s\">",     $a_giorno     );
	printf("</p></td></tr></table></form></center></div>");










	if($CB_DA_ANNO>0){
		$szFromDate = sprintf("%04d%02d%02d",$da_anno,$da_mese,$da_giorno);
		$szToDate   = sprintf("%04d%02d%02d",$a_anno,$a_mese,$a_giorno);

		//$szBuffer = sprintf(" select rpcdopr,righe,quantita,coalesce(errori,0) from (select rpcdopr,count(*) as righe,sum(rpqtspe) as quantita from (select * from rig_stor union select * from rig_prod) tt  where date_part('year',rptmprl)='%s' and date_part('month',rptmprl)='%s' and rptmprl is not null group by rpcdopr) t_righe left outer join (select epcdopr,count(*) as errori from errori_prelievo where date_part('year',eptmprl)='%s' and date_part('month',eptmprl)='%s' and eptmprl is not null group by epcdopr) t_errori on (rpcdopr=epcdopr) order by rpcdopr; ",$year,$month,$year,$month);
		$szBuffer = sprintf(" select rpcdopr,righe,quantita,coalesce(errori,0) as errori_operatore, (coalesce(errori,0)*100)/quantita::float from (select rpcdopr,count(*) as righe,sum(rpqtspe) as quantita from (select * from rig_stor union select * from rig_prod) tt  where to_char(rptmprl,'YYYYMMDD')>='%s' and to_char(rptmprl,'YYYYMMDD')<='%s' and rptmprl is not null group by rpcdopr) t_righe left outer join (select epcdopr,count(*) as errori from errori_prelievo where to_char(eptmprl,'YYYYMMDD')>='%s' and to_char(eptmprl,'YYYYMMDD')<='%s'  and eptmprl is not null group by epcdopr) t_errori on (rpcdopr=epcdopr) order by rpcdopr; ",$szFromDate,$szToDate,$szFromDate,$szToDate);
		// printf("%s\n",$szBuffer);
		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}
		$nRighe=$res->numRows();
		$nTotErrori=0;
		//printf("%d\n",$db->num_rows());
		printf("<h2 align=left>Errori per operatore</h2>");
		printf("<table BORDER=1 COLS=4 WIDTH=\"50%%\" BGCOLOR=\"#4499aa\" >");
		printf("<tr><th align=\"left\">OPERATORE<th align=\"left\">RIGHE<th align=\"left\">PEZZI<th align=\"left\">ERRORI<th align=\"left\">PERC<tr>");
		for($nIndex=0;$nIndex<$nRighe;$nIndex++){
			if ($res->fetchInto($row)){
				printf("<tr><td>%s<td>%d<td>%d<td>%d<td>%f<tr>",$row[0],$row[1],$row[2],$row[3],$row[4]);
				$nTotRighe+=$row[1];
				$nTotCopie+=$row[2];
				$nTotErrori+=$row[3];
			}
		}
		printf("<tr><th align=\"left\">%s<td>%d<td>%d<td>%d<td>%f<tr>",TOTALI,$nTotRighe,$nTotCopie,$nTotErrori,(float)(($nTotErrori*100)/(float)$nTotCopie));
		printf("</table>");
		$res->free();


		//$szBuffer = sprintf("select epcderr,count(*) as errori from errori_prelievo where date_part('year',eptmprl)='%s' and date_part('month',eptmprl)='%s' and eptmprl is not null group by epcderr order by errori desc;",$year,$month);
		$szBuffer = sprintf("select epcderr,count(*) as errori from errori_prelievo where to_char(eptmprl,'YYYYMMDD')>='%s' and to_char(eptmprl,'YYYYMMDD')<='%s'   and eptmprl is not null group by epcderr order by errori desc;",$szFromDate,$szToDate);
		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}
		$nRighe=$res->numRows();
		$nTotErrori=0;
		printf("<h2 align=left>Errori per tipo</h2>");
		printf("<table BORDER=1 COLS=2 WIDTH=\"50%%\" BGCOLOR=\"#4499aa\" >");
		printf("<tr><th align=\"left\">TIPO ERRORE<th align=\"left\">ERRORI<tr>");
		for($nIndex=0;$nIndex<$nRighe;$nIndex++){
			if ($res->fetchInto($row)){
				printf("<tr><td>%s<td>%d<tr>",$row[0],$row[1]);
				$nTotErrori+=$row[1];
			}
		}
		printf("<tr><th align=\"left\">%s<td>%d<tr>",TOTALI,$nTotErrori);
		printf("</table>");
		$res->free();


		$szBuffer = sprintf("select epcdopr,epcderr,count(*) as errori from errori_prelievo where to_char(eptmprl,'YYYYMMDD')>='%s' and to_char(eptmprl,'YYYYMMDD')<='%s'  and eptmprl is not null group by epcdopr,epcderr order by epcdopr,epcderr;",$szFromDate,$szToDate);
		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}
		$nRighe=$res->numRows();
		$nTotErrori=0;
		printf("<h2 align=left>Dettaglio Errori per operatore</h2>");
		printf("<table BORDER=1 COLS=3 WIDTH=\"50%%\" BGCOLOR=\"#4499aa\" >");
		printf("<tr><th align=\"left\">OPERATORE<th align=\"left\">TIPO ERRORE<th align=\"left\">ERRORI<tr>");
		for($nIndex=0;$nIndex<$nRighe;$nIndex++){
			if ($res->fetchInto($row)){
				printf("<tr><td>%s<td>%s<td>%d<tr>",$row[0],$row[1],$row[2]);
				$nTotErrori+=$row[2];
			}
		}
		printf("<tr><th align=\"left\">%s<td><td>%d<tr>",TOTALI,$nTotErrori);
		printf("</table>");
		$res->free();


		$szBuffer = sprintf("select epcdopr,epcderr,epcdpro,count(*) as errori from errori_prelievo where to_char(eptmprl,'YYYYMMDD')>='%s' and to_char(eptmprl,'YYYYMMDD')<='%s'  and eptmprl is not null group by epcdopr,epcderr,epcdpro order by epcdopr,epcderr,epcdpro;",$szFromDate,$szToDate);
		//printf("%s\n",$szBuffer);
		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}
		$nRighe=$res->numRows();
		$nTotErrori=0;
		printf("<h2 align=left>Dettaglio Errori per operatore e codice</h2>");
		printf("<table BORDER=1 COLS=4 WIDTH=\"50%%\" BGCOLOR=\"#4499aa\" >");
		printf("<tr><th align=\"left\">OPERATORE<th align=\"left\">TIPO ERRORE<th align=\"left\">PRODOTTO<th align=\"left\">ERRORI<tr>");
		for($nIndex=0;$nIndex<$nRighe;$nIndex++){
			if ($res->fetchInto($row)){
				printf("<tr><td>%s<td>%s<td>%s<td>%d<tr>",$row[0],$row[1],$row[2],$row[3]);
				$nTotErrori+=$row[3];
			}
		}
		printf("<tr><th align=\"left\">%s<td><td><td>%d<tr>","TOTALI",$nTotErrori);
		printf("</table>");
		$res->free();

	}

?>

<p><h3 align="center"><a href="index.html">Menu Principale</p>

</body>
</html>
