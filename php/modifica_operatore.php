<!doctype html public "-//w3c//dtd html 4.0 transitional//en">
<html>
<head>
   <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
   <meta name="GENERATOR" content="Mozilla/4.72 [en] (X11; U; Linux 2.2.14-12 i686) [Netscape]">
   <title>Anagrafica Operatori</title>
</head>

<body bgcolor="#C0C0C0">

<table border="0" width="100%" cols="3" nosave>
    <tr>
        <td nosave><img src="cni-ril-titolo.gif" height=80 width=171></td>
				<td><h1>WELLA ITALIA</h1><p><h2>Impianto Paperless</h2><p>Castiglione delle Stiviere (MN)<td>
        <td><img src="logo_wella.gif" align="right"></td>
		</tr>
</table>

<h1> Modifica dati Operatore</h1>
<center>

<?php

	require_once 'DB.php';
	$db =& DB::connect('pgsql://roberto:3zin@database/sap');
	if (PEAR::isError($db)) {
    die($db->getMessage());
	}

	$szBuffer = sprintf("SELECT opcdope, opnmope, opcgope, opinope, oploope, opprope, opcpope, opnmtel FROM OPERATORI WHERE opcdope='%s';",$CDOPE);

	$res =& $db->query($szBuffer);
	if (PEAR::isError($res)) {
		die($res->getMessage());
	}

 	if($res->fetchInto($row)){
		printf("<table border=0>");
		$szCodOpe = $row[0];
		printf("<form action=\"mod_operatore.php\" method=\"GET\"> <tr><th align=left>Dati Operatore : <font color=red>%s</font></th></tr>",$szCodOpe);
		printf("<tr><td align=right><font color=red>*</font>Codice: <input type=\"text\" name=\"codice\" VALUE=\"%s\">",$row[0]);
		printf("<tr><td align=right><font color=red>*</font>Nome: <input type=\"text\" name=\"nome\" VALUE=\"%s\">",$row[1]);
		printf("<tr><td align=right><font color=red>*</font>Cognome: <input type=\"text\" name=\"cognome\" VALUE=\"%s\">",$row[2]);
		printf("<tr><td align=right>Telefono: <input type=\"text\" name=\"telefono\" VALUE=\"%s\">",$row[7]);
		printf("<tr><td align=right>CAP: <input type=\"text\" name=\"cap\" VALUE=\"%s\">",$row[6]);
		printf("<tr><td align=right>Localita': <input type=\"text\" name=\"localita\" VALUE=\"%s\">",$row[4]);
		printf("<tr><td align=right>Provincia: <input type=\"text\" name=\"provincia\" VALUE=\"%s\">",$row[5]);
		printf("<tr><td align=right>Indirizzo: <input type=\"text\" name=\"indirizzo\" VALUE=\"%s\">",$row[3]);
	}
	$res->free();
?>

<tr>
<th colspan=3 align=center><input type=submit value='Modifica Operatore'>
</form>
</table>
</center>
</html>

</body>
</html>
