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

<h1> Anagrafica Operatori</h1>

<?php
	require_once 'DB.php';
	$db =& DB::connect('pgsql://roberto:3zin@database/sap');
	if (PEAR::isError($db)) {
    die($db->getMessage());
	}

	$szBuffer=sprintf("select op.opcdope as \"CODICE\", 
	                  op.opnmope as \"NOME\", 
										op.opcgope as \"COGNOME\", 
										op.opinope as \"INDIRIZZO\", 
										op.oploope as \"LOC\", 
										op.opprope as \"PROVINCIA\"
										from operatori op order by op.opcdope;");
	$res =& $db->query($szBuffer);
	if (PEAR::isError($res)) {
		die($res->getMessage());
	}
	$resinfo = $res->tableInfo(DB_TABLEINFO_ORDER);

	printf("<table BORDER COLS=%d WIDTH=\"100%%\" BGCOLOR=\"#4cb6e8\" >",$res->numCols());

	printf("<tr BGCOLOR=\"#2255dd\">");
	for($nIndex=0;$nIndex<$res->numCols();$nIndex++){
		printf("<td>%s</td>\n",$resinfo[$nIndex]["name"]);
	}
	printf("</tr>");
	while($res->fetchInto($row)) {
		printf("<tr>");
		for($nIndex=0;$nIndex<$res->numCols();$nIndex++){
			if($nIndex==0){
				printf("<td><a href=modifica_operatore.php?CDOPE=%s>%s</a></td>\n", 
					$row[$nIndex],
					(trim($row[$nIndex])=="")?("&nbsp"):($row[$nIndex]));
			} else {
//				if($db->field_null($nIndex)){
//					printf("<td>&nbsp</td>\n");
//				} else {
//					printf("<td>%s</td>\n", (trim($row[$nIndex])=="")?("&nbsp"):($row[$nIndex]));
//				}
					printf("<td>%s</td>\n", (trim($row[$nIndex])=="")?("&nbsp"):($row[$nIndex]));
			}
		}
		printf("</tr>");
	}
	$res->free();

	printf("</table>");

	printf("<h1> Associazione Operatori / Settori</h1>");

	$szBuffer=sprintf("select op.opcdope as \"CODICE\", 
	                  op.opnmope as \"NOME\", 
										op.opcgope as \"COGNOME\", 
										os.opisola as \"ISOLA\", 
										os.opset as \"SETTORE\" 
										from operatori op,opeset os where op.opcdope=os.opcdope order by os.opisola,os.opset;");
	$res =& $db->query($szBuffer);
	if (PEAR::isError($res)) {
		die($res->getMessage());
	}
	$resinfo = $res->tableInfo(DB_TABLEINFO_ORDER);

	printf("<table BORDER COLS=%d WIDTH=\"100%%\" BGCOLOR=\"#4cb6e8\" >",$res->numCols());

	printf("<tr BGCOLOR=\"#2255dd\">");
	for($nIndex=0;$nIndex<$res->numCols();$nIndex++){
		printf("<td>%s</td>\n", $resinfo[$nIndex]["name"]);
	}
	printf("</tr>");
	while($res->fetchInto($row)) {
		printf("<tr>");
		for($nIndex=0;$nIndex<$res->numCols();$nIndex++){
//			if($db->field_null($nIndex)){
//				printf("<td>&nbsp</td>\n");
//			} else {
//				printf("<td>%s</td>\n", (trim($row[$nIndex])=="")?("&nbsp"):($row[$nIndex]));
//			}
				printf("<td>%s</td>\n", (trim($row[$nIndex])=="")?("&nbsp"):($row[$nIndex]));
		}
		printf("</tr>");
	}
	$res->free();

	printf("</table>");

	printf("<p><a href=\"insert_operatore.php\">INSERIMENTO OPERATORE </a></p>");
?>

<p><h3 align="center"><a href="index.html">Menu Principale</p>

</body>
</html>
