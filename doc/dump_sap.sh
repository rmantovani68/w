#!/bin/bash
pg_dump -t articoli sap > sap_dump.txt
pg_dump -t col_prod -s sap >> sap_dump.txt
pg_dump -t col_stor -s sap >> sap_dump.txt
pg_dump -t eventi -s sap >> sap_dump.txt
pg_dump -t imballi sap >> sap_dump.txt
pg_dump -t operatori sap >> sap_dump.txt
pg_dump -t opeset sap >> sap_dump.txt
pg_dump -t ord_prod -s sap >> sap_dump.txt
pg_dump -t ord_stor -s sap >> sap_dump.txt
pg_dump -t pga_forms sap >> sap_dump.txt
pg_dump -t pga_layout sap >> sap_dump.txt
pg_dump -t pga_queries sap >> sap_dump.txt
pg_dump -t pga_reports sap >> sap_dump.txt
pg_dump -t pga_schema sap >> sap_dump.txt
pg_dump -t pga_scripts sap >> sap_dump.txt
pg_dump -t ric_art sap >> sap_dump.txt
pg_dump -t ric_ord -s sap >> sap_dump.txt
pg_dump -t ric_stor -s sap >> sap_dump.txt
pg_dump -t rig_prod -s sap >> sap_dump.txt
pg_dump -t rig_stor -s sap >> sap_dump.txt
pg_dump -t rig_tmp -s sap >> sap_dump.txt
pg_dump -t ubicazioni sap >> sap_dump.txt
