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

<table>
<center>
	<table border=1 width=100%>
	<td bgcolor=#6090b0 align=center><Font color=white size=5><B>STATISTICA DI PRODUTTIVITA` - SAP</B></td>
</center>
</table>



<?php
	/*
	* Sta roba e' da eliminare, serve solo qui in AeL
	require("db_pgsql.inc");  
	require("table.inc");    
	*/

	$db = new DB_Sql;
	$db -> Host = "database";
	$db -> Password = "3zin";
	$db -> User = "stefano";
	$db -> Database = "sap";
	$db -> Type = "pgsql";
	$db -> Debug = True;
	$db -> Halt_On_Error = "report";

	$db_time = new DB_Sql;
	$db_time -> Host = "database";
	$db_time -> Password = "3zin";
	$db_time -> User = "stefano";
	$db_time -> Database = "sap";
	$db_time -> Type = "pgsql";
	$db_time -> Debug = True;
	$db_time -> Halt_On_Error = "report";

	$mesi[]="GENNAIO";
	$mesi[]="FEBBRAIO";
	$mesi[]="MARZO";
	$mesi[]="APRILE";
	$mesi[]="MAGGIO";
	$mesi[]="GIUGNO";
	$mesi[]="LUGLIO";
	$mesi[]="AGOSTO";
	$mesi[]="SETTEMBRE";
	$mesi[]="OTTOBRE";
	$mesi[]="NOVEMBRE";
	$mesi[]="DICEMBRE";

	if($CB_DA_ANNO=="" && $CD_DA_MESE=="" && $CD_DA_GIORNO=="" &&
	   $CB_A_ANNO=="" && $CD_A_MESE=="" && $CD_A_GIORNO==""){
		/*
		* Ricavo la data odierna
		*/
		$date = getdate();

		$da_anno=date("Y");
		$da_mese=date("m");
		$da_giorno=date("d");
		$a_anno=date("Y");
		$a_mese=date("m");
		$a_giorno=date("d");
	} else {
		$da_anno=$CB_DA_ANNO;
		$da_mese=$CB_DA_MESE;
		$da_giorno=$CB_DA_GIORNO;
		$a_anno=$CB_A_ANNO;
		$a_mese=$CB_A_MESE;
		$a_giorno=$CB_A_GIORNO;
	}


	/* definizione tabella di inserimento date */
	printf("<div align=\"center\"><center> <form method=\"get\" action=\"gruppo_elaborazioni_sap.php3\"> <table border=\"0\" cellpadding=\"5\" cellspacing=\"5\">");
	printf("<tr> <td><strong>DAL:</strong></td> <td align=\"center\"><strong>ANNO</strong></td> <td>");
	printf("<p><select name=\"CB_DA_ANNO\" size=\"1\">");
	for($nIndex=2000;$nIndex<2015;$nIndex++){
		if($da_anno==$nIndex){
			printf("<option selected value=$nIndex>$nIndex</option>");
		} else {
			printf("<option value=$nIndex>$nIndex</option>");
		}
	}
	printf("</select></p> </td> <td align=\"center\"><strong>MESE</strong></td> <td>");
	printf("<p><select name=\"CB_DA_MESE\" size=\"1\">");
	for($nIndex=0;$nIndex<=11;$nIndex++){
		if($da_mese==$nIndex+1){
			printf("<option selected value=%d>%s</option>",$nIndex+1,$mesi[$nIndex]);
		} else {
			printf("<option value=%d>%s</option>",$nIndex+1,$mesi[$nIndex]);
		}
	}
	printf("</select></p> </td> <td><strong>GIORNO</strong></td> <td>");
	printf("<p><select name=\"CB_DA_GIORNO\" size=\"1\">");
	for($nIndex=0;$nIndex<=30;$nIndex++){
		if($da_giorno==$nIndex+1){
			printf("<option selected value=%d>%s</option>",$nIndex+1,$nIndex+1);
		} else {
			printf("<option value=%d>%s</option>",$nIndex+1,$nIndex+1);
		}
	}
	printf("</select></p> </td> </tr> <tr>");
	printf("<td><strong>AL:</strong></td> <td align=\"center\"><strong>ANNO</strong></td> <td>");
	printf("<p><select name=\"CB_A_ANNO\" size=\"1\">");
	for($nIndex=2000;$nIndex<2015;$nIndex++){
		if($a_anno==$nIndex){
			printf("<option selected value=$nIndex>$nIndex</option>");
		} else {
			printf("<option value=$nIndex>$nIndex</option>");
		}
	}
	printf("</select></p> </td> <td align=\"center\"><strong>MESE</strong></td> <td>");
	printf("<p><select name=\"CB_A_MESE\" size=\"1\">");
	for($nIndex=0;$nIndex<=11;$nIndex++){
		if($a_mese==$nIndex+1){
			printf("<option selected value=%d>%s</option>",$nIndex+1,$mesi[$nIndex]);
		} else {
			printf("<option value=%d>%s</option>",$nIndex+1,$mesi[$nIndex]);
		}
	}
	printf("</select></p> </td> <td><strong>GIORNO</strong></td> <td>");
	printf("<p><select name=\"CB_A_GIORNO\" size=\"1\">");
	for($nIndex=0;$nIndex<=30;$nIndex++){
		if($a_giorno==$nIndex+1){
			printf("<option selected value=%d>%s</option>",$nIndex+1,$nIndex+1);
		} else {
			printf("<option value=%d>%s</option>",$nIndex+1,$nIndex+1);
		}
	}
	printf("</select></p> </td> </tr> <tr>");
	printf("<td><strong></strong>&nbsp;</td> <td align=\"right\"><strong></strong>&nbsp;</td> <td>&nbsp;</td>");
	printf("<td><strong></strong> <p><input type=\"submit\" value=\"ESEGUI STATISTICA\"></p>");
	printf("</td> <td>&nbsp;</td> <td><strong></strong>&nbsp;</td> <td>&nbsp;</td> </tr>");
	printf("</table></form> </center></div>");
	/* fine definizione tabella */

	$szFromDate = sprintf("%04d%02d%02d",$da_anno,$da_mese,$da_giorno);
	$szToDate   = sprintf("%04d%02d%02d",$a_anno,$a_mese,$a_giorno);
		
	if($CB_DA_ANNO>0){
		printf("<Font size=\"5\"><B>STATISTICHE DI EVASIONE DAL %02d-%02d-%04d AL %02d-%02d-%04d</B>",$da_giorno,$da_mese,$da_anno,$a_giorno,$a_mese,$a_anno);
	

		/* ricavo il tempo di start dell'impianto */


		/*
		* LG 31/01/2005: modifica per dividere il tempo di start tra isola 1 e 2
		*                contestualmente e' stato aggiunto nella tabella eventi il campo isola
		*                Per la modifica la soluzione piu' semplice e' di duplicare il codice,
		*                ma forse si puo' rendere piu' efficiente con una sola query
		*/

		/***********
		 * ISOLA 1 *
		 ***********/
		$bStart=0;
		$bFirstTime=1;
		$tempo_start=0;
		$ore_start_isola1=0;
		$min_start_isola1=0;
		$sec_start_isola1=0;
		$sec_start=0;

		$szBuffer=sprintf("select evento,data from eventi where date(data)>='%04d-%02d-%02d' and date(data)<='%04d-%02d-%02d' and isola=1 order by data;",$da_anno,$da_mese,$da_giorno,$a_anno,$a_mese,$a_giorno);
		$db -> query($szBuffer);

		while($db->next_record()){

			$evento=$db->f(0);
			$time=$db->f(1);

			//printf ("EVENTO '%s' - %s<br>",$evento,$time);
			if(!$bStart && !strcmp(trim($evento),"START")){
				/* comincio a 'contare' */
				$bStart=1;
				$time_start=$time;
				//printf ("START %s<br>",$time_start);
			} else if($bStart && !strcmp(trim($evento),"STOP")){
				if(!$bFirstTime){
					/* Aggiungo al tempo di start */
					$bStart=0;
					$time_stop=$time;
					//printf ("STOP %s<br>",$time_stop);

					$szBuffer=sprintf("select extract(epoch from '%s'::timestamp)-extract(epoch from '%s'::timestamp);",$time_stop,$time_start);

					$db_time->query($szBuffer);

					/* --- */
					//printf("<br>select extract(epoch from '%s'::timestamp)-extract(epoch from '%s'::timestamp);<br>",$time_stop,$time_start);

					if($db_time->next_record()){

						//printf("RESULT : %s<br>",$db_time->f(0));

						$tempo_start+=$db_time->f(0);

						//printf ("TEMPO START %s<br>",$tempo_start);

					}
					$db_time->free();
				}
			}
			$bFirstTime=0;
		}

		$db->free();

		/*
		* se alla fine del ciclo sono ancora in start
		* sommo al tempo in start la differenza fra adesso e l'ultimo evento di start 
		* LG 19-04-2006: MHHHH!!!! Ma se ho un crash e finisco il periodo con uno start
		* allora mi viene un tempo enorme! Se ci fossero problemi questa parte andrebbe
		* tolta a costo di perdere un giorno di lavoro (acceso al mattino e crash alla
		* sera senza riaccensione).
		*/
		if($bStart){
			$szBuffer=sprintf("select extract(epoch from 'now'::timestamp)-extract(epoch from '%s'::timestamp);",$time_start);

			$db_time->query($szBuffer);

			/* --- */
			//printf("<br>select extract(epoch from 'now'::timestamp)-extract(epoch from '%s'::timestamp);<br>",$time_start);

			if($db_time->next_record()){

				//printf("RESULT : %s<br>",$db_time->f(0));

				$tempo_start+=$db_time->f(0);
				//printf ("TEMPO START %s<br>",$tempo_start);
			}

			$db_time->free();
		}

		$tempo_start_isola1=$tempo_start;
		$ore_start_isola1=(int)($tempo_start/3600);
		$min_start_isola1=(int)(($tempo_start-$ore_start_isola1*3600)/60);
		$sec_start_isola1=(int)($tempo_start-$ore_start_isola1*3600-$min_start_isola1*60);
		printf("<br><Font size=\"5\"><B>Tempo di Start Isola 1: %02d:%02d:%02d </B>",$ore_start_isola1,$min_start_isola1,$sec_start_isola1);


		/***********
		 * ISOLA 2 *
		 ***********/

		$bStart=0;
		$bFirstTime=1;
		$tempo_start=0;
		$ore_start_isola2=0;
		$min_start_isola2=0;
		$sec_start_isola2=0;

		$szBuffer=sprintf("select evento,data from eventi where date(data)>='%04d-%02d-%02d' and date(data)<='%04d-%02d-%02d' and isola=2 order by data;",$da_anno,$da_mese,$da_giorno,$a_anno,$a_mese,$a_giorno);
		$db -> query($szBuffer);

		while($db->next_record()){

			$evento=$db->f(0);
			$time=$db->f(1);

			if(!$bStart && !strcmp(trim($evento),"START")){
				/* comincio a 'contare' */
				$bStart=1;
				$time_start=$time;
			} else if($bStart && !strcmp(trim($evento),"STOP")){
				if($bFirstTime){
					/* Aggiungo al tempo di start */
					$bStart=0;
					$time_stop=$time;

					$szBuffer=sprintf("select extract(epoch from '%s'::timestamp)-extract(epoch from '%s'::timestamp);",$time_stop,$time_start);
					$db_time->query($szBuffer);

					if($db_time->next_record()){
						$tempo_start+=$db_time->f(0);
					}
					$db_time->free();
				}
			}
			$bFirstTime=0;
		}
		$db->free();

		/*
		* se alla fine del ciclo sono ancora in start
		* sommo al tempo in start la differenza fra adesso e l'ultimo evento di start 
		*/
		if($bStart){
			$szBuffer=sprintf("select extract(epoch from 'now'::timestamp)-extract(epoch from '%s'::timestamp);",$time_start);
			$db_time->query($szBuffer);

			if($db_time->next_record()){
				$tempo_start+=$db_time->f(0);
			}
			$db_time->free();
		}
		
		$tempo_start_isola2=$tempo_start;
		$ore_start_isola2=(int)($tempo_start/3600);
		$min_start_isola2=(int)(($tempo_start-$ore_start_isola2*3600)/60);
		$sec_start_isola2=(int)($tempo_start-$ore_start_isola2*3600-$min_start_isola2*60);
		printf("<br><Font size=\"5\"><B>Tempo di Start Isola 2: %02d:%02d:%02d </B>",$ore_start_isola2,$min_start_isola2,$sec_start_isola2);

		/***********
		 * SOMMA   *
		 ***********/
		$tempo_start=$tempo_start_isola1+$tempo_start_isola2;
		$ore_start=(int)($tempo_start/3600);
		$min_start=(int)(($tempo_start-$ore_start*3600)/60);
		$sec_start=(int)($tempo_start-$ore_start*3600-$min_start*60);
		printf("<br><Font size=\"5\"><B>Somma dei tempi Isole 1 e 2: %02d:%02d:%02d </B>",$ore_start,$min_start,$sec_start);
		
		/***********
		 * TOTALE  *
		 ***********/
		$bStart=0;
		$bFirstTime=1;
		$bStartIsola1=0;
		$bStartIsola2=0;
		$tempo_start=0;
		$ore_start=0;
		$min_start=0;
		$sec_start=0;

		$szBuffer=sprintf("select evento,data,isola from eventi where date(data)>='%04d-%02d-%02d' and date(data)<='%04d-%02d-%02d' order by data;",$da_anno,$da_mese,$da_giorno,$a_anno,$a_mese,$a_giorno);
		$db -> query($szBuffer);

		while($db->next_record()){

			$evento=$db->f(0);
			$time=$db->f(1);
			$isola=$db->f(2);

			if(!strcmp(trim($evento),"START")){
				if($isola==1){
					$bStartIsola1=1;
				} else if ($isola==2){
					$bStartIsola2=1;
				}
				if(!$bStart && ($bStartIsola1 || $bStartIsola2)){
					/* comincio a 'contare' */
					$bStart=1;
					$time_start=$time;
				}
			} else if(!strcmp(trim($evento),"STOP")){
				if(!$bFirstTime){
					if($isola==1){
						$bStartIsola1=0;
					} else if ($isola==2){
						$bStartIsola2=0;
					}
					if($bStart && (!$bStartIsola1 && !$bStartIsola2)){
						/* Aggiungo al tempo di start */
						$bStart=0;
						$time_stop=$time;
						$szBuffer=sprintf("select extract(epoch from '%s'::timestamp)-extract(epoch from '%s'::timestamp);",$time_stop,$time_start);
						$db_time->query($szBuffer);

						if($db_time->next_record()){
							$tempo_start+=$db_time->f(0);
						}
						$db_time->free();
					}
				}
			}
			$bFirstTime=0;
		} 
		$db->free();

		/*
		* se alla fine del ciclo sono ancora in start
		* sommo al tempo in start la differenza fra adesso e l'ultimo evento di start 
		*/
		if($bStart){
			$szBuffer=sprintf("select extract(epoch from 'now'::timestamp)-extract(epoch from '%s'::timestamp);",$time_start);
			$db_time->query($szBuffer);

			if($db_time->next_record()){
				$tempo_start+=$db_time->f(0);
			}
			$db_time->free();
		}
		
		$ore_start=(int)($tempo_start/3600);
		$min_start=(int)(($tempo_start-$ore_start*3600)/60);
		$sec_start=(int)($tempo_start-$ore_start*3600-$min_start*60);
		printf("<br><Font size=\"5\"><B>Tempo di Start Totale: %02d:%02d:%02d </B>",$ore_start,$min_start,$sec_start);

		/***********************************************
		 * Fine calcolo dei tempi di Start Isole 1 e 2 *
		 ***********************************************/



		$szBuffer=sprintf("select distinct ronelab from ric_stor where rodteva>='%s' and rodteva<='%s' group by ronelab;",$szFromDate,$szToDate);
		$db -> query($szBuffer);

		/*
		* controllo presenza codice elaborazione in archivio storico
		*/
		if(!$db->next_record()){
			printf("<br><Font size=\"5\"><B> NON DISPONIBILI</B>");
		} else {
			/* Numero Elaborazioni */
			$nElaborazioni=$db->num_rows();

			/* 
			* RICAVARE DATA,ORA INIZIO E FINE ELABORAZIONE
			*/
			
//			/* Numero Ordini Elaborazione */
//			$szBuffer=sprintf("select count(ordprog) from ric_stor where rodteva>='%s' and rodteva<='%s';",$szFromDate,$szToDate);
//			$db -> query($szBuffer);
//			if($db->next_record()){
//				$nOrdiniElab=$db->f(0);
//			}
//			$db->free();
//
//			/* Numero sottordini Elaborazione */
//			$szBuffer=sprintf("select count(ordkey) from ord_stor where opdteva>='%s' and opdteva<='%s';",$szFromDate,$szToDate);
//			$db -> query($szBuffer);
//			if($db->next_record()){
//				$nSottordiniElab=$db->f(0);
//			}
//			$db->free();
//
//			/* righe, copie, colli totali - peso reale e predeterminato */ 
//			$szBuffer=sprintf("select sum(cpnmrgh), sum(cpnmcpe), count(cpnmcol), sum(cppspre), sum(cppsrea) from col_stor where cpdteva>='%s' and cpdteva<='%s';",$szFromDate,$szToDate);

			$szBuffer=sprintf("select sum(opnmrgh), sum(opnmcpe), sum(opnmcll), sum(oppspre), sum(oppsrea), count(ordkey), count(ordprog) from ord_stor where opdteva>='%s' and opdteva<='%s';",$szFromDate,$szToDate);

			$db -> query($szBuffer);
			if($db->next_record()){
				$nRigheElab=$db->f(0);
				$nCopieElab=$db->f(1);
				$nColliElab=$db->f(2);
				$nPesopreElab=$db->f(3);
				$nPesoreaElab=$db->f(4);
				$nSottordiniElab=$db->f(5);
				$nOrdiniElab=$db->f(6);
			}
			$db->free();

			printf("<br><table COLS=2 WIDTH=\"40%%\">");
			printf("<tr><th align=left>ELABORAZIONI<th align=left>%d<tr>",$nElaborazioni);
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
			$db->free();

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
																	from ord_stor where ord_stor.opdteva>='%s' and ord_stor.opdteva<='%s' group by ord_stor.opcdisl;",$szFromDate,$szToDate);
			$db -> query($szBuffer);
			printf("<table BORDER=1 COLS=5 WIDTH=\"80%%\" BGCOLOR=\"#4499aa\" >");
			printf("<tr><th align=left>ISOLA<th align=left>SOTTORDINI<th align=left>RIGHE<th align=left>PEZZI<th align=left>COLLI<th align=left>PRESE<th align=left>PESO DETERMINATO (Kg)<th align=left>PESO REALE (Kg)<tr>");

			$nTotSottordini=0;
			$nTotColli=0;
			$nTotLinee=0;
			$nTotPrese=0;
			$nTotPesoPre=0;
			$nTotPesoRea=0;
			$nTotCopie=0;
			$nColli[];

			for($nIndex=0;$nIndex<$db->num_rows();$nIndex++){
				if ($db->next_record()){
					printf("<tr><th align=left>%d<td>%d<td>%d<td>%d<td>%d<td>%d<td>%15.3f<td>%15.3f<tr>",$db->f(0),$db->f(1),$db->f(3),$db->f(4),$db->f(2),$db->f(4)+$db->f(2),$db->f(5),$db->f(6));
					$nTotSottordini+=$db->f(1);
					$nTotColli+=$db->f(2);
					$nTotLinee+=$db->f(3);
					$nTotPrese+=$db->f(4)+$db->f(2);
					$nColli[$db->f(0)]=$db->f(2);
					$nTotCopie+=$db->f(4);
					$nTotPesoPre+=$db->f(5);
					$nTotPesoRea+=$db->f(6);
				}
			}
			printf("<tr><th align=left>%s<td>%d<td>%d<td>%d<td>%d<td>%d<td>%15.3f<td>%15.3f<tr>",TOTALI,$nTotSottordini,$nTotLinee,$nTotCopie,$nTotColli,$nTotPrese,$nTotPesoPre,$nTotPesoRea);
			printf("</table>");
			$db->free();

			$nSettori[0]=8;
			$nSettori[1]=8;

			/*
			* DATI SUDDIVISI PER SETTORE
			*/
			for($nIndexIsola=1;$nIndexIsola<=2;$nIndexIsola++){
				/* controllo che l'isola abbia degli ordini altrimenti non visualizzo la tabella */
				if($nColli[$nIndexIsola]!=0){
					printf("<br><h2>ISOLA%d</h2>",$nIndexIsola);
					
					printf("<table COLS=2 WIDTH=\"80%%\"><tr><td>");

					printf("<table BORDER=1 COLS=4 WIDTH=\"60%%\" BGCOLOR=\"#44AA66\" >");
					printf("<tr><th align=left>SETTORE</th>");
					printf("<th align=left>RIGHE</th>");
					printf("<th align=left>PEZZI</th>");
					printf("<th align=left>PRESE</th></tr>");
					$szBuffer = sprintf("select distinct(substr(rig_stor.rpcdubi,2,2)),sum(rig_stor.rpqtspe),count(rig_stor.rpqtspe) from rig_stor,ord_stor where rig_stor.ordkey=ord_stor.ordkey and ord_stor.opcdisl=%d and ord_stor.opdteva>='%s' and ord_stor.opdteva<='%s' group by substr(rig_stor.rpcdubi,2,2);",$nIndexIsola,$szFromDate,$szToDate);
					$db -> query($szBuffer);
					for($nIndex=0;$nIndex<$nSettori[$nIndexIsola-1];$nIndex++){
						$db->next_record();
						printf("<tr><th align=left>%d</th><td>%d</td><td>%d</td><td>%d</td><tr>",$nIndex+1,$db->f(2),$db->f(1),($nIndex==0)?($db->f(1)+$nColli[$nIndexIsola]):($db->f(1)));
					}
					printf("</table>");

					printf("<td>");

					$db->free();

					/*
					* FORMATI UTILIZZATI PER ISOLA
					*/
					$szBuffer=sprintf("select distinct cptpfor, count(cptpfor) from col_stor
														 where cpdteva>='%s' and cpdteva<='%s' and substr(ordkey,12,1)='%d'
														 group by cptpfor;",$szFromDate,$szToDate,$nIndexIsola);
					$db -> query($szBuffer);
					if($db->next_record()){
						printf("<br><h3>IMBALLI UTILIZZATI</h3>");
						printf("<table BORDER=1 COLS=2 WIDTH=\"60%%\" BGCOLOR=\"#44AA66\">");
						printf("<tr><th align=left>FORMATO</th>");
						printf("<th align=left>QUANTITA'</th><tr>");
						do {
							printf("<tr><th align=left>%s</th><td>%d</td><tr>",$db->f(0),$db->f(1));
						} while($db->next_record());
						printf("<tr><th align=left>TOTALE</th><th align=left>%d</th>",$nColli[$nIndexIsola]);
						printf("</table>");
					}
					$db->free();

					printf("</table>");

				}
			}
		}
	}
?>

<p><h3 align="center"><a href="index.html">Menu Principale</a>

</body>
</html>
