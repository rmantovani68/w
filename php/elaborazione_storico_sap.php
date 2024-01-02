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
	<td bgcolor=#608080 align=center><Font color=white size=5><B>RICERCA ELABORAZIONE IN STORICO - SAP</b></td>
</center>
</table>

<?php
	$nSettori[0]=8;
	$nSettori[1]=8;

	printf("<center><FORM method=\"get\" action=\"elaborazione_storico_sap.php\">");
	printf("<table border=\"0\"><tr><th align=\"left\">ELABORAZIONE  </td><td><input type=\"text\" name=\"TXT_ELABORAZIONE\" size=\"15\" value=\"%s\">",$TXT_ELABORAZIONE);
	printf("</input></td>");
	printf("<th> <INPUT type=\"submit\" value=\"RICERCA\"></form></th></tr></table></center><br>");

  $TXT_ELABORAZIONE=$_GET['TXT_ELABORAZIONE'];

	if(strlen($TXT_ELABORAZIONE)!=0){
		require_once 'DB.php';
		$db =& DB::connect('pgsql://roberto:3zin@database/sap');
		if (PEAR::isError($db)) {
			die($db->getMessage());
		}
		

		$szBuffer=sprintf("select opnelab from ord_stor where opnelab='%s';",$TXT_ELABORAZIONE);
		$res =& $db->query($szBuffer);
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}

		/*
		* controllo presenza codice elaborazione in archivio storico
		*/
		if(!$res->fetchInto($row)){
			printf("<h2 align=center>ELABORAZIONE NON PRESENTE</h2>");
		} else {

			/* 
			* RICAVARE DATA,ORA INIZIO E FINE ELABORAZIONE
			*/
			
			/* Numero Ordini Elaborazione */
			$szBuffer=sprintf("select count(ordprog) from ric_stor where ronelab='%s';",$TXT_ELABORAZIONE);
			$res1 =& $db->query($szBuffer);
			if (PEAR::isError($res1)) {
				die($res1->getMessage());
			}
			if($res1->fetchInto($row1)){
				$nOrdiniElab=$row1[0];
			}
			$res1->free();

			/* Numero sottordini Elaborazione */
			$szBuffer=sprintf("select count(ordkey) from ord_stor where opnelab='%s';",$TXT_ELABORAZIONE);
			$res1 =& $db->query($szBuffer);
			if (PEAR::isError($res1)) {
				die($res1->getMessage());
			}
			if($res1->fetchInto($row1)){
				$nSottordiniElab=$row1[0];
			}
			$res1->free();

			/* righe, copie, colli totali - peso reale e predeterminato */ 
			$szBuffer=sprintf("select sum(cpnmrgh), sum(cpnmcpe), count(cpnmcol), sum(cppspre), sum(cppsrea) from col_stor where cpnelab='%s';",$TXT_ELABORAZIONE);
			$res1 =& $db->query($szBuffer);
			if (PEAR::isError($res1)) {
				die($res1->getMessage());
			}
			if($res1->fetchInto($row1)){
				$nRigheElab=$row1[0];
				$nCopieElab=$row1[1];
				$nColliElab=$row1[2];
				$nPesopreElab=$row1[3];
				$nPesoreaElab=$row1[4];
			}
			$res1->free();

			printf("<br><table COLS=2 WIDTH=\"40%%\">");
			printf("<tr><th align=left>ELABORAZIONE<th align=left>%d<tr>",$TXT_ELABORAZIONE);
			printf("<th align=left>ORDINI<th align=left>%d<tr>",$nOrdiniElab);
			printf("<th align=left>SOTTORDINI<th align=left>%d<tr>",$nSottordiniElab);
			printf("<th align=left>RIGHE<th align=left>%d<tr>",$nRigheElab);
			printf("<th align=left>PEZZI<th align=left>%d<tr>",$nCopieElab);
			printf("<th align=left>COLLI<th align=left>%d<tr>",$nColliElab);
			printf("<th align=left>PRESE<th align=left>%d<tr>",$nColliElab+$nCopieElab);
			printf("<th align=left>PESO CALCOLATO(kg.)<th align=left>%15.3f<tr>",(float)((float)($nPesopreElab)/(float)1000));
			printf("<th align=left>PESO RILEVATO(kg.)<th align=left>%15.3f<tr>",(float)((float)($nPesoreaElab)/(float)1000));
			printf("</table><br><br>");
			printf("<h2 align=left>DATI SUDDIVISI PER ISOLA</h2>");

			/*
			* DATI SUDDIVISI PER ISOLA
			*/
			$szBuffer = sprintf("select distinct(ord_stor.opcdisl),
																	count(ord_stor.opcdisl),
																	sum(ord_stor.opnmcll),
																	sum(ord_stor.opnmrgh),
																	sum(ord_stor.opnmcpe),
																	sum(ord_stor.oppspre)::float4/1000,
																	sum(ord_stor.oppsrea)::float4/1000
																	from ord_stor where ord_stor.opnelab='%s' group by ord_stor.opcdisl;",$TXT_ELABORAZIONE);
			$res1 =& $db->query($szBuffer);
			if (PEAR::isError($res1)) {
				die($res1->getMessage());
			}
			printf("<table BORDER=1 COLS=5 WIDTH=\"80%%\" BGCOLOR=\"#4499aa\" >");
			printf("<tr><th align=left>ISOLA<th align=left>SOTTORDINI<th align=left>RIGHE<th align=left>PEZZI<th align=left>COLLI<th align=left>PRESE<th align=left>PESO D.(Kg)<th align=left>PESO R.(Kg)<tr>");

			$nTotSottordini=0;
			$nTotColli=0;
			$nTotLinee=0;
			$nTotPrese=0;
			$nTotPesoPre=0;
			$nTotPesoRea=0;
			$nTotCopie=0;
			$nColli[2000];

			for($nIndex=0;$nIndex<$res1->numRows();$nIndex++){
				if($res1->fetchInto($row1)){
					printf("<tr><th align=left>%d<td>%d<td>%d<td>%d<td>%d<td>%d<td>%15.3f<td>%15.3f<tr>",$row1[0],$row1[1],$row1[3],$row1[4],$row1[2],$row1[4]+$row1[2],$row1[5],$row1[6]);
					$nTotSottordini+=$row1[1];
					$nTotColli+=$row1[2];
					$nTotLinee+=$row1[3];
					$nTotPrese+=$row1[4]+$row1[2];
					$nColli[$row1[0]]=$row1[2];
					$nTotCopie+=$row1[4];
					$nTotPesoPre+=$row1[5];
					$nTotPesoRea+=$row1[6];
				}
			}
			printf("<tr><th align=left>%s<td>%d<td>%d<td>%d<td>%d<td>%d<td>%15.3f<td>%15.3f<tr>",TOTALI,$nTotSottordini,$nTotLinee,$nTotCopie,$nTotColli,$nTotPrese,$nTotPesoPre,$nTotPesoRea);
			printf("</table>");
			$res1->free();

			/*
			* DATI SUDDIVISI PER SETTORE
			*/
			for($nIndexIsola=1;$nIndexIsola<=2;$nIndexIsola++){
				/* controllo che l'isola abbia dei ordini altrimenti non visualizzo la tabella */
				if($nColli[$nIndexIsola]!=0){
					printf("<br><h2>ISOLA%d</h2>",$nIndexIsola);
					printf("<table BORDER=1 COLS=4 WIDTH=\"40%%\" BGCOLOR=\"#44AA66\" >");
					printf("<tr><th align=left>SETTORE</td>");
					printf("<th align=left>RIGHE</td>");
					printf("<th align=left>PEZZI</td>");
					printf("<th align=left>PRESE</td></tr>");
					$szBuffer = sprintf("select distinct(substr(rig_stor.rpcdubi,2,2)),sum(rig_stor.rpqtspe),count(rig_stor.rpqtspe) from rig_stor,ord_stor where rig_stor.ordkey=ord_stor.ordkey and ord_stor.opcdisl=%d and ord_stor.opnelab='%s' group by substr(rig_stor.rpcdubi,2,2);",$nIndexIsola,$TXT_ELABORAZIONE);
					$res1 =& $db->query($szBuffer);
					if (PEAR::isError($res1)) {
						die($res1->getMessage());
					}
					for($nIndex=0;$nIndex<$nSettori[$nIndexIsola-1];$nIndex++){
						if($res1->fetchInto($row1)){
						}
						printf("<tr><th align=left>%d<td>%d<td>%d<td>%d<tr>",$nIndex+1,$row1[2],$row1[1],($nIndex==0)?($row1[1]+$nColli[$nIndexIsola]):($row1[1]));
					}
					printf("</table>");
					$res1->free();

					/*
					* FORMATI UTILIZZATI PER ISOLA
					*/
					$szBuffer=sprintf("select distinct cptpfor, count(cptpfor) from col_stor
														 where cpnelab='%s' and substr(ordkey,12,1)='%d'
														 group by cptpfor;",$TXT_ELABORAZIONE,$nIndexIsola);
					$res1 =& $db->query($szBuffer);
					if (PEAR::isError($res1)) {
						die($res1->getMessage());
					}
					if($res1->fetchInto($row1)){
						printf("<br><h3>IMBALLI UTILIZZATI IN ISOLA %d</h3>",$nIndexIsola);
						printf("<table BORDER=1 COLS=2 WIDTH=\"40%%\" BGCOLOR=\"#44AA66\" >");
						printf("<tr><th align=left>FORMATO</td>");
						printf("<th align=left>QUANTITA'</td><tr>");
						do {
							printf("<tr><th align=left>%s<td>%d<tr>",$row1[0],$row1[1]);
						} while($res1->fetchInto($row1));
						printf("<tr><th align=left>TOTALE<th align=left>%d",$nColli[$nIndexIsola]);
						printf("</table>");
					}
					$res1->free();
				}
			}
		}
		$res->free();
	}
?>
			
<p><h3 align="center"><a href="index.html">Menu Principale</p>

</body>
</html>

