CONFIGURATION WELLAPLC_CONTROL

 VAR_GLOBAL
	FCPLZ AT %IXNULL : BOOL:= 0;  (* fotocellula presenza collo in PLZ *)
	FCSC AT %IXNULL : BOOL:= 0;  (* fotocellula di abilitazione scanner e presenza collo *)
	PSCP AT %IXNULL : BOOL:= 0;  (* pulsante cancellazione CPLZ dopo mancata risposta *)
	SLNOPLZ AT %IXNULL : BOOL:= 0;  (* selettore NO forzatura al PLZ *)
	SLPLZ AT %IXNULL : BOOL:= 0;  (* selett. abilitazione forzatura al PLZ *)
	SLRESET AT %IXNULL : BOOL:= 0;  (* comando di svuotamento di entrambe le code *)
	FDPLZ AT %QXNULL : BOOL:= 0;  (* comando al PLZ se palettizzare (=1 DENTRO) o no (=0 FUORI) *)
	LGPLZ AT %QXNULL : BOOL:= 0;  (* lampada gialla; code vuote *)
	LRPLZ AT %QXNULL : BOOL:= 0;  (* lampada rossa ; continua se code piene; lampeggiante per time-out scanner *)
	LVPLZ AT %QXNULL : BOOL:= 0;  (* lampada verde ; continua se abilitata la forzatura; lampeggiante se CPLZ attivo *)
 END_VAR

 RESOURCE unica ON sola_cpu_presente

  PROGRAM G : WELLAPLC

   (* Input *)
    (

  (* Output *)
   );
 END_RESOURCE
END_CONFIGURATION
