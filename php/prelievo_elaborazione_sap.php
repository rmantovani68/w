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
	<td bgcolor=#608080 align=center><Font color=white size=5><B>RICERCA  PRESE  RESIDUE  DI  UN  CODICE - SAP</B></td>
</table>

<?php
	require_once 'DB.php';
	$db =& DB::connect('pgsql://roberto:3zin@database/sap');
	if (PEAR::isError($db)) {
    die($db->getMessage());
	}

	printf("<FORM method=\"get\" action=\"prelievo_elaborazione_sap.php\">");
	printf("<table border=\"0\"><tr><th>ELABORAZIONE : </th><th><input type=\"text\" name=\"TXT_ELAB\" size=\"15\" value=\"%s\">",$TXT_ELAB);
	printf("</input></th> <tr> <th>PRODOTTO :</th> <th><input type = \"text\" name=\"TXT_PRODOTTO\" size=\"15\" value=\"%s\">",$TXT_PRODOTTO);
	printf("</input></th> </tr> </table>");
	printf("<INPUT type=\"submit\" value=\"RICERCA\"> </form>");

	$TXT_PRODOTTO=$_GET['TXT_PRODOTTO'];
	$TXT_ELAB=$_GET['TXT_ELAB'];

	$szBuffer = sprintf("select prdescr from articoli where prcdpro='%s';",$TXT_PRODOTTO);
//	printf("query: %s<br>",$szBuffer);
	$res =& $db->query($szBuffer);
	if (PEAR::isError($res)) {
		die($res->getMessage());
	}
	if($res->fetchInto($row)){
		$szTitolo=$row[0];
//		printf("titolo: %s<br>",$szTitolo);
	}
	$res->free();

	$szBuffer = sprintf("select distinct ordprog from ord_prod where opnelab='%s';",$TXT_ELAB);
//	printf("query: %s<br>",$szBuffer);
	$res =& $db->query($szBuffer);
	if (PEAR::isError($res)) {
		die($res->getMessage());
	}

	$nQtyProd['A']=0;
	$nQtyProd['P']=0;
	$nQtyProd['E']=0;
	$nQtyProd['H']=0;
	$nQtyTotale=0;
	$nOrdini=0;

	while ($res->fetchInto($row)) {
//		printf("Ordprog: %s<br>",$row[0]);
		$szBuffer = sprintf("select sum(rpqtspe),count(rpqtspe), rpstato  from rig_prod where ordprog='%s' and rpcdpro='%s' group by rpstato;",$row[0],$TXT_PRODOTTO);
//		printf("query: %s<br>",$szBuffer);
		$res1 =& $db->query($szBuffer);
		if (PEAR::isError($res1)) {
			die($res1->getMessage());
		}
		$nNumero=0;
		while ($res1->fetchInto($row1)) {
			$nQtyProd[$row1[2]]+=$row1[0];
			$nQtyTotale+=$row1[0];
			if($row1[1]) $nNumero=1;
		}
		$nOrdini+=$nNumero;
		$res1->free();
	}

	$res->free();

	if( (strlen($TXT_ELAB)!=0) || (strlen($TXT_PRODOTTO)!=0) ){
		printf("<p><table border=\"2\" width=\"70%%\" cols=\"2\" nosave>");
		printf("<tr><td BGCOLOR=\"#4499aa\">ELABORAZIONE</td><td>%s</td></tr>",($TXT_ELAB=="")?("&nbsp"):($TXT_ELAB));
		printf("<tr><td BGCOLOR=\"#4499aa\">PRODOTTO</td><td>%s</td></tr>",($szTitolo=="")?("&nbsp"):($szTitolo));
		printf("<tr><td BGCOLOR=\"#4499aa\">ORDINI TOTALI</td><td>%d</td></tr>",$nOrdini);
		printf("<tr><td BGCOLOR=\"#4499aa\">QUANTITA' TOTALE</td><td>%d</td></tr>",$nQtyTotale);
		printf("<tr><td BGCOLOR=\"#4499aa\">QUANTITA' DA PRELEVARE</td><td>%d</td></tr>",$nQtyProd['A']);
		printf("<tr><td BGCOLOR=\"#4499aa\">QUANTITA' PRELEVATA</td><td>%d</td></tr>",$nQtyProd['P']);
		printf("<tr><td BGCOLOR=\"#4499aa\">QUANTITA' EVASA</td><td>%d</td></tr>",$nQtyProd['E']+$nQtyProd['H']);
		printf("</table>");
	}
?>

</center>
</center>

<p><h3 align="center"><a href="index.html">Menu Principale</p>

</body>
</html>
