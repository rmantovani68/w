CONFIGURATION LABOCOS3_CONTROL

 VAR_GLOBAL
	B7 AT %IXIOS.0.0.16.0.37 : BOOL:= 0;  (* micro pareggiatore *)
	FC11 AT %IXIOS.0.0.16.0.25 : BOOL:= 0;
	FC12 AT %IXIOS.0.0.16.0.24 : BOOL:= 0;
	FC14 AT %IXIOS.0.0.16.0.18 : BOOL:= 0;
	FC15 AT %IXIOS.0.0.16.0.19 : BOOL:= 0;
	FC17 AT %IXIOS.0.0.16.0.26 : BOOL:= 0;
	FC18 AT %IXIOS.0.0.16.0.34 : BOOL:= 0;  (* paletta friz. fine linea *)
	FC21 AT %IXIOS.0.0.16.0.36 : BOOL:= 0;  (* fot. mtz. a monte dell' ultimo spintore *)
	FC3 AT %IXIOS.0.0.16.0.10 : BOOL:= 0;  (* fot. passo-passo da isola2 *)
	FC4 AT %IXIOS.0.0.16.0.11 : BOOL:= 0;
	FCA AT %IXIOS.0.0.16.0.17 : BOOL:= 0;
	FCB AT %IXIOS.0.0.16.0.33 : BOOL:= 0;  (* fot. pareggiatore *)
	FCB1 AT %IXIOS.0.0.16.0.9 : BOOL:= 0;
	FCB2 AT %IXIOS.0.0.16.0.8 : BOOL:= 0;
	FCB3 AT %IXIOS.0.0.16.0.97 : BOOL:= 0;
	FCB4 AT %IXIOS.0.0.16.0.96 : BOOL:= 0;
	FCB5 AT %IXIOS.0.0.16.0.91 : BOOL:= 0;
	FCB6 AT %IXIOS.0.0.16.0.90 : BOOL:= 0;
	FCC AT %IXIOS.0.0.16.0.104 : BOOL:= 0;  (* fotocellula spintore bilancia *)
	FCD AT %IXIOS.0.0.16.0.100 : BOOL:= 0;  (* fot. pieno accumulo bollini bianchi *)
	FCE AT %IXIOS.0.0.16.0.101 : BOOL:= 0;
	FCH AT %IXIOS.0.0.16.0.43 : BOOL:= 0;  (* fot. pieno *)
	FCI AT %IXIOS.0.0.16.0.40 : BOOL:= 0;
	FCL AT %IXIOS.0.0.16.0.75 : BOOL:= 0;  (* fot. pieno friz. MM *)
	FCM AT %IXIOS.0.0.16.0.74 : BOOL:= 0;
	FCN AT %IXIOS.0.0.16.0.80 : BOOL:= 0;  (* fot. di testa uscita nastratrice *)
	FCP AT %IXIOS.0.0.16.0.83 : BOOL:= 0;  (* fot. di testa traslaz. a monte del plz *)
	FCQ AT %IXIOS.0.0.16.0.81 : BOOL:= 0;
	FCS AT %IXIOS.0.0.16.0.87 : BOOL:= 0;
	FCT AT %IXIOS.0.0.16.0.89 : BOOL:= 0;  (* pieno friz. isola2 *)
	FCU AT %IXIOS.0.0.16.0.88 : BOOL:= 0;
	FCZ AT %IXIOS.0.0.16.0.35 : BOOL:= 0;
	FEM AT %IXIOS.0.0.16.0.99 : BOOL:= 0;
	INAL AT %IXIOS.0.0.16.0.106 : BOOL:= 0;  (* alimentazione sezionata presente *)
	PAIS2 AT %IXIOS.0.0.16.0.4 : BOOL:= 0;  (* PS ARRESTO ISOLA2 *)
	PAMAR2 AT %IXIOS.0.0.16.0.7 : BOOL:= 0;  (* pulsante arresto MARCIA2 al piano superiore *)
	PANE AT %IXIOS.0.0.16.0.57 : BOOL:= 0;  (* pulsante nastro elevatore ALTO *)
	PARNE AT %IXIOS.0.0.16.0.59 : BOOL:= 0;  (* pulsante arresto nastro elevatore *)
	PBNE AT %IXIOS.0.0.16.0.58 : BOOL:= 0;  (* pulsante nastro elevatore BASSO *)
	PEDALE AT %IXIOS.0.0.16.0.55 : BOOL:= 0;  (* per avanzamento collo *)
	PMI AT %IXIOS.0.0.16.0.105 : BOOL:= 0;  (* pulsante di marcia impianto *)
	PRESS AT %IXIOS.0.0.16.0.1 : BOOL:= 0;  (* pressostato aria *)
	PSA AT %IXIOS.0.0.16.0.2 : BOOL:= 0;
	PSO AT %IXIOS.0.0.16.0.5 : BOOL:= 0;
	PVIS2 AT %IXIOS.0.0.16.0.3 : BOOL:= 0;  (* PS MARCIA ISOLA2 *)
	PVMAR2 AT %IXIOS.0.0.16.0.6 : BOOL:= 0;  (* pulsante di avvio MARCIA2 al piano superiore *)
	SALTO AT %IXIOS.0.0.16.0.21 : BOOL:= 0;  (* switch nastro alto *)
	SBAP AT %IXIOS.0.0.16.0.68 : BOOL:= 0;  (* selettore avanzamento su bilancia *)
	SBASSO AT %IXIOS.0.0.16.0.16 : BOOL:= 0;  (* switch nastro basso *)
	SEA AT %IXIOS.0.0.16.0.103 : BOOL:= 0;  (* mc. pistone scarto avanti *)
	SEB AT %IXIOS.0.0.16.0.107 : BOOL:= 0;  (* selettore esclusione bilancia *)
	SEI AT %IXIOS.0.0.16.0.102 : BOOL:= 0;  (* mc. pistone scarto ind. *)
	SELALTO AT %IXIOS.0.0.16.0.64 : BOOL:= 0;  (* selettore nastro alto *)
	SELBASS AT %IXIOS.0.0.16.0.65 : BOOL:= 0;  (* selettore nastro basso *)
	SIA AT %IXIOS.0.0.16.0.84 : BOOL:= 0;
	SII AT %IXIOS.0.0.16.0.85 : BOOL:= 0;
	TERM AT %IXIOS.0.0.16.0.0 : BOOL:= 0;  (* ingresso catena dei termici *)
	ZC0101 AT %IXIOS.0.0.1.0.0 : BOOL:= 0;
	ZC0102 AT %IXIOS.0.0.1.0.1 : BOOL:= 0;
	ZC0103 AT %IXIOS.0.0.1.0.2 : BOOL:= 0;
	ZC0104 AT %IXIOS.0.0.1.0.3 : BOOL:= 0;
	ZC0105 AT %IXIOS.0.0.1.0.4 : BOOL:= 0;
	ZC0106 AT %IXIOS.0.0.1.0.5 : BOOL:= 0;
	ZC0107 AT %IXIOS.0.0.1.0.6 : BOOL:= 0;
	ZC0108 AT %IXIOS.0.0.1.0.7 : BOOL:= 0;
	ZC0109 AT %IXIOS.0.0.1.0.8 : BOOL:= 0;
	ZC0110 AT %IXIOS.0.0.1.0.9 : BOOL:= 0;
	ZC0111 AT %IXIOS.0.0.1.0.10 : BOOL:= 0;
	ZC0112 AT %IXIOS.0.0.1.0.11 : BOOL:= 0;
	ZC0201 AT %IXIOS.0.0.2.0.0 : BOOL:= 0;
	ZC0202 AT %IXIOS.0.0.2.0.1 : BOOL:= 0;
	ZC0203 AT %IXIOS.0.0.2.0.2 : BOOL:= 0;
	ZC0204 AT %IXIOS.0.0.2.0.3 : BOOL:= 0;
	ZC0205 AT %IXIOS.0.0.2.0.4 : BOOL:= 0;
	ZC0206 AT %IXIOS.0.0.2.0.5 : BOOL:= 0;
	ZC0207 AT %IXIOS.0.0.2.0.6 : BOOL:= 0;
	ZC0208 AT %IXIOS.0.0.2.0.7 : BOOL:= 0;
	ZC0209 AT %IXIOS.0.0.2.0.8 : BOOL:= 0;
	ZC0210 AT %IXIOS.0.0.2.0.9 : BOOL:= 0;
	ZC0211 AT %IXIOS.0.0.2.0.10 : BOOL:= 0;
	ZC0212 AT %IXIOS.0.0.2.0.11 : BOOL:= 0;
	ZC0301 AT %IXIOS.0.0.3.0.0 : BOOL:= 0;
	ZC0302 AT %IXIOS.0.0.3.0.1 : BOOL:= 0;
	ZC0303 AT %IXIOS.0.0.3.0.2 : BOOL:= 0;
	ZC0304 AT %IXIOS.0.0.3.0.3 : BOOL:= 0;
	ZC0305 AT %IXIOS.0.0.3.0.4 : BOOL:= 0;
	ZC0306 AT %IXIOS.0.0.3.0.5 : BOOL:= 0;
	ZC0307 AT %IXIOS.0.0.3.0.6 : BOOL:= 0;
	ZC0308 AT %IXIOS.0.0.3.0.7 : BOOL:= 0;
	ZC0309 AT %IXIOS.0.0.3.0.8 : BOOL:= 0;
	ZC0310 AT %IXIOS.0.0.3.0.9 : BOOL:= 0;
	ZC0311 AT %IXIOS.0.0.3.0.10 : BOOL:= 0;
	ZC0312 AT %IXIOS.0.0.3.0.11 : BOOL:= 0;
	ZC0401 AT %IXIOS.0.0.4.0.0 : BOOL:= 0;
	ZC0402 AT %IXIOS.0.0.4.0.1 : BOOL:= 0;
	ZC0403 AT %IXIOS.0.0.4.0.2 : BOOL:= 0;
	ZC0404 AT %IXIOS.0.0.4.0.3 : BOOL:= 0;
	ZC0405 AT %IXIOS.0.0.4.0.4 : BOOL:= 0;
	ZC0406 AT %IXIOS.0.0.4.0.5 : BOOL:= 0;
	ZC0407 AT %IXIOS.0.0.4.0.6 : BOOL:= 0;
	ZC0408 AT %IXIOS.0.0.4.0.7 : BOOL:= 0;
	ZC0409 AT %IXIOS.0.0.4.0.8 : BOOL:= 0;
	ZC0410 AT %IXIOS.0.0.4.0.9 : BOOL:= 0;
	ZC0411 AT %IXIOS.0.0.4.0.10 : BOOL:= 0;
	ZC0412 AT %IXIOS.0.0.4.0.11 : BOOL:= 0;
	ZC0501 AT %IXIOS.0.0.5.0.0 : BOOL:= 0;
	ZC0502 AT %IXIOS.0.0.5.0.1 : BOOL:= 0;
	ZC0503 AT %IXIOS.0.0.5.0.2 : BOOL:= 0;
	ZC0504 AT %IXIOS.0.0.5.0.3 : BOOL:= 0;
	ZC0505 AT %IXIOS.0.0.5.0.4 : BOOL:= 0;
	ZC0506 AT %IXIOS.0.0.5.0.5 : BOOL:= 0;
	ZC0507 AT %IXIOS.0.0.5.0.6 : BOOL:= 0;
	ZC0508 AT %IXIOS.0.0.5.0.7 : BOOL:= 0;
	ZC0509 AT %IXIOS.0.0.5.0.8 : BOOL:= 0;
	ZC0510 AT %IXIOS.0.0.5.0.9 : BOOL:= 0;
	ZC0511 AT %IXIOS.0.0.5.0.10 : BOOL:= 0;
	ZC0512 AT %IXIOS.0.0.5.0.11 : BOOL:= 0;
	ZC0601 AT %IXIOS.0.0.6.0.0 : BOOL:= 0;
	ZC0602 AT %IXIOS.0.0.6.0.1 : BOOL:= 0;
	ZC0603 AT %IXIOS.0.0.6.0.2 : BOOL:= 0;
	ZC0604 AT %IXIOS.0.0.6.0.3 : BOOL:= 0;
	ZC0605 AT %IXIOS.0.0.6.0.4 : BOOL:= 0;
	ZC0606 AT %IXIOS.0.0.6.0.5 : BOOL:= 0;
	ZC0607 AT %IXIOS.0.0.6.0.6 : BOOL:= 0;
	ZC0608 AT %IXIOS.0.0.6.0.7 : BOOL:= 0;
	ZC0609 AT %IXIOS.0.0.6.0.8 : BOOL:= 0;
	ZC0610 AT %IXIOS.0.0.6.0.9 : BOOL:= 0;
	ZC0611 AT %IXIOS.0.0.6.0.10 : BOOL:= 0;
	ZC0612 AT %IXIOS.0.0.6.0.11 : BOOL:= 0;
	ZC0701 AT %IXIOS.0.0.7.0.0 : BOOL:= 0;
	ZC0702 AT %IXIOS.0.0.7.0.1 : BOOL:= 0;
	ZC0703 AT %IXIOS.0.0.7.0.2 : BOOL:= 0;
	ZC0704 AT %IXIOS.0.0.7.0.3 : BOOL:= 0;
	ZC0705 AT %IXIOS.0.0.7.0.4 : BOOL:= 0;
	ZC0706 AT %IXIOS.0.0.7.0.5 : BOOL:= 0;
	ZC0707 AT %IXIOS.0.0.7.0.6 : BOOL:= 0;
	ZC0708 AT %IXIOS.0.0.7.0.7 : BOOL:= 0;
	ZC0709 AT %IXIOS.0.0.7.0.8 : BOOL:= 0;
	ZC0710 AT %IXIOS.0.0.7.0.9 : BOOL:= 0;
	ZC0711 AT %IXIOS.0.0.7.0.10 : BOOL:= 0;
	ZC0712 AT %IXIOS.0.0.7.0.11 : BOOL:= 0;
	ZC0801 AT %IXIOS.0.0.8.0.0 : BOOL:= 0;
	ZC0802 AT %IXIOS.0.0.8.0.1 : BOOL:= 0;
	ZC0803 AT %IXIOS.0.0.8.0.2 : BOOL:= 0;
	ZC0804 AT %IXIOS.0.0.8.0.3 : BOOL:= 0;
	ZC0805 AT %IXIOS.0.0.8.0.4 : BOOL:= 0;
	ZC0806 AT %IXIOS.0.0.8.0.5 : BOOL:= 0;
	ZC0807 AT %IXIOS.0.0.8.0.6 : BOOL:= 0;
	ZC0808 AT %IXIOS.0.0.8.0.7 : BOOL:= 0;
	ZC0809 AT %IXIOS.0.0.8.0.8 : BOOL:= 0;
	ZC0810 AT %IXIOS.0.0.8.0.9 : BOOL:= 0;
	ZC0811 AT %IXIOS.0.0.8.0.10 : BOOL:= 0;
	ZC0812 AT %IXIOS.0.0.8.0.11 : BOOL:= 0;
	ZC0901 AT %IXIOS.0.0.9.0.0 : BOOL:= 0;
	ZC0902 AT %IXIOS.0.0.9.0.1 : BOOL:= 0;
	ZC0903 AT %IXIOS.0.0.9.0.2 : BOOL:= 0;
	ZC0904 AT %IXIOS.0.0.9.0.3 : BOOL:= 0;
	ZC0905 AT %IXIOS.0.0.9.0.4 : BOOL:= 0;
	ZC0906 AT %IXIOS.0.0.9.0.5 : BOOL:= 0;
	ZC0907 AT %IXIOS.0.0.9.0.6 : BOOL:= 0;
	ZC0908 AT %IXIOS.0.0.9.0.7 : BOOL:= 0;
	ZC0909 AT %IXIOS.0.0.9.0.8 : BOOL:= 0;
	ZC0910 AT %IXIOS.0.0.9.0.9 : BOOL:= 0;
	ZC0911 AT %IXIOS.0.0.9.0.10 : BOOL:= 0;
	ZC0912 AT %IXIOS.0.0.9.0.11 : BOOL:= 0;
	ZC1001 AT %IXIOS.0.0.10.0.0 : BOOL:= 0;
	ZC1002 AT %IXIOS.0.0.10.0.1 : BOOL:= 0;
	ZC1003 AT %IXIOS.0.0.10.0.2 : BOOL:= 0;
	ZC1004 AT %IXIOS.0.0.10.0.3 : BOOL:= 0;
	ZC1005 AT %IXIOS.0.0.10.0.4 : BOOL:= 0;
	ZC1006 AT %IXIOS.0.0.10.0.5 : BOOL:= 0;
	ZC1007 AT %IXIOS.0.0.10.0.6 : BOOL:= 0;
	ZC1008 AT %IXIOS.0.0.10.0.7 : BOOL:= 0;
	ZC1009 AT %IXIOS.0.0.10.0.8 : BOOL:= 0;
	ZC1010 AT %IXIOS.0.0.10.0.9 : BOOL:= 0;
	ZC1011 AT %IXIOS.0.0.10.0.10 : BOOL:= 0;
	ZC1012 AT %IXIOS.0.0.10.0.11 : BOOL:= 0;
	ZC1013 AT %IXIOS.0.0.10.0.16 : BOOL:= 0;
	ZC1014 AT %IXIOS.0.0.10.0.17 : BOOL:= 0;
	ZC1015 AT %IXIOS.0.0.10.0.18 : BOOL:= 0;
	ZC1016 AT %IXIOS.0.0.10.0.19 : BOOL:= 0;
	ZC1017 AT %IXIOS.0.0.10.0.20 : BOOL:= 0;
	ZC1018 AT %IXIOS.0.0.10.0.21 : BOOL:= 0;
	ZC1019 AT %IXIOS.0.0.10.0.22 : BOOL:= 0;
	ZC1020 AT %IXIOS.0.0.10.0.23 : BOOL:= 0;
	ZC1021 AT %IXIOS.0.0.10.0.24 : BOOL:= 0;
	ZC1022 AT %IXIOS.0.0.10.0.25 : BOOL:= 0;
	ZC1023 AT %IXIOS.0.0.10.0.26 : BOOL:= 0;
	ZC1024 AT %IXIOS.0.0.10.0.27 : BOOL:= 0;
	ZC1101 AT %IXIOS.0.0.11.0.0 : BOOL:= 0;
	ZC1102 AT %IXIOS.0.0.11.0.1 : BOOL:= 0;
	ZC1103 AT %IXIOS.0.0.11.0.2 : BOOL:= 0;
	ZC1104 AT %IXIOS.0.0.11.0.3 : BOOL:= 0;
	ZC1105 AT %IXIOS.0.0.11.0.4 : BOOL:= 0;
	ZC1106 AT %IXIOS.0.0.11.0.5 : BOOL:= 0;
	ZC1107 AT %IXIOS.0.0.11.0.6 : BOOL:= 0;
	ZC1108 AT %IXIOS.0.0.11.0.7 : BOOL:= 0;
	ZC1109 AT %IXIOS.0.0.11.0.8 : BOOL:= 0;
	ZC1110 AT %IXIOS.0.0.11.0.9 : BOOL:= 0;
	ZC1111 AT %IXIOS.0.0.11.0.10 : BOOL:= 0;
	ZC1112 AT %IXIOS.0.0.11.0.11 : BOOL:= 0;
	ZC1113 AT %IXIOS.0.0.11.0.16 : BOOL:= 0;
	ZC1114 AT %IXIOS.0.0.11.0.17 : BOOL:= 0;
	ZC1115 AT %IXIOS.0.0.11.0.18 : BOOL:= 0;
	ZC1116 AT %IXIOS.0.0.11.0.19 : BOOL:= 0;
	ZC1117 AT %IXIOS.0.0.11.0.20 : BOOL:= 0;
	ZC1118 AT %IXIOS.0.0.11.0.21 : BOOL:= 0;
	ZC1119 AT %IXIOS.0.0.11.0.22 : BOOL:= 0;
	ZC1120 AT %IXIOS.0.0.11.0.23 : BOOL:= 0;
	ZC1121 AT %IXIOS.0.0.11.0.24 : BOOL:= 0;
	ZC1122 AT %IXIOS.0.0.11.0.25 : BOOL:= 0;
	ZC1123 AT %IXIOS.0.0.11.0.26 : BOOL:= 0;
	ZC1124 AT %IXIOS.0.0.11.0.27 : BOOL:= 0;
	ZC1201 AT %IXIOS.0.0.12.0.0 : BOOL:= 0;
	ZC1202 AT %IXIOS.0.0.12.0.1 : BOOL:= 0;
	ZC1203 AT %IXIOS.0.0.12.0.2 : BOOL:= 0;
	ZC1204 AT %IXIOS.0.0.12.0.3 : BOOL:= 0;
	ZC1205 AT %IXIOS.0.0.12.0.4 : BOOL:= 0;
	ZC1206 AT %IXIOS.0.0.12.0.5 : BOOL:= 0;
	ZC1207 AT %IXIOS.0.0.12.0.6 : BOOL:= 0;
	ZC1208 AT %IXIOS.0.0.12.0.7 : BOOL:= 0;
	ZC1209 AT %IXIOS.0.0.12.0.8 : BOOL:= 0;
	ZC1210 AT %IXIOS.0.0.12.0.9 : BOOL:= 0;
	ZC1211 AT %IXIOS.0.0.12.0.10 : BOOL:= 0;
	ZC1212 AT %IXIOS.0.0.12.0.11 : BOOL:= 0;
	ZC1301 AT %IXIOS.0.0.13.0.0 : BOOL:= 0;
	ZC1302 AT %IXIOS.0.0.13.0.1 : BOOL:= 0;
	ZC1303 AT %IXIOS.0.0.13.0.2 : BOOL:= 0;
	ZC1304 AT %IXIOS.0.0.13.0.3 : BOOL:= 0;
	ZC1305 AT %IXIOS.0.0.13.0.4 : BOOL:= 0;
	ZC1306 AT %IXIOS.0.0.13.0.5 : BOOL:= 0;
	ZC1307 AT %IXIOS.0.0.13.0.6 : BOOL:= 0;
	ZC1308 AT %IXIOS.0.0.13.0.7 : BOOL:= 0;
	ZC1309 AT %IXIOS.0.0.13.0.8 : BOOL:= 0;
	ZC1310 AT %IXIOS.0.0.13.0.9 : BOOL:= 0;
	ZC1311 AT %IXIOS.0.0.13.0.10 : BOOL:= 0;
	ZC1312 AT %IXIOS.0.0.13.0.11 : BOOL:= 0;
	ZC1401 AT %IXIOS.0.0.14.0.0 : BOOL:= 0;
	ZC1402 AT %IXIOS.0.0.14.0.1 : BOOL:= 0;
	ZC1403 AT %IXIOS.0.0.14.0.2 : BOOL:= 0;
	ZC1404 AT %IXIOS.0.0.14.0.3 : BOOL:= 0;
	ZC1405 AT %IXIOS.0.0.14.0.4 : BOOL:= 0;
	ZC1406 AT %IXIOS.0.0.14.0.5 : BOOL:= 0;
	ZC1407 AT %IXIOS.0.0.14.0.6 : BOOL:= 0;
	ZC1408 AT %IXIOS.0.0.14.0.7 : BOOL:= 0;
	ZC1409 AT %IXIOS.0.0.14.0.8 : BOOL:= 0;
	ZC1410 AT %IXIOS.0.0.14.0.9 : BOOL:= 0;
	ZC1411 AT %IXIOS.0.0.14.0.10 : BOOL:= 0;
	ZC1412 AT %IXIOS.0.0.14.0.11 : BOOL:= 0;
	ZC1501 AT %IXIOS.0.0.15.0.0 : BOOL:= 0;
	ZC1502 AT %IXIOS.0.0.15.0.1 : BOOL:= 0;
	ZC1503 AT %IXIOS.0.0.15.0.2 : BOOL:= 0;
	ZC1504 AT %IXIOS.0.0.15.0.3 : BOOL:= 0;
	ZC1505 AT %IXIOS.0.0.15.0.4 : BOOL:= 0;
	ZC1506 AT %IXIOS.0.0.15.0.5 : BOOL:= 0;
	ZC1507 AT %IXIOS.0.0.15.0.6 : BOOL:= 0;
	ZC1508 AT %IXIOS.0.0.15.0.7 : BOOL:= 0;
	ZC1509 AT %IXIOS.0.0.15.0.8 : BOOL:= 0;
	ZC1510 AT %IXIOS.0.0.15.0.9 : BOOL:= 0;
	ZC1511 AT %IXIOS.0.0.15.0.10 : BOOL:= 0;
	ZC1512 AT %IXIOS.0.0.15.0.11 : BOOL:= 0;
	C0101 AT %QXIOS.0.0.1.0.128 : BOOL:= 0;
	C0102 AT %QXIOS.0.0.1.0.129 : BOOL:= 0;
	C0103 AT %QXIOS.0.0.1.0.130 : BOOL:= 0;
	C0104 AT %QXIOS.0.0.1.0.131 : BOOL:= 0;
	C0105 AT %QXIOS.0.0.1.0.132 : BOOL:= 0;
	C0106 AT %QXIOS.0.0.1.0.133 : BOOL:= 0;
	C0107 AT %QXIOS.0.0.1.0.134 : BOOL:= 0;
	C0108 AT %QXIOS.0.0.1.0.135 : BOOL:= 0;
	C0109 AT %QXIOS.0.0.1.0.136 : BOOL:= 0;
	C0110 AT %QXIOS.0.0.1.0.137 : BOOL:= 0;
	C0111 AT %QXIOS.0.0.1.0.138 : BOOL:= 0;
	C0112 AT %QXIOS.0.0.1.0.139 : BOOL:= 0;
	C0201 AT %QXIOS.0.0.2.0.128 : BOOL:= 0;
	C0202 AT %QXIOS.0.0.2.0.129 : BOOL:= 0;
	C0203 AT %QXIOS.0.0.2.0.130 : BOOL:= 0;
	C0204 AT %QXIOS.0.0.2.0.131 : BOOL:= 0;
	C0205 AT %QXIOS.0.0.2.0.132 : BOOL:= 0;
	C0206 AT %QXIOS.0.0.2.0.133 : BOOL:= 0;
	C0207 AT %QXIOS.0.0.2.0.134 : BOOL:= 0;
	C0208 AT %QXIOS.0.0.2.0.135 : BOOL:= 0;
	C0209 AT %QXIOS.0.0.2.0.136 : BOOL:= 0;
	C0210 AT %QXIOS.0.0.2.0.137 : BOOL:= 0;
	C0211 AT %QXIOS.0.0.2.0.138 : BOOL:= 0;
	C0212 AT %QXIOS.0.0.2.0.139 : BOOL:= 0;
	C0301 AT %QXIOS.0.0.3.0.128 : BOOL:= 0;
	C0302 AT %QXIOS.0.0.3.0.129 : BOOL:= 0;
	C0303 AT %QXIOS.0.0.3.0.130 : BOOL:= 0;
	C0304 AT %QXIOS.0.0.3.0.131 : BOOL:= 0;
	C0305 AT %QXIOS.0.0.3.0.132 : BOOL:= 0;
	C0306 AT %QXIOS.0.0.3.0.133 : BOOL:= 0;
	C0307 AT %QXIOS.0.0.3.0.134 : BOOL:= 0;
	C0308 AT %QXIOS.0.0.3.0.135 : BOOL:= 0;
	C0309 AT %QXIOS.0.0.3.0.136 : BOOL:= 0;
	C0310 AT %QXIOS.0.0.3.0.137 : BOOL:= 0;
	C0311 AT %QXIOS.0.0.3.0.138 : BOOL:= 0;
	C0312 AT %QXIOS.0.0.3.0.139 : BOOL:= 0;
	C0401 AT %QXIOS.0.0.4.0.128 : BOOL:= 0;
	C0402 AT %QXIOS.0.0.4.0.129 : BOOL:= 0;
	C0403 AT %QXIOS.0.0.4.0.130 : BOOL:= 0;
	C0404 AT %QXIOS.0.0.4.0.131 : BOOL:= 0;
	C0405 AT %QXIOS.0.0.4.0.132 : BOOL:= 0;
	C0406 AT %QXIOS.0.0.4.0.133 : BOOL:= 0;
	C0407 AT %QXIOS.0.0.4.0.134 : BOOL:= 0;
	C0408 AT %QXIOS.0.0.4.0.135 : BOOL:= 0;
	C0409 AT %QXIOS.0.0.4.0.136 : BOOL:= 0;
	C0410 AT %QXIOS.0.0.4.0.137 : BOOL:= 0;
	C0411 AT %QXIOS.0.0.4.0.138 : BOOL:= 0;
	C0412 AT %QXIOS.0.0.4.0.139 : BOOL:= 0;
	C0501 AT %QXIOS.0.0.5.0.128 : BOOL:= 0;
	C0502 AT %QXIOS.0.0.5.0.129 : BOOL:= 0;
	C0503 AT %QXIOS.0.0.5.0.130 : BOOL:= 0;
	C0504 AT %QXIOS.0.0.5.0.131 : BOOL:= 0;
	C0505 AT %QXIOS.0.0.5.0.132 : BOOL:= 0;
	C0506 AT %QXIOS.0.0.5.0.133 : BOOL:= 0;
	C0507 AT %QXIOS.0.0.5.0.134 : BOOL:= 0;
	C0508 AT %QXIOS.0.0.5.0.135 : BOOL:= 0;
	C0509 AT %QXIOS.0.0.5.0.136 : BOOL:= 0;
	C0510 AT %QXIOS.0.0.5.0.137 : BOOL:= 0;
	C0511 AT %QXIOS.0.0.5.0.138 : BOOL:= 0;
	C0512 AT %QXIOS.0.0.5.0.139 : BOOL:= 0;
	C0601 AT %QXIOS.0.0.6.0.128 : BOOL:= 0;
	C0602 AT %QXIOS.0.0.6.0.129 : BOOL:= 0;
	C0603 AT %QXIOS.0.0.6.0.130 : BOOL:= 0;
	C0604 AT %QXIOS.0.0.6.0.131 : BOOL:= 0;
	C0605 AT %QXIOS.0.0.6.0.132 : BOOL:= 0;
	C0606 AT %QXIOS.0.0.6.0.133 : BOOL:= 0;
	C0607 AT %QXIOS.0.0.6.0.134 : BOOL:= 0;
	C0608 AT %QXIOS.0.0.6.0.135 : BOOL:= 0;
	C0609 AT %QXIOS.0.0.6.0.136 : BOOL:= 0;
	C0610 AT %QXIOS.0.0.6.0.137 : BOOL:= 0;
	C0611 AT %QXIOS.0.0.6.0.138 : BOOL:= 0;
	C0612 AT %QXIOS.0.0.6.0.139 : BOOL:= 0;
	C0701 AT %QXIOS.0.0.7.0.128 : BOOL:= 0;
	C0702 AT %QXIOS.0.0.7.0.129 : BOOL:= 0;
	C0703 AT %QXIOS.0.0.7.0.130 : BOOL:= 0;
	C0704 AT %QXIOS.0.0.7.0.131 : BOOL:= 0;
	C0705 AT %QXIOS.0.0.7.0.132 : BOOL:= 0;
	C0706 AT %QXIOS.0.0.7.0.133 : BOOL:= 0;
	C0707 AT %QXIOS.0.0.7.0.134 : BOOL:= 0;
	C0708 AT %QXIOS.0.0.7.0.135 : BOOL:= 0;
	C0709 AT %QXIOS.0.0.7.0.136 : BOOL:= 0;
	C0710 AT %QXIOS.0.0.7.0.137 : BOOL:= 0;
	C0711 AT %QXIOS.0.0.7.0.138 : BOOL:= 0;
	C0712 AT %QXIOS.0.0.7.0.139 : BOOL:= 0;
	C0801 AT %QXIOS.0.0.8.0.128 : BOOL:= 0;
	C0802 AT %QXIOS.0.0.8.0.129 : BOOL:= 0;
	C0803 AT %QXIOS.0.0.8.0.130 : BOOL:= 0;
	C0804 AT %QXIOS.0.0.8.0.131 : BOOL:= 0;
	C0805 AT %QXIOS.0.0.8.0.132 : BOOL:= 0;
	C0806 AT %QXIOS.0.0.8.0.133 : BOOL:= 0;
	C0807 AT %QXIOS.0.0.8.0.134 : BOOL:= 0;
	C0808 AT %QXIOS.0.0.8.0.135 : BOOL:= 0;
	C0809 AT %QXIOS.0.0.8.0.136 : BOOL:= 0;
	C0810 AT %QXIOS.0.0.8.0.137 : BOOL:= 0;
	C0811 AT %QXIOS.0.0.8.0.138 : BOOL:= 0;
	C0812 AT %QXIOS.0.0.8.0.139 : BOOL:= 0;
	C0901 AT %QXIOS.0.0.9.0.128 : BOOL:= 0;
	C0902 AT %QXIOS.0.0.9.0.129 : BOOL:= 0;
	C0903 AT %QXIOS.0.0.9.0.130 : BOOL:= 0;
	C0904 AT %QXIOS.0.0.9.0.131 : BOOL:= 0;
	C0905 AT %QXIOS.0.0.9.0.132 : BOOL:= 0;
	C0906 AT %QXIOS.0.0.9.0.133 : BOOL:= 0;
	C0907 AT %QXIOS.0.0.9.0.134 : BOOL:= 0;
	C0908 AT %QXIOS.0.0.9.0.135 : BOOL:= 0;
	C0909 AT %QXIOS.0.0.9.0.136 : BOOL:= 0;
	C0910 AT %QXIOS.0.0.9.0.137 : BOOL:= 0;
	C0911 AT %QXIOS.0.0.9.0.138 : BOOL:= 0;
	C0912 AT %QXIOS.0.0.9.0.139 : BOOL:= 0;
	C1001 AT %QXIOS.0.0.10.0.128 : BOOL:= 0;
	C1002 AT %QXIOS.0.0.10.0.129 : BOOL:= 0;
	C1003 AT %QXIOS.0.0.10.0.130 : BOOL:= 0;
	C1004 AT %QXIOS.0.0.10.0.131 : BOOL:= 0;
	C1005 AT %QXIOS.0.0.10.0.132 : BOOL:= 0;
	C1006 AT %QXIOS.0.0.10.0.133 : BOOL:= 0;
	C1007 AT %QXIOS.0.0.10.0.134 : BOOL:= 0;
	C1008 AT %QXIOS.0.0.10.0.135 : BOOL:= 0;
	C1009 AT %QXIOS.0.0.10.0.136 : BOOL:= 0;
	C1010 AT %QXIOS.0.0.10.0.137 : BOOL:= 0;
	C1011 AT %QXIOS.0.0.10.0.138 : BOOL:= 0;
	C1012 AT %QXIOS.0.0.10.0.139 : BOOL:= 0;
	C1013 AT %QXIOS.0.0.10.0.144 : BOOL:= 0;
	C1014 AT %QXIOS.0.0.10.0.145 : BOOL:= 0;
	C1015 AT %QXIOS.0.0.10.0.146 : BOOL:= 0;
	C1016 AT %QXIOS.0.0.10.0.147 : BOOL:= 0;
	C1017 AT %QXIOS.0.0.10.0.148 : BOOL:= 0;
	C1018 AT %QXIOS.0.0.10.0.149 : BOOL:= 0;
	C1019 AT %QXIOS.0.0.10.0.150 : BOOL:= 0;
	C1020 AT %QXIOS.0.0.10.0.151 : BOOL:= 0;
	C1021 AT %QXIOS.0.0.10.0.152 : BOOL:= 0;
	C1022 AT %QXIOS.0.0.10.0.153 : BOOL:= 0;
	C1023 AT %QXIOS.0.0.10.0.154 : BOOL:= 0;
	C1024 AT %QXIOS.0.0.10.0.155 : BOOL:= 0;
	C1101 AT %QXIOS.0.0.11.0.128 : BOOL:= 0;
	C1102 AT %QXIOS.0.0.11.0.129 : BOOL:= 0;
	C1103 AT %QXIOS.0.0.11.0.130 : BOOL:= 0;
	C1104 AT %QXIOS.0.0.11.0.131 : BOOL:= 0;
	C1105 AT %QXIOS.0.0.11.0.132 : BOOL:= 0;
	C1106 AT %QXIOS.0.0.11.0.133 : BOOL:= 0;
	C1107 AT %QXIOS.0.0.11.0.134 : BOOL:= 0;
	C1108 AT %QXIOS.0.0.11.0.135 : BOOL:= 0;
	C1109 AT %QXIOS.0.0.11.0.136 : BOOL:= 0;
	C1110 AT %QXIOS.0.0.11.0.137 : BOOL:= 0;
	C1111 AT %QXIOS.0.0.11.0.138 : BOOL:= 0;
	C1112 AT %QXIOS.0.0.11.0.139 : BOOL:= 0;
	C1113 AT %QXIOS.0.0.11.0.144 : BOOL:= 0;
	C1114 AT %QXIOS.0.0.11.0.145 : BOOL:= 0;
	C1115 AT %QXIOS.0.0.11.0.146 : BOOL:= 0;
	C1116 AT %QXIOS.0.0.11.0.147 : BOOL:= 0;
	C1117 AT %QXIOS.0.0.11.0.148 : BOOL:= 0;
	C1118 AT %QXIOS.0.0.11.0.149 : BOOL:= 0;
	C1119 AT %QXIOS.0.0.11.0.150 : BOOL:= 0;
	C1120 AT %QXIOS.0.0.11.0.151 : BOOL:= 0;
	C1121 AT %QXIOS.0.0.11.0.152 : BOOL:= 0;
	C1122 AT %QXIOS.0.0.11.0.153 : BOOL:= 0;
	C1123 AT %QXIOS.0.0.11.0.154 : BOOL:= 0;
	C1124 AT %QXIOS.0.0.11.0.155 : BOOL:= 0;
	C1201 AT %QXIOS.0.0.12.0.128 : BOOL:= 0;
	C1202 AT %QXIOS.0.0.12.0.129 : BOOL:= 0;
	C1203 AT %QXIOS.0.0.12.0.130 : BOOL:= 0;
	C1204 AT %QXIOS.0.0.12.0.131 : BOOL:= 0;
	C1205 AT %QXIOS.0.0.12.0.132 : BOOL:= 0;
	C1206 AT %QXIOS.0.0.12.0.133 : BOOL:= 0;
	C1207 AT %QXIOS.0.0.12.0.134 : BOOL:= 0;
	C1208 AT %QXIOS.0.0.12.0.135 : BOOL:= 0;
	C1209 AT %QXIOS.0.0.12.0.136 : BOOL:= 0;
	C1210 AT %QXIOS.0.0.12.0.137 : BOOL:= 0;
	C1211 AT %QXIOS.0.0.12.0.138 : BOOL:= 0;
	C1212 AT %QXIOS.0.0.12.0.139 : BOOL:= 0;
	C1301 AT %QXIOS.0.0.13.0.128 : BOOL:= 0;
	C1302 AT %QXIOS.0.0.13.0.129 : BOOL:= 0;
	C1303 AT %QXIOS.0.0.13.0.130 : BOOL:= 0;
	C1304 AT %QXIOS.0.0.13.0.131 : BOOL:= 0;
	C1305 AT %QXIOS.0.0.13.0.132 : BOOL:= 0;
	C1306 AT %QXIOS.0.0.13.0.133 : BOOL:= 0;
	C1307 AT %QXIOS.0.0.13.0.134 : BOOL:= 0;
	C1308 AT %QXIOS.0.0.13.0.135 : BOOL:= 0;
	C1309 AT %QXIOS.0.0.13.0.136 : BOOL:= 0;
	C1310 AT %QXIOS.0.0.13.0.137 : BOOL:= 0;
	C1311 AT %QXIOS.0.0.13.0.138 : BOOL:= 0;
	C1312 AT %QXIOS.0.0.13.0.139 : BOOL:= 0;
	C1401 AT %QXIOS.0.0.14.0.128 : BOOL:= 0;
	C1402 AT %QXIOS.0.0.14.0.129 : BOOL:= 0;
	C1403 AT %QXIOS.0.0.14.0.130 : BOOL:= 0;
	C1404 AT %QXIOS.0.0.14.0.131 : BOOL:= 0;
	C1405 AT %QXIOS.0.0.14.0.132 : BOOL:= 0;
	C1406 AT %QXIOS.0.0.14.0.133 : BOOL:= 0;
	C1407 AT %QXIOS.0.0.14.0.134 : BOOL:= 0;
	C1408 AT %QXIOS.0.0.14.0.135 : BOOL:= 0;
	C1409 AT %QXIOS.0.0.14.0.136 : BOOL:= 0;
	C1410 AT %QXIOS.0.0.14.0.137 : BOOL:= 0;
	C1411 AT %QXIOS.0.0.14.0.138 : BOOL:= 0;
	C1412 AT %QXIOS.0.0.14.0.139 : BOOL:= 0;
	C1501 AT %QXIOS.0.0.15.0.128 : BOOL:= 0;
	C1502 AT %QXIOS.0.0.15.0.129 : BOOL:= 0;
	C1503 AT %QXIOS.0.0.15.0.130 : BOOL:= 0;
	C1504 AT %QXIOS.0.0.15.0.131 : BOOL:= 0;
	C1505 AT %QXIOS.0.0.15.0.132 : BOOL:= 0;
	C1506 AT %QXIOS.0.0.15.0.133 : BOOL:= 0;
	C1507 AT %QXIOS.0.0.15.0.134 : BOOL:= 0;
	C1508 AT %QXIOS.0.0.15.0.135 : BOOL:= 0;
	C1509 AT %QXIOS.0.0.15.0.136 : BOOL:= 0;
	C1510 AT %QXIOS.0.0.15.0.137 : BOOL:= 0;
	C1511 AT %QXIOS.0.0.15.0.138 : BOOL:= 0;
	C1512 AT %QXIOS.0.0.15.0.139 : BOOL:= 0;
	C29 AT %QXIOS.0.0.16.0.202 : BOOL:= 0;  (* motore centralina idraulica ELEVATORE *)
	EV7 AT %QXIOS.0.0.16.0.147 : BOOL:= 0;
	EVA AT %QXIOS.0.0.16.0.145 : BOOL:= 0;
	EVB AT %QXIOS.0.0.16.0.146 : BOOL:= 0;
	EVC AT %QXIOS.0.0.16.0.148 : BOOL:= 0;  (* pareggiatore *)
	EVE AT %QXIOS.0.0.16.0.215 : BOOL:= 0;  (* spintore *)
	EVG AT %QXIOS.0.0.16.0.153 : BOOL:= 0;
	EVH AT %QXIOS.0.0.16.0.162 : BOOL:= 0;  (* elettr. catene alte *)
	EVI AT %QXIOS.0.0.16.0.163 : BOOL:= 0;
	EVL AT %QXIOS.0.0.16.0.161 : BOOL:= 0;
	EVM AT %QXIOS.0.0.16.0.138 : BOOL:= 0;
	EVN AT %QXIOS.0.0.16.0.139 : BOOL:= 0;
	EVNASB AT %QXIOS.0.0.16.0.216 : BOOL:= 0;  (* elettr. nastro basso *)
	EVO AT %QXIOS.0.0.16.0.144 : BOOL:= 0;
	EVP AT %QXIOS.0.0.16.0.136 : BOOL:= 0;
	KA5 AT %QXIOS.0.0.16.0.134 : BOOL:= 0;  (* RELE DI SGANCIO 110Vac *)
	LEM AT %QXIOS.0.0.16.0.130 : BOOL:= 0;
	LPM AT %QXIOS.0.0.16.0.128 : BOOL:= 0;  (* lampada termico scattato *)
	LPRESS AT %QXIOS.0.0.16.0.129 : BOOL:= 0;  (* lampada mancanza aria *)
	M10 AT %QXIOS.0.0.16.0.179 : BOOL:= 0;
	M11 AT %QXIOS.0.0.16.0.180 : BOOL:= 0;  (* mot. curva *)
	M13 AT %QXIOS.0.0.16.0.182 : BOOL:= 0;  (* dorsale corsie C e D *)
	M31 AT %QXIOS.0.0.16.0.208 : BOOL:= 0;
	M32 AT %QXIOS.0.0.16.0.209 : BOOL:= 0;
	M33 AT %QXIOS.0.0.16.0.210 : BOOL:= 0;
	M9 AT %QXIOS.0.0.16.0.178 : BOOL:= 0;
	MA AT %QXIOS.0.0.16.0.177 : BOOL:= 0;
	MB AT %QXIOS.0.0.16.0.211 : BOOL:= 0;
	MC AT %QXIOS.0.0.16.0.212 : BOOL:= 0;  (* motore nastro basculante *)
	ME AT %QXIOS.0.0.16.0.181 : BOOL:= 0;  (* mot. pareggiatore *)
	MF AT %QXIOS.0.0.16.0.192 : BOOL:= 0;  (* motore scarto *)
	MG AT %QXIOS.0.0.16.0.183 : BOOL:= 0;  (* mot. dei 2 espulsori bilancia *)
	MH AT %QXIOS.0.0.16.0.184 : BOOL:= 0;  (* mot. dopo frizionato con espulsore *)
	MI AT %QXIOS.0.0.16.0.186 : BOOL:= 0;  (* mot. friz. a monte nastratrice *)
	ML AT %QXIOS.0.0.16.0.194 : BOOL:= 0;  (* MOT. NASTRO INTRODUZ. NASTRATRICE *)
	MM AT %QXIOS.0.0.16.0.196 : BOOL:= 0;  (* mot. friz. a valle nastratrice *)
	MN AT %QXIOS.0.0.16.0.197 : BOOL:= 0;  (* mot. traverso *)
	MO AT %QXIOS.0.0.16.0.200 : BOOL:= 0;  (* mot. finale al plz *)
	MP AT %QXIOS.0.0.16.0.198 : BOOL:= 0;  (* mot. catene trslaz. *)
	MQ AT %QXIOS.0.0.16.0.203 : BOOL:= 0;  (* mtz. isola2 *)
	MR AT %QXIOS.0.0.16.0.166 : BOOL:= 0;  (* curva isola2 *)
	MS AT %QXIOS.0.0.16.0.167 : BOOL:= 0;
	MT AT %QXIOS.0.0.16.0.169 : BOOL:= 0;  (* mot. isola1 *)
	MU AT %QXIOS.0.0.16.0.176 : BOOL:= 0;  (* mot. isola1 *)
	MV AT %QXIOS.0.0.16.0.171 : BOOL:= 0;
	MW AT %QXIOS.0.0.16.0.168 : BOOL:= 0;  (* curva isola1 *)
	MZ AT %QXIOS.0.0.16.0.170 : BOOL:= 0;  (* mot isola1 *)
	R0101 AT %QXIOS.0.0.1.0.144 : BOOL:= 0;
	R0102 AT %QXIOS.0.0.1.0.145 : BOOL:= 0;
	R0103 AT %QXIOS.0.0.1.0.146 : BOOL:= 0;
	R0104 AT %QXIOS.0.0.1.0.147 : BOOL:= 0;
	R0105 AT %QXIOS.0.0.1.0.148 : BOOL:= 0;
	R0106 AT %QXIOS.0.0.1.0.149 : BOOL:= 0;
	R0107 AT %QXIOS.0.0.1.0.150 : BOOL:= 0;
	R0108 AT %QXIOS.0.0.1.0.151 : BOOL:= 0;
	R0109 AT %QXIOS.0.0.1.0.152 : BOOL:= 0;
	R0110 AT %QXIOS.0.0.1.0.153 : BOOL:= 0;
	R0111 AT %QXIOS.0.0.1.0.154 : BOOL:= 0;
	R0112 AT %QXIOS.0.0.1.0.155 : BOOL:= 0;
	R0113 AT %QXIOS.0.0.1.0.160 : BOOL:= 0;
	R0114 AT %QXIOS.0.0.1.0.161 : BOOL:= 0;
	R0115 AT %QXIOS.0.0.1.0.162 : BOOL:= 0;
	R0116 AT %QXIOS.0.0.1.0.163 : BOOL:= 0;
	R0117 AT %QXIOS.0.0.1.0.164 : BOOL:= 0;
	R0118 AT %QXIOS.0.0.1.0.165 : BOOL:= 0;
	R0119 AT %QXIOS.0.0.1.0.166 : BOOL:= 0;
	R0120 AT %QXIOS.0.0.1.0.167 : BOOL:= 0;
	R0121 AT %QXIOS.0.0.1.0.168 : BOOL:= 0;
	R0122 AT %QXIOS.0.0.1.0.169 : BOOL:= 0;
	R0123 AT %QXIOS.0.0.1.0.170 : BOOL:= 0;
	R0124 AT %QXIOS.0.0.1.0.171 : BOOL:= 0;
	R0201 AT %QXIOS.0.0.2.0.144 : BOOL:= 0;
	R0202 AT %QXIOS.0.0.2.0.145 : BOOL:= 0;
	R0203 AT %QXIOS.0.0.2.0.146 : BOOL:= 0;
	R0204 AT %QXIOS.0.0.2.0.147 : BOOL:= 0;
	R0205 AT %QXIOS.0.0.2.0.148 : BOOL:= 0;
	R0206 AT %QXIOS.0.0.2.0.149 : BOOL:= 0;
	R0207 AT %QXIOS.0.0.2.0.150 : BOOL:= 0;
	R0208 AT %QXIOS.0.0.2.0.151 : BOOL:= 0;
	R0209 AT %QXIOS.0.0.2.0.152 : BOOL:= 0;
	R0210 AT %QXIOS.0.0.2.0.153 : BOOL:= 0;
	R0211 AT %QXIOS.0.0.2.0.154 : BOOL:= 0;
	R0212 AT %QXIOS.0.0.2.0.155 : BOOL:= 0;
	R0213 AT %QXIOS.0.0.2.0.160 : BOOL:= 0;
	R0214 AT %QXIOS.0.0.2.0.161 : BOOL:= 0;
	R0215 AT %QXIOS.0.0.2.0.162 : BOOL:= 0;
	R0216 AT %QXIOS.0.0.2.0.163 : BOOL:= 0;
	R0217 AT %QXIOS.0.0.2.0.164 : BOOL:= 0;
	R0218 AT %QXIOS.0.0.2.0.165 : BOOL:= 0;
	R0219 AT %QXIOS.0.0.2.0.166 : BOOL:= 0;
	R0220 AT %QXIOS.0.0.2.0.167 : BOOL:= 0;
	R0221 AT %QXIOS.0.0.2.0.168 : BOOL:= 0;
	R0222 AT %QXIOS.0.0.2.0.169 : BOOL:= 0;
	R0223 AT %QXIOS.0.0.2.0.170 : BOOL:= 0;
	R0224 AT %QXIOS.0.0.2.0.171 : BOOL:= 0;
	R0301 AT %QXIOS.0.0.3.0.144 : BOOL:= 0;
	R0302 AT %QXIOS.0.0.3.0.145 : BOOL:= 0;
	R0303 AT %QXIOS.0.0.3.0.146 : BOOL:= 0;
	R0304 AT %QXIOS.0.0.3.0.147 : BOOL:= 0;
	R0305 AT %QXIOS.0.0.3.0.148 : BOOL:= 0;
	R0306 AT %QXIOS.0.0.3.0.149 : BOOL:= 0;
	R0307 AT %QXIOS.0.0.3.0.150 : BOOL:= 0;
	R0308 AT %QXIOS.0.0.3.0.151 : BOOL:= 0;
	R0309 AT %QXIOS.0.0.3.0.152 : BOOL:= 0;
	R0310 AT %QXIOS.0.0.3.0.153 : BOOL:= 0;
	R0311 AT %QXIOS.0.0.3.0.154 : BOOL:= 0;
	R0312 AT %QXIOS.0.0.3.0.155 : BOOL:= 0;
	R0313 AT %QXIOS.0.0.3.0.160 : BOOL:= 0;
	R0314 AT %QXIOS.0.0.3.0.161 : BOOL:= 0;
	R0315 AT %QXIOS.0.0.3.0.162 : BOOL:= 0;
	R0316 AT %QXIOS.0.0.3.0.163 : BOOL:= 0;
	R0317 AT %QXIOS.0.0.3.0.164 : BOOL:= 0;
	R0318 AT %QXIOS.0.0.3.0.165 : BOOL:= 0;
	R0319 AT %QXIOS.0.0.3.0.166 : BOOL:= 0;
	R0320 AT %QXIOS.0.0.3.0.167 : BOOL:= 0;
	R0321 AT %QXIOS.0.0.3.0.168 : BOOL:= 0;
	R0322 AT %QXIOS.0.0.3.0.169 : BOOL:= 0;
	R0323 AT %QXIOS.0.0.3.0.170 : BOOL:= 0;
	R0324 AT %QXIOS.0.0.3.0.171 : BOOL:= 0;
	R0401 AT %QXIOS.0.0.4.0.144 : BOOL:= 0;
	R0402 AT %QXIOS.0.0.4.0.145 : BOOL:= 0;
	R0403 AT %QXIOS.0.0.4.0.146 : BOOL:= 0;
	R0404 AT %QXIOS.0.0.4.0.147 : BOOL:= 0;
	R0405 AT %QXIOS.0.0.4.0.148 : BOOL:= 0;
	R0406 AT %QXIOS.0.0.4.0.149 : BOOL:= 0;
	R0407 AT %QXIOS.0.0.4.0.150 : BOOL:= 0;
	R0408 AT %QXIOS.0.0.4.0.151 : BOOL:= 0;
	R0409 AT %QXIOS.0.0.4.0.152 : BOOL:= 0;
	R0410 AT %QXIOS.0.0.4.0.153 : BOOL:= 0;
	R0411 AT %QXIOS.0.0.4.0.154 : BOOL:= 0;
	R0412 AT %QXIOS.0.0.4.0.155 : BOOL:= 0;
	R0413 AT %QXIOS.0.0.4.0.160 : BOOL:= 0;
	R0414 AT %QXIOS.0.0.4.0.161 : BOOL:= 0;
	R0415 AT %QXIOS.0.0.4.0.162 : BOOL:= 0;
	R0416 AT %QXIOS.0.0.4.0.163 : BOOL:= 0;
	R0417 AT %QXIOS.0.0.4.0.164 : BOOL:= 0;
	R0418 AT %QXIOS.0.0.4.0.165 : BOOL:= 0;
	R0419 AT %QXIOS.0.0.4.0.166 : BOOL:= 0;
	R0420 AT %QXIOS.0.0.4.0.167 : BOOL:= 0;
	R0421 AT %QXIOS.0.0.4.0.168 : BOOL:= 0;
	R0422 AT %QXIOS.0.0.4.0.169 : BOOL:= 0;
	R0423 AT %QXIOS.0.0.4.0.170 : BOOL:= 0;
	R0424 AT %QXIOS.0.0.4.0.171 : BOOL:= 0;
	R0501 AT %QXIOS.0.0.5.0.144 : BOOL:= 0;
	R0502 AT %QXIOS.0.0.5.0.145 : BOOL:= 0;
	R0503 AT %QXIOS.0.0.5.0.146 : BOOL:= 0;
	R0504 AT %QXIOS.0.0.5.0.147 : BOOL:= 0;
	R0505 AT %QXIOS.0.0.5.0.148 : BOOL:= 0;
	R0506 AT %QXIOS.0.0.5.0.149 : BOOL:= 0;
	R0507 AT %QXIOS.0.0.5.0.150 : BOOL:= 0;
	R0508 AT %QXIOS.0.0.5.0.151 : BOOL:= 0;
	R0509 AT %QXIOS.0.0.5.0.152 : BOOL:= 0;
	R0510 AT %QXIOS.0.0.5.0.153 : BOOL:= 0;
	R0511 AT %QXIOS.0.0.5.0.154 : BOOL:= 0;
	R0512 AT %QXIOS.0.0.5.0.155 : BOOL:= 0;
	R0601 AT %QXIOS.0.0.6.0.144 : BOOL:= 0;
	R0602 AT %QXIOS.0.0.6.0.145 : BOOL:= 0;
	R0603 AT %QXIOS.0.0.6.0.146 : BOOL:= 0;
	R0604 AT %QXIOS.0.0.6.0.147 : BOOL:= 0;
	R0605 AT %QXIOS.0.0.6.0.148 : BOOL:= 0;
	R0606 AT %QXIOS.0.0.6.0.149 : BOOL:= 0;
	R0607 AT %QXIOS.0.0.6.0.150 : BOOL:= 0;
	R0608 AT %QXIOS.0.0.6.0.151 : BOOL:= 0;
	R0609 AT %QXIOS.0.0.6.0.152 : BOOL:= 0;
	R0610 AT %QXIOS.0.0.6.0.153 : BOOL:= 0;
	R0611 AT %QXIOS.0.0.6.0.154 : BOOL:= 0;
	R0612 AT %QXIOS.0.0.6.0.155 : BOOL:= 0;
	R0701 AT %QXIOS.0.0.7.0.144 : BOOL:= 0;
	R0702 AT %QXIOS.0.0.7.0.145 : BOOL:= 0;
	R0703 AT %QXIOS.0.0.7.0.146 : BOOL:= 0;
	R0704 AT %QXIOS.0.0.7.0.147 : BOOL:= 0;
	R0705 AT %QXIOS.0.0.7.0.148 : BOOL:= 0;
	R0706 AT %QXIOS.0.0.7.0.149 : BOOL:= 0;
	R0707 AT %QXIOS.0.0.7.0.150 : BOOL:= 0;
	R0708 AT %QXIOS.0.0.7.0.151 : BOOL:= 0;
	R0709 AT %QXIOS.0.0.7.0.152 : BOOL:= 0;
	R0710 AT %QXIOS.0.0.7.0.153 : BOOL:= 0;
	R0711 AT %QXIOS.0.0.7.0.154 : BOOL:= 0;
	R0712 AT %QXIOS.0.0.7.0.155 : BOOL:= 0;
	R0713 AT %QXIOS.0.0.7.0.160 : BOOL:= 0;
	R0714 AT %QXIOS.0.0.7.0.161 : BOOL:= 0;
	R0715 AT %QXIOS.0.0.7.0.162 : BOOL:= 0;
	R0716 AT %QXIOS.0.0.7.0.163 : BOOL:= 0;
	R0717 AT %QXIOS.0.0.7.0.164 : BOOL:= 0;
	R0718 AT %QXIOS.0.0.7.0.165 : BOOL:= 0;
	R0719 AT %QXIOS.0.0.7.0.166 : BOOL:= 0;
	R0720 AT %QXIOS.0.0.7.0.167 : BOOL:= 0;
	R0721 AT %QXIOS.0.0.7.0.168 : BOOL:= 0;
	R0722 AT %QXIOS.0.0.7.0.169 : BOOL:= 0;
	R0723 AT %QXIOS.0.0.7.0.170 : BOOL:= 0;
	R0724 AT %QXIOS.0.0.7.0.171 : BOOL:= 0;
	R0801 AT %QXIOS.0.0.8.0.144 : BOOL:= 0;
	R0802 AT %QXIOS.0.0.8.0.145 : BOOL:= 0;
	R0803 AT %QXIOS.0.0.8.0.146 : BOOL:= 0;
	R0804 AT %QXIOS.0.0.8.0.147 : BOOL:= 0;
	R0805 AT %QXIOS.0.0.8.0.148 : BOOL:= 0;
	R0806 AT %QXIOS.0.0.8.0.149 : BOOL:= 0;
	R0807 AT %QXIOS.0.0.8.0.150 : BOOL:= 0;
	R0808 AT %QXIOS.0.0.8.0.151 : BOOL:= 0;
	R0809 AT %QXIOS.0.0.8.0.152 : BOOL:= 0;
	R0810 AT %QXIOS.0.0.8.0.153 : BOOL:= 0;
	R0811 AT %QXIOS.0.0.8.0.154 : BOOL:= 0;
	R0812 AT %QXIOS.0.0.8.0.155 : BOOL:= 0;
	R0813 AT %QXIOS.0.0.8.0.160 : BOOL:= 0;
	R0814 AT %QXIOS.0.0.8.0.161 : BOOL:= 0;
	R0815 AT %QXIOS.0.0.8.0.162 : BOOL:= 0;
	R0816 AT %QXIOS.0.0.8.0.163 : BOOL:= 0;
	R0817 AT %QXIOS.0.0.8.0.164 : BOOL:= 0;
	R0818 AT %QXIOS.0.0.8.0.165 : BOOL:= 0;
	R0819 AT %QXIOS.0.0.8.0.166 : BOOL:= 0;
	R0820 AT %QXIOS.0.0.8.0.167 : BOOL:= 0;
	R0821 AT %QXIOS.0.0.8.0.168 : BOOL:= 0;
	R0822 AT %QXIOS.0.0.8.0.169 : BOOL:= 0;
	R0823 AT %QXIOS.0.0.8.0.170 : BOOL:= 0;
	R0824 AT %QXIOS.0.0.8.0.171 : BOOL:= 0;
	R0901 AT %QXIOS.0.0.9.0.144 : BOOL:= 0;
	R0902 AT %QXIOS.0.0.9.0.145 : BOOL:= 0;
	R0903 AT %QXIOS.0.0.9.0.146 : BOOL:= 0;
	R0904 AT %QXIOS.0.0.9.0.147 : BOOL:= 0;
	R0905 AT %QXIOS.0.0.9.0.148 : BOOL:= 0;
	R0906 AT %QXIOS.0.0.9.0.149 : BOOL:= 0;
	R0907 AT %QXIOS.0.0.9.0.150 : BOOL:= 0;
	R0908 AT %QXIOS.0.0.9.0.151 : BOOL:= 0;
	R0909 AT %QXIOS.0.0.9.0.152 : BOOL:= 0;
	R0910 AT %QXIOS.0.0.9.0.153 : BOOL:= 0;
	R0911 AT %QXIOS.0.0.9.0.154 : BOOL:= 0;
	R0912 AT %QXIOS.0.0.9.0.155 : BOOL:= 0;
	R1001 AT %QXIOS.0.0.10.0.160 : BOOL:= 0;
	R1002 AT %QXIOS.0.0.10.0.161 : BOOL:= 0;
	R1003 AT %QXIOS.0.0.10.0.162 : BOOL:= 0;
	R1004 AT %QXIOS.0.0.10.0.163 : BOOL:= 0;
	R1005 AT %QXIOS.0.0.10.0.164 : BOOL:= 0;
	R1006 AT %QXIOS.0.0.10.0.165 : BOOL:= 0;
	R1007 AT %QXIOS.0.0.10.0.166 : BOOL:= 0;
	R1008 AT %QXIOS.0.0.10.0.167 : BOOL:= 0;
	R1009 AT %QXIOS.0.0.10.0.168 : BOOL:= 0;
	R1010 AT %QXIOS.0.0.10.0.169 : BOOL:= 0;
	R1011 AT %QXIOS.0.0.10.0.170 : BOOL:= 0;
	R1012 AT %QXIOS.0.0.10.0.171 : BOOL:= 0;
	R1013 AT %QXIOS.0.0.10.0.176 : BOOL:= 0;
	R1014 AT %QXIOS.0.0.10.0.177 : BOOL:= 0;
	R1015 AT %QXIOS.0.0.10.0.178 : BOOL:= 0;
	R1016 AT %QXIOS.0.0.10.0.179 : BOOL:= 0;
	R1017 AT %QXIOS.0.0.10.0.180 : BOOL:= 0;
	R1018 AT %QXIOS.0.0.10.0.181 : BOOL:= 0;
	R1019 AT %QXIOS.0.0.10.0.182 : BOOL:= 0;
	R1020 AT %QXIOS.0.0.10.0.183 : BOOL:= 0;
	R1021 AT %QXIOS.0.0.10.0.184 : BOOL:= 0;
	R1022 AT %QXIOS.0.0.10.0.185 : BOOL:= 0;
	R1023 AT %QXIOS.0.0.10.0.186 : BOOL:= 0;
	R1024 AT %QXIOS.0.0.10.0.187 : BOOL:= 0;
	R1101 AT %QXIOS.0.0.11.0.160 : BOOL:= 0;
	R1102 AT %QXIOS.0.0.11.0.161 : BOOL:= 0;
	R1103 AT %QXIOS.0.0.11.0.162 : BOOL:= 0;
	R1104 AT %QXIOS.0.0.11.0.163 : BOOL:= 0;
	R1105 AT %QXIOS.0.0.11.0.164 : BOOL:= 0;
	R1106 AT %QXIOS.0.0.11.0.165 : BOOL:= 0;
	R1107 AT %QXIOS.0.0.11.0.166 : BOOL:= 0;
	R1108 AT %QXIOS.0.0.11.0.167 : BOOL:= 0;
	R1109 AT %QXIOS.0.0.11.0.168 : BOOL:= 0;
	R1110 AT %QXIOS.0.0.11.0.169 : BOOL:= 0;
	R1111 AT %QXIOS.0.0.11.0.170 : BOOL:= 0;
	R1112 AT %QXIOS.0.0.11.0.171 : BOOL:= 0;
	R1113 AT %QXIOS.0.0.11.0.176 : BOOL:= 0;
	R1114 AT %QXIOS.0.0.11.0.177 : BOOL:= 0;
	R1115 AT %QXIOS.0.0.11.0.178 : BOOL:= 0;
	R1116 AT %QXIOS.0.0.11.0.179 : BOOL:= 0;
	R1117 AT %QXIOS.0.0.11.0.180 : BOOL:= 0;
	R1118 AT %QXIOS.0.0.11.0.181 : BOOL:= 0;
	R1119 AT %QXIOS.0.0.11.0.182 : BOOL:= 0;
	R1120 AT %QXIOS.0.0.11.0.183 : BOOL:= 0;
	R1121 AT %QXIOS.0.0.11.0.184 : BOOL:= 0;
	R1122 AT %QXIOS.0.0.11.0.185 : BOOL:= 0;
	R1123 AT %QXIOS.0.0.11.0.186 : BOOL:= 0;
	R1124 AT %QXIOS.0.0.11.0.187 : BOOL:= 0;
	R1201 AT %QXIOS.0.0.12.0.144 : BOOL:= 0;
	R1202 AT %QXIOS.0.0.12.0.145 : BOOL:= 0;
	R1203 AT %QXIOS.0.0.12.0.146 : BOOL:= 0;
	R1204 AT %QXIOS.0.0.12.0.147 : BOOL:= 0;
	R1205 AT %QXIOS.0.0.12.0.148 : BOOL:= 0;
	R1206 AT %QXIOS.0.0.12.0.149 : BOOL:= 0;
	R1207 AT %QXIOS.0.0.12.0.150 : BOOL:= 0;
	R1208 AT %QXIOS.0.0.12.0.151 : BOOL:= 0;
	R1209 AT %QXIOS.0.0.12.0.152 : BOOL:= 0;
	R1210 AT %QXIOS.0.0.12.0.153 : BOOL:= 0;
	R1211 AT %QXIOS.0.0.12.0.154 : BOOL:= 0;
	R1212 AT %QXIOS.0.0.12.0.155 : BOOL:= 0;
	R1301 AT %QXIOS.0.0.13.0.144 : BOOL:= 0;
	R1302 AT %QXIOS.0.0.13.0.145 : BOOL:= 0;
	R1303 AT %QXIOS.0.0.13.0.146 : BOOL:= 0;
	R1304 AT %QXIOS.0.0.13.0.147 : BOOL:= 0;
	R1305 AT %QXIOS.0.0.13.0.148 : BOOL:= 0;
	R1306 AT %QXIOS.0.0.13.0.149 : BOOL:= 0;
	R1307 AT %QXIOS.0.0.13.0.150 : BOOL:= 0;
	R1308 AT %QXIOS.0.0.13.0.151 : BOOL:= 0;
	R1309 AT %QXIOS.0.0.13.0.152 : BOOL:= 0;
	R1310 AT %QXIOS.0.0.13.0.153 : BOOL:= 0;
	R1311 AT %QXIOS.0.0.13.0.154 : BOOL:= 0;
	R1312 AT %QXIOS.0.0.13.0.155 : BOOL:= 0;
	R1401 AT %QXIOS.0.0.14.0.144 : BOOL:= 0;
	R1402 AT %QXIOS.0.0.14.0.145 : BOOL:= 0;
	R1403 AT %QXIOS.0.0.14.0.146 : BOOL:= 0;
	R1404 AT %QXIOS.0.0.14.0.147 : BOOL:= 0;
	R1405 AT %QXIOS.0.0.14.0.148 : BOOL:= 0;
	R1406 AT %QXIOS.0.0.14.0.149 : BOOL:= 0;
	R1407 AT %QXIOS.0.0.14.0.150 : BOOL:= 0;
	R1408 AT %QXIOS.0.0.14.0.151 : BOOL:= 0;
	R1409 AT %QXIOS.0.0.14.0.152 : BOOL:= 0;
	R1410 AT %QXIOS.0.0.14.0.153 : BOOL:= 0;
	R1411 AT %QXIOS.0.0.14.0.154 : BOOL:= 0;
	R1412 AT %QXIOS.0.0.14.0.155 : BOOL:= 0;
	R1501 AT %QXIOS.0.0.15.0.144 : BOOL:= 0;
	R1502 AT %QXIOS.0.0.15.0.145 : BOOL:= 0;
	R1503 AT %QXIOS.0.0.15.0.146 : BOOL:= 0;
	R1504 AT %QXIOS.0.0.15.0.147 : BOOL:= 0;
	R1505 AT %QXIOS.0.0.15.0.148 : BOOL:= 0;
	R1506 AT %QXIOS.0.0.15.0.149 : BOOL:= 0;
	R1507 AT %QXIOS.0.0.15.0.150 : BOOL:= 0;
	R1508 AT %QXIOS.0.0.15.0.151 : BOOL:= 0;
	R1509 AT %QXIOS.0.0.15.0.152 : BOOL:= 0;
	R1510 AT %QXIOS.0.0.15.0.153 : BOOL:= 0;
	R1511 AT %QXIOS.0.0.15.0.154 : BOOL:= 0;
	R1512 AT %QXIOS.0.0.15.0.155 : BOOL:= 0;
	SIRENA AT %QXIOS.0.0.16.0.135 : BOOL:= 0;
 END_VAR

 RESOURCE unica ON sola_cpu_presente

  PROGRAM G : LABOCOS3

   (* Input *)
    (

  (* Output *)
   );
 END_RESOURCE
END_CONFIGURATION
