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

<center>

<table>
	<table border=1 width=100%>
	<td bgcolor=#608080 align=center><Font color=white size=5><B>RICERCA ACCESSI OPERATORE - SAP</B></td>
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
	$TXT_OPERATORE=$_GET['TXT_OPERATORE'];

	$operatore=$TXT_OPERATORE;
	if($CB_DA_ANNO=="" && $CB_DA_MESE=="" && $CB_DA_GIORNO=="" &&
	   $CB_A_ANNO=="" && $CB_A_MESE=="" && $CB_A_GIORNO=="" 
		 ){
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



	printf("<FORM method=\"get\" action=\"accessi_operatore_sap.php\">");
	printf("<table border=\"0\">");
	printf("<tr> <th>OPERATORE :</th> <th><input type = \"text\" name=\"TXT_OPERATORE\" size=\"15\" value=\"%s\"></input></th> </tr> ",$TXT_OPERATORE);


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




	printf("</select></p> </td></tr></table>");
	printf("<INPUT type=\"submit\" value=\"RICERCA\"> </form>");


	/* form per pulsante di conversione excel */
	printf("<div align=\"center\"><center> <form method=\"GET\" action=\"accessi_operatore_excel.php\">");
	printf("<table><tr><td><strong></strong> <p><input type=\"submit\" value=\"EXCEL\">");

	printf("<input type=\"hidden\" name=\"TXT_OPERATORE\" value=\"%s\">",   $operatore    );
	printf("<input type=\"hidden\" name=\"CB_DA_ANNO\" value=\"%s\">",      $da_anno      );
	printf("<input type=\"hidden\" name=\"CB_DA_MESE\" value=\"%s\">",      $da_mese      );
	printf("<input type=\"hidden\" name=\"CB_DA_GIORNO\" value=\"%s\">",    $da_giorno    );
	printf("<input type=\"hidden\" name=\"CB_A_ANNO\" value=\"%s\">",       $a_anno       );
	printf("<input type=\"hidden\" name=\"CB_A_MESE\" value=\"%s\">",       $a_mese       );
	printf("<input type=\"hidden\" name=\"CB_A_GIORNO\" value=\"%s\">",     $a_giorno     );
	printf("</p></td></tr></table></form></center></div>");

	$szFromDate = sprintf("%04d%02d%02d",$da_anno,$da_mese,$da_giorno);
	$szToDate   = sprintf("%04d%02d%02d",$a_anno,$a_mese,$a_giorno);



	if($CB_DA_ANNO>0){

		$szBuffer = sprintf("select to_char(read_time,'YYYY-MM-DD HH24:MI'),isola_id,settore_id,stato_lettura  from badge_registry where operatore_id='%s' and to_char(read_time,'YYYYMMDD')>='%s' and to_char(read_time,'YYYYMMDD')<='%s' order by read_time;",$operatore,$szFromDate,$szToDate);
		// printf("query: %s<br>",$szBuffer);
		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}


		printf("<table BORDER=1 COLS=8 WIDTH=\"80%%\" BGCOLOR=\"#4499aa\" >");
		printf("<tr><th align=left>DATA/ORA LETTURA<th align=left>ISOLA<th align=left>SETTORE<th align=left>STATO LETTURA<tr>");
		while ($res->fetchInto($row)) {
			printf("<tr><td>%s<td>%s<td>%s<td>%s<tr>",$row[0],$row[1],$row[2],$row[3]);
		}
		$res->free();
		printf("</table>");
	}

?>

</center>
</center>

<p><h3 align="center"><a href="index.html">Menu Principale</p>

</body>
</html>
