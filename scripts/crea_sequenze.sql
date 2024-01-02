\connect - roberto
CREATE SEQUENCE "progcollo" start 1000000 increment 1 maxvalue 2000000 minvalue 999999  cache 1 ;
SELECT nextval ('"progcollo"');
\connect - roberto
CREATE SEQUENCE "sequence_anno" start 2000 increment 1 maxvalue 4000 minvalue 2000  cache 1 ;
SELECT nextval ('"sequence_anno"');
