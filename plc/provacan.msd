CONFIGURATION WELLA_CONTROL

 VAR_GLOBAL
	OUT9_1 AT %QXIOS.0.0.9.0.128 : BOOL:= 0;
	OUT10_1 AT %QXIOS.0.0.10.0.128 : BOOL:= 0;
	OUT11_1 AT %QXIOS.0.0.11.0.128 : BOOL:= 0;
	OUT12_1 AT %QXIOS.0.0.12.0.128 : BOOL:= 0;
	OUT13_1 AT %QXIOS.0.0.13.0.128 : BOOL:= 0;
	OUT14_1 AT %QXIOS.0.0.14.0.128 : BOOL:= 0;
	OUT15_1 AT %QXIOS.0.0.15.0.128 : BOOL:= 0;
 END_VAR

 RESOURCE unica ON sola_cpu_presente

  PROGRAM G : WELLA

   (* Input *)
    (

  (* Output *)
   );
 END_RESOURCE
END_CONFIGURATION
