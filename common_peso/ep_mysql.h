/*--------------------------------------------*/
/* POSTGRESQL Functions                       */
/*--------------------------------------------*/

DBExecStatusType DBresultStatus(DBresult *DBRes);
char *DBcmdTuples(DBresult *DBRes);
int DBnfields(DBresult *DBRes);
int DBntuples(DBresult *DBRes);
DBFieldType DBftype(DBresult *DBRes,int nField);
unsigned int DBgetlength(DBresult *DBRes,int nTuple,int nField);
char *DBfname(DBresult *DBRes,int nField);
char *DBgetvalue(DBresult *DBRes,int nTuple,int nField);
char *DBresultErrorMessage(DBresult *DBRes);
DBconn *pgsql_connect_db(char *szHost,char *szPort,char *szDataBase);
void pgsql_disconnect_db(DBconn *DB);
DBresult *pgsql_exec(DBconn *DB,char *szQuery);
void pgsql_free_result(DBresult *DBRes);
