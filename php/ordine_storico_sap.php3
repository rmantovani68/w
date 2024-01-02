<html>

<head>
<meta http-equiv="Content-Type"
content="text/html; charset=iso-8859-1">
<meta name="GENERATOR" content="Sono stati io !!!">
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
	<td bgcolor=#608080 align=center><Font color=white size=5><B>RICERCA ORDINE IN STORICO - SAP</b></td>
</center>
</table>

<?php
	printf("<center><FORM method=\"get\" action=\"ordine_storico_sap.php3\">");
	printf("<table border=\"0\"><tr><th>ORDINE : </th><th><input type=\"text\" name=\"TXT_ORDINE\" size=\"15\" value=\"%s\">",$TXT_ORDINE);
	printf("</input></td>");
	printf("<th> <INPUT type=\"submit\" value=\"RICERCA\"form> </th> </tr> </table></center><br>");

	if(strlen($TXT_ORDINE)!=0){
		$db = new DB_Sql;
		$db -> Host = "database";
		$db -> Password = "3zin";
		$db -> User = "stefano";
		$db -> Database = "sap";
		$db -> Type = "pgsql";
		$db -> Debug = True;
		$db -> Halt_On_Error = "report";

		$szBuffer=sprintf("select ordprog from ord_stor where ordprog='%s'",$TXT_ORDINE);
		$db -> query($szBuffer);

		/*
		* controllo presenza codice ordine in archivio storico
		*/
		if(!$db->next_record()){
			printf("<h2 align=center>ORDINE NON PRESENTE</h2>");
		} else {
			$szBuffer=sprintf("select ronmcll,ronmrgh,ronmcpe,rodscli,rodteva,rotmeva,ropspre,ropsrea from ric_stor where ordprog='%s';",$TXT_ORDINE);
			$db -> query($szBuffer);
			if($db->next_record()){
				/*
				* tabella dati ordine
				*/
				$OrdNmColli=$db->f(0);
				$OrdNmRighe=$db->f(1);
				$OrdNmCopie=$db->f(2);
				$OrdDsCli  =$db->f(3);
				$OrdDtEva  =$db->f(4);
				$OrdTmEva  =$db->f(5);
				$OrdPsPre  =$db->f(6);
				$OrdPsRea  =$db->f(7);

				printf("<table BORDER COLS=4 WIDTH=\"100%%\">");
				printf("<tr> <td>CODICE</td> <td><input value=\"%s\"></input></td> <td>COLLI</td> <td><input value=\"%s\"></input></td> </tr>",$TXT_ORDINE,$OrdNmColli);
				printf("<tr><td>CODICE CLIENTE</td><td><input value=\"%s\"></input></td><td>RIGHE</td><td><input value=\"%s\"></input></td></tr>","&nbsp",$OrdNmRighe);
				printf("<tr><td>RAGIONE SOCIALE</td><td><input value=\"%s\"></input></td><td>ARTICOLI</td><td><input value=\"%s\"></input></td></tr>",$OrdDsCli,$OrdNmCopie);
				printf("<tr><td>DATA EVASIONE</td><td><input value=\"%s\"></input></td><td>PESO DETERMINATO (Kg.)</td><td><input value=\"%-9.3f\"></input></td></tr>",$OrdDtEva,$OrdPsPre/1000);
				printf("<tr><td>ORA EVASIONE</td><td><input value=\"%s\"></input></td><td>PESO REALE (Kg.)</td><td><input value=\"%-9.3f\"></input></td></tr>",$OrdTmEva,$OrdPsRea/1000);
				printf("</table>");

				$db->free();

				$szBuffer=sprintf("select col_stor.ordkey as \"ORDINE\",
																	col_stor.cpnmcol as \"COLLO\",
																	col_stor.cptpfor as \"FORMATO\",
																	col_stor.cpprgcl as \"SEGNAC.\",
																	col_stor.cpnmrgh as \"RIGHE\",
																	col_stor.cpnmcpe as \"COPIE\",
																	col_stor.cppspre::float4/1000 as \"PESO D(Kg)\",
																	col_stor.cppsrea::float4/1000 as \"PESO R(Kg)\",
																	col_stor.cpdteva as \"D.EVA.\",
																	col_stor.cptmeva as \"O.EVA\",
																	col_stor.cpstato as \"STATO\",
																	col_stor.cpcdflg as \"FORZATO\"
																	from col_stor
																	where col_stor.ordprog='%s' order by col_stor.ordkey,col_stor.cpnmcol;",$TXT_ORDINE);
				$db -> query($szBuffer);
				if($db->next_record()){
					/*
					* lista colli ordine
					*/
					printf("<p><br>LISTA COLLI<p>");
					printf("<table BORDER COLS=%d WIDTH=\"100%%\">",$db->num_fields());
					printf("<tr BGCOLOR=\"#4499aa\">");
					/* intestazione */
					for($nIndex=0;$nIndex<$db->num_fields();$nIndex++){
						printf("<td>%s</td>\n", $db->field_name($nIndex));
					}
					printf("</tr>");
					/* riempio la lista */
					do {
						printf("<tr>");
						for($nIndex=0;$nIndex<$db->num_fields();$nIndex++){
							if($db->field_null($nIndex)){
								printf("<td>&nbsp</td>\n");
							} else {
								printf("<td>%s</td>\n", (trim($db->f($nIndex))=="")?("&nbsp"):($db->f($nIndex)));
							}
						}
						printf("</tr>");
					} while ($db->next_record());
					printf("</table>");
				}
				$db->free();
			}

			printf("<p><br>PICKING LIST<p>");

      $szBuffer=sprintf("select r.ordkey as \"SOTTORDINE\",
      													r.rpnmcol as \"COLLO\",
                                r.rpcdpro as \"CODICE\",
																r.rpqtspe as \"QUANTITA'\",
																a.prdescr as \"DESCRIZIONE\" ,
																r.rpcdopr as \"OPERATORE\" ,
																r.rpdtpre as \"DATA PRELIEVO\" ,
																r.rptmpre as \"ORA PRELIEVO\" 
																from rig_stor r, articoli a where 
																r.rpcdpro=a.prcdpro and r.ordprog='%s' order by r.ordkey,r.rpnmcol;",$TXT_ORDINE);
			$db -> query($szBuffer);

			printf("<table BORDER COLS=%d WIDTH=\"100%%\">",$db->num_fields());
			printf("<tr BGCOLOR=\"#4499aa\">");
			/* intestazione */
			for($nIndex=0;$nIndex<$db->num_fields();$nIndex++){
				printf("<td>%s</td>\n", $db->field_name($nIndex));
			}
			printf("</tr>");
			/* riempio la lista */
			while($db->next_record()){
				for($nIndex=0;$nIndex<$db->num_fields();$nIndex++){
					if($db->field_null($nIndex)){
						printf("<td>&nbsp</td>\n");
					} else {
						printf("<td>%s</td>\n", (trim($db->f($nIndex))=="")?("&nbsp"):($db->f($nIndex)));
					}
				}
				printf("</tr>");
			}
			printf("</table><br><br>");
			$db->free();
		}
	}
?>

<p><h3 align="center"><a href="index.html">Menu Principale</p>

</body>
</html>
