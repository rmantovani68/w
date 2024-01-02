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
	<td bgcolor=#608080 align=center><Font color=white size=5><B>RICERCA LETTURE BARCODE IDENTIFICATIVI - SAP</B></td>
</table>

<?php
	require_once 'DB.php';
	$db =& DB::connect('pgsql://roberto:3zin@database/sap');
	if (PEAR::isError($db)) {
    die($db->getMessage());
	}




	printf("<FORM method=\"get\" action=\"letture_barcode_id_sap.php\">");
	printf("<table border=\"0\"><tr><th>BARCODE ID : </th><th><input type=\"text\" name=\"TXT_BARCODE_ID\" size=\"15\" value=\"%s\">",$TXT_BARCODE_ID);
	printf("</input></th> <tr> <th>PRODOTTO :</th> <th><input type = \"text\" name=\"TXT_PRODOTTO\" size=\"15\" value=\"%s\">",$TXT_PRODOTTO);
	printf("</input></th> </tr> </table>");
	printf("<INPUT type=\"submit\" value=\"RICERCA\"> </form>");

	$TXT_PRODOTTO=$_GET['TXT_PRODOTTO'];
	$TXT_BARCODE_ID=$_GET['TXT_BARCODE_ID'];


	$szBuffer = sprintf("select lbprbri,lbcdbri,lbcdpro,lbdstit,ordkey,lbnmcol,lbnmcpe,to_char(lbtmlet,'YYYY-MM-DD HH24:MI') from letture_barcode_id_prodotto where lbcdpro~*'^%s' and lbcdbri~*'^%s';",$TXT_PRODOTTO,$TXT_BARCODE_ID);
	printf("query: %s<br>",$szBuffer);
	$res =& $db->query($szBuffer);
	if (PEAR::isError($res)) {
		die($res->getMessage());
	}


	printf("<table BORDER=1 COLS=8 WIDTH=\"80%%\" BGCOLOR=\"#4499aa\" >");
	printf("<tr><th align=left>PROG. LETT.<th align=left>BARCODE_ID<th align=left>PRODOTTO<th align=left>TITOLO<th align=left>ORDINE<th align=left>COLLO<th align=left>COPIE<th align=left>DATA<tr>");
	while ($res->fetchInto($row)) {
//		printf("DENTRO");
//		printf("%d<br>",$row[0]);

		printf("<tr><td>%d<td>%s<td>%s<td>%s<td>%s<td>%d<td>%d<td>%s<tr>",$row[0],$row[1],$row[2],$row[3],$row[4],$row[5],$row[6],$row[7]);
//		printf("<tr><td>%s<td>%s<td>%d<tr>",$row[0],$row[1],$row[2]);
	}
	$res->free();
	printf("</table>");

?>

</center>
</center>

<p><h3 align="center"><a href="index.html">Menu Principale</p>

</body>
</html>
