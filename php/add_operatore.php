
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
	require_once 'DB.php';
	$db =& DB::connect('pgsql://roberto:3zin@database/sap');
	if (PEAR::isError($db)) {
    die($db->getMessage());
	}

	$szBuffer=sprintf("INSERT INTO operatori (opcdope, opnmope, opcgope, opinope, oploope, opprope, opcpope, opnmtel) VALUES('$codice', '$nome',  '$cognome',  '$indirizzo', '$localita',  '$provincia', '$cap',  '$telefono');");
		printf("%s",$szBuffer );
	$pre = $db->prepare($szBuffer);
	if (PEAR::isError($pre)) {
    die($pre->getMessage());
	}
	$res = $db->execute($pre);
	if (PEAR::isError($res)) {
		die($res->getMessage());
	}

	if($db->affectedRows()==1){
		printf("<br>Inserimento Operatore");
		printf("<br>Avvenuto correttamente");
	}
	$db->freePrepared($pre);
?>
<p><h3 align="center"><a href="anagr_operatori.php">Operatori</a></p>
<p><h3 align="center"><a href="index.html">Menu Principale</p>

</html>
