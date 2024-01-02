#!/bin/bash
#
# database_dumpall.sh
# Versione: 1.0
# Autore: Luca Guerrini
#

DB_HOST="localhost"
DB_NAME=""
BACKUP_DIR="$HOME"
LOG_FILE=""
FD_LOG_FILE=7  # File descriptor del file di log
E_XCD=66       # Errore di mancato cambio directory
NOARGS=0
E_BADARGS=65   # Errore di argomenti sbagliati
DUMPALL='false'
WEEK_DUMP='false'
FTP='false'
SCP='false'
NETBIOS='false'
VERBOSE=""
DUMP_DATE=`date`
DAY_OF_WEEK=`date +%u`



function print_help()
{
	echo
	echo "Usage: `basename $0` [options] <database-name>"
	echo
	echo "List of options:"
	echo "-h <host-name>: name of the host (default localhost)"
	echo "-d <dir-name>: name of the directory for the backup"
	echo "-w : save dumps for a week"
	echo "-a : dump all the database"
	echo "-v : verbose"
	echo "-ftp : activate ftp copy of dump file"
	echo "-ftphost : ftp host"
	echo "-ftpuser : ftp user"
	echo "-ftppassword : ftp password"
	echo "-scp : activate scp copy of dump file"
	echo "-scphost : scp host"
	echo "-scpuser : scp user"

	echo
}

function log() # Scrive ora e data nel log_file.
{
	echo "$(date) $*" >&${FD_LOG_FILE}     # Questo dovrebbe appendere la data al file.
}

if [ $# = $NOARGS ]
then
	print_help
	exit $E_BADARGS
fi

while [ $# != 0 ]
do
	case "$1" in
		-ftp) FTP='true' ;;
		-ftpuser) FTP_USER=$2 ; shift ;;
		-ftppassword) FTP_PASSWORD=$2 ; shift ;;
		-ftphost) FTP_HOST=$2 ; shift ;;
		-scp) SCP='true' ;;
		-scpuser) SCP_USER=$2 ; shift ;;
		-scphost) SCP_HOST=$2 ; shift ;;
		-netbios) NETBIOS='true' ;;
		-netbiosname) NETBIOS_NAME=$2 ; shift ;;
		-h) DB_HOST=$2 ; shift ;;
		-d) BACKUP_DIR=$2 ; shift ;;
		-w) WEEK_DUMP='true' ;;
		-a) DUMPALL='true' ;;
		-v) VERBOSE='--verbose' ;;
		-*) ;;
		*)  DB_NAME=$1 ;;
	esac
shift
done
echo $DB_NAME

LOG_FILE="$BACKUP_DIR/database_dumpall.log"

if $DUMPALL
then
	DUMP_NAME="dumpall.dump.gz"
else
	if [[ "$DB_NAME" = "" ]]
	then
		print_help
	else
		DUMP_NAME="$DB_NAME.dump.gz"
	fi
fi

if cat /dev/null > $LOG_FILE
then
	exec 7>>$LOG_FILE # Aggancio il file di log al suo fd
	log "INIZIO BACKUP DEL DATABASE DA HOST $DB_HOST"
else
	echo "ERR: Impossibile creare il file di log"
	exit $E_XCD
fi

if cd "$BACKUP_DIR" 2>>/dev/null
then
	log "Cambiata directory in $BACKUP_DIR."
else
	log "ERR: Impossibile cambiare la directory in $BACKUP_DIR." 
	exit $E_XCD
fi

if $WEEK_DUMP
then
	log "Cancello i file relativi a 7 giorni fa"
	rm $DUMP_NAME.$DAY_OF_WEEK
else
	log "Cancello i file relativi ai vecchi dump"
	rm $DUMP_NAME.*
	rm $DUMP_NAME
fi


log "Inizio del dump"
if $DUMPALL
then
	echo
	pg_dumpall $VERBOSE --no-privileges  -h $DB_HOST 2>&${FD_LOG_FILE} | gzip -c > $DUMP_NAME.$DAY_OF_WEEK
else
	# Tengo la traccia dell'ultimo item di reg_mov
	#echo 'drop table reg_mov_ultimo_item;' | psql -h $DB_HOST $DB_NAME
	#echo 'create table reg_mov_ultimo_item as select max(rmprmov) as "rmprmov"  from reg_mov;' | psql -h $DB_HOST $DB_NAME
	pg_dump $VERBOSE --no-privileges --no-owner -i -h $DB_HOST $DB_NAME 2>&${FD_LOG_FILE} | gzip -c > $DUMP_NAME.$DAY_OF_WEEK
fi
log "Fine del dump"
echo "FINE BACKUP DEL DATABASE DA HOST $DB_HOST" >&${FD_LOG_FILE}


if $NETBIOS
then
	log "Inizio del rilevamento netbios"
	NETBIOS_ADDRESS=$(nmblookup $NETBIOS_NAME|tail -n 1|sed 's/ .*$//');echo $NETBIOS_ADDRESS
	echo "NETBIOS_ADDRESS="$NETBIOS_ADDRESS
	FTP_HOST=$NETBIOS_ADDRESS
	echo "Indirizzo Netbios: " $NETBIOS_ADDRESS >&${FD_LOG_FILE};
	echo "Indirizzo Ftp: " $FTP_HOST >&${FD_LOG_FILE};
fi


# rm 03-04-2008 : invio file via ftp a host specifico
if $FTP
then
	log "Inizio della spedizione con ncftp"
	echo "ncftpput -u $FTP_USER -p $FTP_PASSWORD $FTP_HOST . /home/roberto/$DUMP_NAME.$DAY_OF_WEEK" >&${FD_LOG_FILE}
	ncftpput -u $FTP_USER -p $FTP_PASSWORD $FTP_HOST . /home/roberto/$DUMP_NAME.$DAY_OF_WEEK 2>&${FD_LOG_FILE}
	echo "Fine della spedizione con ncftp del file" $DUMP_NAME.$DAY_OF_WEEK >&${FD_LOG_FILE}
fi

# lg 19-07-2016 : invio file via scp a host specifico
if $SCP
then
	log "Inizio della spedizione con scp"
	echo "scp /home/picking/$DUMP_NAME.$DAY_OF_WEEK picking@wella2:/home/picking" >&${FD_LOG_FILE}
	/usr/bin/scp /home/picking/$DUMP_NAME.$DAY_OF_WEEK picking@wella2:/home/picking 2>&${FD_LOG_FILE}
	echo "Fine della spedizione con scp del file" $DUMP_NAME.$DAY_OF_WEEK >&${FD_LOG_FILE}
fi



exit 0
