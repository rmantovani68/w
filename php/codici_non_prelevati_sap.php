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
	<td bgcolor=#6090b0 align=center><Font color=white size=5><B>LISTA CODICI NON PRELEVATI - SAP</B></td>
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

	$CB_DA_ANNO=$_GET['CB_DA_ANNO'];
	$CB_DA_MESE=$_GET['CB_DA_MESE'];
	$CB_DA_GIORNO=$_GET['CB_DA_GIORNO'];
	$CB_A_ANNO=$_GET['CB_A_ANNO'];
	$CB_A_MESE=$_GET['CB_A_MESE'];
	$CB_A_GIORNO=$_GET['CB_A_GIORNO'];

	if($CB_DA_ANNO=="" && $CD_DA_MESE=="" && $CD_DA_GIORNO=="" &&
	   $CB_A_ANNO=="" && $CD_A_MESE=="" && $CD_A_GIORNO==""){
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
	printf("<div align=\"center\"><center> <form method=\"get\" action=\"codici_non_prelevati_sap.php\"> <table border=\"0\" cellpadding=\"5\" cellspacing=\"5\">");
	printf("<tr> <td><strong>DAL:</strong></td> <td align=\"center\"><strong>ANNO</strong></td> <td>");
	printf("<p><select name=\"CB_DA_ANNO\" size=\"1\">");
	for($nIndex=2000;$nIndex<2025;$nIndex++){
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
	for($nIndex=2000;$nIndex<2025;$nIndex++){
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
	printf("<td><strong></strong> <p><input type=\"submit\" value=\"ESEGUI STATISTICA\"></p>");
	printf("</td> <td>&nbsp;</td> <td><strong></strong>&nbsp;</td> <td>&nbsp;</td> </tr>");
	printf("</table></form> </center></div>");
	/* fine definizione tabella */

	$szFromDate = sprintf("%04d%02d%02d",$da_anno,$da_mese,$da_giorno);
	$szToDate   = sprintf("%04d%02d%02d",$a_anno,$a_mese,$a_giorno);

		

	if($CB_DA_ANNO>0){
		printf("<Font size=\"5\"><B>LISTA CODICI NON PRELEVATI DAL %s-%s-%s AL %s-%s-%s</B>",$da_giorno,$da_mese,$da_anno,$a_giorno,$a_mese,$a_anno);

		/*
		* rm 22-04-2003 : lista prodotti non prelevati da data a data
		*/
		$szBuffer=sprintf("select u.ubicazione,u.codprod,a.prdescr,u.settore,u.isola from ubicazioni u,articoli a where u.codprod=a.prcdpro and u.codprod not in (select rpcdpro from rig_stor where rpdtpre>='%s' and rpdtpre<='%s' group by rpcdpro) order by u.isola,u.settore,u.ubicazione;", $szFromDate,$szToDate);

		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}

		/*
		*/
		if($res->fetchInto($row)){}
		$settore=$row[3];
		$isola=$row[4];
		$old_settore=0;
		$old_isola=-1;
		printf("<br><br><table BORDER=1 COLS=3 WIDTH=\"70%%\" BGCOLOR=\"#999999\"><tr><th align=left>UBICAZIONE</th><th align=left>CODICE</th><th align=left>DESCRIZIONE</th></tr>");
		do{
			if($old_isola!=$isola){
				printf("<tr>Isola numero %d</tr>",$isola);
				$old_isola=$isola;
			}
			if($old_settore!=$settore){
				printf("<tr>Settore numero %d</tr>",$settore);
				$old_settore=$settore;
			}
			printf("<tr><td>%s</td><td>%s</td><td>%s</td></tr>",$row[0],$row[1],$row[2]);
			$settore=$row[3];
			$isola=$row[4];
		} while($res->fetchInto($row));
		printf("</table><br>");
		$res->free();
	}
?>

<p><h3 align="center"><a href="index.html">Menu Principale</p>

</body>
</html>