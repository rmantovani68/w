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
        <td nosave><img src="cni-ril-titolo.gif" width="354"
        height="165"></td>
        <td>&nbsp;</td>
        <td>WELLA ITALIA<p>Impianto PAPERLESS <br>
        Castiglione delle Stiviere (MN) <br>
        <img src="logo_wella.gif" width="64" height="50"></p>
        </td>
    </tr>
</table>

<h1> Anagrafica Operatori</h1>

<?php
	$tb = new Table;
	$tb -> heading = True;
	$db = new DB_Sql;
	$db -> Host = "database";
	$db -> Password = "3zin";
	$db -> User = "roberto";
//	$db -> Database = "picking";
	$db -> Database = "sap";
	$db -> Type = "pgsql";
	$db -> Debug = True;
	$db -> Halt_On_Error = "report";

	$db->query("select opcdope as \"CODICE\", 
	                  opnmope as \"NOME\", 
										opcgope as \"COGNOME\", 
										opinope as \"INDIRIZZO\", 
										oploope as \"LOC\", 
										opprope as \"PROVINCIA\", 
										opcpope as \"CAP\", 
										opnmtel as \"TELEFONO\" 
										from operatori;");

/*
	$tb->show_result($db);
*/

	printf("<table BORDER COLS=%d WIDTH=\"100%%\" BGCOLOR=\"#33CCFF\" >",$db->num_rows());

	printf("<tr BGCOLOR=\"#2255dd\">");
	for($nIndex=0;$nIndex<$db->num_fields();$nIndex++){
		printf("<td>%s</td>\n", $db->field_name($nIndex));
	}
	printf("</tr>");
	while($db->next_record()) {
		printf("<tr>");
		for($nIndex=0;$nIndex<$db->num_fields();$nIndex++){
			printf("<td>%s</td>\n", $db->f($nIndex));
		}
		printf("</tr>");
	}
	$db->free();

	printf("</table>");

	printf("<p><a href=\"insert_operatore.php3\">Inserimento Operatore </a></p>");
?>

</body>
</html>
