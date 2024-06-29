CREATE TABLE missing_pbl_mapping
(
    ubicazione text
);

\copy missing_pbl_mapping FROM '/home/roberto/Downloads/missingUbicazioneMappa.csv' DELIMITER ',' CSV
