CREATE TABLE pbl_mapping
(
    Materiale text,
    Div text,
    Mag text,
    T text,
    Testo_breve_materiale text,
    Cat text,
    Ubicazione text,
    Class text,
    N_cert text,
    Stock_disponibile text,
    UMO text,
    Data_EM text,
    Ult_mov text,
    Scad_DMC text,
    PBL_Location text
);

-- COPY pbl_mapping FROM '/home/roberto/Downloads/PBL_Mapping.csv' WITH (FORMAT csv);
\copy pbl_mapping FROM '/home/roberto/Downloads/PBL_Mapping.csv' DELIMITER ',' CSV