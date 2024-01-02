/*--------------------------------------------*/
/* POSTGRESQL Functions                       */
/*--------------------------------------------*/

DBconn *pgsql_connect_db(char *szHost,char *szPort,char *szDataBase);
void pgsql_disconnect_db(DBconn *DB);
DBresult *pgsql_exec(DBconn *DB,char *szQuery);
void pgsql_free_result(DBresult *DBRes);
