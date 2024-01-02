CONFIGURATION LABOCOSR_CONTROL

 VAR_GLOBAL
	FC18 AT %IXNULL : BOOL:= 0;  (* paletta friz. fine linea *)
	FC21 AT %IXNULL : BOOL:= 0;  (* fot. mtz. a monte dell' ultimo spintore *)
	FCC AT %IXNULL : BOOL:= 0;  (* fotocellula spintore bilancia *)
	FCD AT %IXNULL : BOOL:= 0;  (* fot. pieno accumulo bollini bianchi *)
	FCG AT %IXNULL : BOOL:= 0;  (* fot. di pieno dopo espulsore *)
	FCH AT %IXNULL : BOOL:= 0;  (* fot. pieno *)
	INAL AT %IXIOS.0.0.1.0.54 : BOOL:= 0;  (* alimentazione sezionata presente *)
	PMI3 AT %IXNULL : BOOL:= 0;
	PSFMB AT %IXNULL : BOOL:= 0;  (* pulsante scarto forzato manuale bilancia *)
	SEA AT %IXNULL : BOOL:= 0;  (* mc. pistone scarto avanti *)
	SEB AT %IXNULL : BOOL:= 0;  (* selettore esclusione bilancia *)
	SEI AT %IXNULL : BOOL:= 0;  (* mc. pistone scarto ind. *)
	TERM AT %IXNULL : BOOL:= 0;  (* ingresso catena dei termici *)
	EVE AT %QXIOS.0.0.1.0.182 : BOOL:= 0;  (* spintore *)
	LALLM13 AT %QXNULL : BOOL:= 0;  (* lampada allarme risposta da PC *)
	LMI AT %QXNULL : BOOL:= 0;  (* lampada MARCIA inserita *)
	LPM AT %QXNULL : BOOL:= 0;  (* lampada termico scattato *)
	M13 AT %QXIOS.0.0.1.0.144 : BOOL:= 0;  (* dorsale corsie C e D *)
	MG AT %QXIOS.0.0.1.0.154 : BOOL:= 0;  (* mot. dei 2 espulsori bilancia *)
	MH AT %QXNULL : BOOL:= 0;  (* mot. dopo frizionato con espulsore *)
	MI AT %QXmot.friz. a monte nastratrice : BOOL:= 0;
	ML AT %QXNULL : BOOL:= 0;  (* MOT. NASTRO INTRODUZ. NASTRATRICE *)
 END_VAR

 RESOURCE unica ON sola_cpu_presente

  PROGRAM G : LABOCOSR

   (* Input *)
    (

  (* Output *)
   );
 END_RESOURCE
END_CONFIGURATION
