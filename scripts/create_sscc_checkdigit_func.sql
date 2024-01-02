CREATE or replace FUNCTION sscc_checkdigit(text) RETURNS bigint AS $$
select 
(10 - 
((
	-- posizioni dispari * 3
	(
		SELECT
		SUM(digit * 3)
		FROM (
			SELECT
			ROW_NUMBER() over () AS pos,
			digit
			FROM (
				SELECT REGEXP_SPLIT_TO_TABLE( $1, ''::TEXT)::INT AS digit
			) AS split
		) AS sub
		WHERE pos <= LENGTH($1) 
		AND pos % 2 = LENGTH($1) % 2
	)
+
	-- posizioni pari * 1
 	(
 		SELECT
 		SUM(digit)
 		FROM (
 			SELECT
 			ROW_NUMBER() over () AS pos,
 			digit
 			FROM (
 				SELECT REGEXP_SPLIT_TO_TABLE( $1, ''::TEXT)::INT AS digit
 			) AS split
 		) AS sub
 		WHERE pos <= LENGTH($1) 
 		AND pos % 2 = (LENGTH($1) - 1) % 2
 	)
) 
% 10)) % 10
$$ LANGUAGE SQL;


