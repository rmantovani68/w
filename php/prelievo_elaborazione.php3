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
	<td bgcolor=#608080 align=center><Font color=white size=5><B>RICERCA  PRESE  RESIDUE  DI  UN  CODICE</B></td>
</table>

<?php
	$db = new DB_Sql;
	$db -> Host = "database";
	$db -> Password = "3zin";
	$db -> User = "roberto";
	$db -> Database = "picking";
	$db -> Type = "pgsql";
	$db -> Debug = True;
	$db -> Halt_On_Error = "report";

	printf("<FORM method=\"get\" action=\"prelievo_elaborazione.php3\">");
	printf("<table border=\"0\"><tr><th>ELABORAZIONE : </th><th><input type=\"text\" name=\"TXT_ELAB\" size=\"15\" value=\"%s\">",$TXT_ELAB);
	printf("</input></th> <tr> <th>PRODOTTO :</th> <th><input type = \"text\" name=\"TXT_PRODOTTO\" size=\"15\" value=\"%s\">",$TXT_PRODOTTO);
	printf("</input></th> </tr> </table>");
	printf("<INPUT type=\"submit\" value=\"RICERCA\"form>");

	$szBuffer = sprintf("select distinct ordprog from ord_prod where opnelab='%06d';",$TXT_ELAB);
	$db -> query($szBuffer);

	$db1 = new DB_Sql;
	$db1 -> Host = "localhost";
	$db1 -> Password = "3zin";
	$db1 -> User = "roberto";
	$db1 -> Database = "picking";
	$db1 -> Type = "pgsql";
	$db1 -> Debug = True;
	$db1 -> Halt_On_Error = "report";

	$szBuffer = sprintf("select prdescr from articoli where prcdpro='%s';",$TXT_PRODOTTO);
	$db1 -> query($szBuffer);
	$db1->next_record();
	$szTitolo=$db1->f(0);
	$db1->free();

	$nQtyProd['A']=0;
	$nQtyProd['P']=0;
	$nQtyProd['E']=0;
	$nQtyTotale=0;
	$nOrdini=0;


	while($db->next_record()){
		$szBuffer = sprintf("select sum(rpqtspe),count(rpqtspe), rpstato  from rig_prod where ordprog='%06d' and rpcdpro='%s' group by rpstato;",$db->f(0),$TXT_PRODOTTO);
		$db1 -> query($szBuffer);
		$nNumero=0;
		while($db1->next_record()){
			$nQtyProd[$db1->f(2)]+=$db1->f(0);
			$nQtyTotale+=$db1->f(0);
			if($db1->f(1)) $nNumero=1;
		}
		$nOrdini+=$nNumero;
		$db1->free();
	}
	$db->free();

	if( (strlen($TXT_ELAB)!=0) || (strlen($TXT_PRODOTTO)!=0) ){
		printf("<p><table border=\"2\" width=\"70%%\" cols=\"2\" nosave>");
		printf("<tr><td BGCOLOR=\"#4499aa\">ELABORAZIONE</td><td>%s</td></tr>",($TXT_ELAB=="")?("&nbsp"):($TXT_ELAB));
		printf("<tr><td BGCOLOR=\"#4499aa\">PRODOTTO</td><td>%s</td></tr>",($szTitolo=="")?("&nbsp"):($szTitolo));
		printf("<tr><td BGCOLOR=\"#4499aa\">ORDINI TOTALI</td><td>%d</td></tr>",$nOrdini);
		printf("<tr><td BGCOLOR=\"#4499aa\">QUANTITA' TOTALE</td><td>%d</td></tr>",$nQtyTotale);
		printf("<tr><td BGCOLOR=\"#4499aa\">QUANTITA' DA PRELEVARE</td><td>%d</td></tr>",$nQtyProd['A']);
		printf("<tr><td BGCOLOR=\"#4499aa\">QUANTITA' PRELEVATA</td><td>%d</td></tr>",$nQtyProd['P']);
		printf("<tr><td BGCOLOR=\"#4499aa\">QUANTITA' EVASA</td><td>%d</td></tr>",$nQtyProd['E']);
		printf("</table>");
	}
?>

</center>
</center>

<p><h3 align="center"><a href="index.html">Menu Principale</p>

</body>
</html>
