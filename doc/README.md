EASY PICKING 3.0 - VERSIONE SQL (POSTGRESQL)
--------------------------------------------

```
sudo apt install build-essential -y
sudo apt install git -y
sudo apt install libgtk2.0-dev -y 
sudo apt install libpq-dev -y 
sudo apt install postgresql-16-y 
sudo apt install postgresql-client -y
```

NOTE PER L'UTILIZZO DI POSTGRES

- Fare partire il servizio postgres/postmaster utilizzando linuxconf da root;

- Modificare il file pg_hba.conf (in /var/lib/pgsql) per accettare
  connessioni da host remoti sulla porta 5432;
  aggiunfere riga:
  ```
  host    all             all             127.0.0.1/0            trust
  ```
```
sudo su postgres
createuser -s roberto

createdb sap
sudo systemctl enable postgresql
sudo systemctl restart postgresql


```
