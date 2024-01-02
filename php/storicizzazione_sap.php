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
	<td bgcolor=#6090b0 align=center><Font color=white size=5><B>MODULO DI CANCELLAZIONE DATI DI STORICO MENSILE - SAP</B></td>
</center>
</table>


<?php

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

	printf("<center><FORM method=\"get\" action=\"storicizzazione_sap.php\">");
	printf("<table><th><h3>ANNO</h3><th align=\"right\"><select name=\"CB_ANNO\" size=\"1\">");
	for($nIndex=2000;$nIndex<2025;$nIndex++){
		if($year-1==$nIndex){
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
	printf("</select></th><tr><td></td><th align=\"right\"><INPUT type=\"submit\" value=\"ESEGUI CANCELLAZIONE\"></th></form></table></center>");

	if($CB_ANNO>0){
		require_once 'DB.php';
		$db =& DB::connect('pgsql://roberto:3zin@database/sap');
		if (PEAR::isError($db)) {
			die($db->getMessage());
		}

		$szBuffer = sprintf("begin work;");
		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}
		$res->free();

		$szBuffer = sprintf("delete from ord_stor where substr(opdteva,5,2)='%02d' and substr(opdteva,1,4)='%04d';",$month,$year);
		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}
		$res->free();

		$szBuffer = sprintf("delete from col_stor where substr(cpdteva,5,2)='%02d' and substr(cpdteva,1,4)='%04d';",$month,$year);
		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}
		$res->free();

		$szBuffer = sprintf("delete from rig_stor where substr(rpdtpre,5,2)='%02d' and substr(rpdtpre,1,4)='%04d';",$month,$year);
		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}
		$res->free();

		$szBuffer = sprintf("delete from ric_stor where substr(rodteva,5,2)='%02d' and substr(rodteva,1,4)='%04d';",$month,$year);
		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}
		$res->free();

		$szBuffer = sprintf("commit work;");
		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}
		$res->free();


		printf("<h1 align=\"center\"> <Font size=\"5\" color=\"#ee0055\"><B> CANCELLAZIONE DATI DI %s - %d TERMINATA</B>",$mesi[$month-1],$year);
		$db->disconnect();
	}

?>

<p><h3 align="center"><a href="index.html">Menu Principale</p>

</body>
</html>
