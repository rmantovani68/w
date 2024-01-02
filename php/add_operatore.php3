
<!doctype html public "-//w3c//dtd html 4.0 transitional//en">
<html>
<head>
   <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
   <meta name="GENERATOR" content="Mozilla/4.72 [en] (X11; U; Linux 2.2.14-12 i686) [Netscape]">
   <title>anagrafica</title>
</head>

<body bgcolor="#C0C0C0">

<table border="0" width="100%" cols="3" nosave>
    <tr>
        <td nosave><img src="cni-ril-titolo.gif" height=80 width=171></td>
				<td><h1>WELLA ITALIA</h1><p><h2>Impianto Paperless</h2><p>Castiglione delle Stiviere (MN)<td>
        <td><img src="logo_wella.gif" align="right"></td>
		</tr>
</table>

<h1>
Inserimento Operatore</h1>
&nbsp;

<?php
	$db = new DB_Sql;
	$db -> Host = "database";
	$db -> Password = "3zin";
	$db -> User = "roberto";
//	$db -> Database = "picking";
	$db -> Database = "sap";
	$db -> Type = "pgsql";
	$db -> Debug = True;
	$db -> Halt_On_Error = "report";

	$db->query("INSERT INTO operatori (opcdope, opnmope, opcgope, opinope, oploope, opprope, opcpope, opnmtel) VALUES('$codice', '$nome',  '$cognome',  '$indirizzo', '$localita',  '$provincia', '$cap',  '$telefono');");

	if($db->affected_rows()==1){
		printf("<br>Inserimento Operatore");
		printf("<br>Avvenuto correttamente");
	}
	$db->free();
?>
<p><h3 align="center"><a href="anagr_operatori.php3">Operatori</a></p>
<p><h3 align="center"><a href="index.html">Menu Principale</p>

</html>
