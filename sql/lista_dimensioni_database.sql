SELECT pg_database.datname, 
pg_database_size(pg_database.datname), 
pg_size_pretty(pg_database_size(pg_database.datname)) 
FROM pg_database ORDER by 2 DESC;
