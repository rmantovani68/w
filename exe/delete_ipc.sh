# ----------------------------------------
# Cancellazione dei processi e dei dati
# IPC utlizzati nell'impianto wella
# ----------------------------------------
echo 'Cancellazione dati in corso ...'
ps -ax  | while read a b c d e; do case "$e" in *exe/main*) echo kill -15 $a; kill -15 $a;;  esac; done 
ps -ax  | while read a b c d e; do case "$e" in *exe/receive*) echo kill -15 $a; kill -15 $a;;  esac; done 
ps -ax  | while read a b c d e; do case "$e" in *exe/volum*) echo kill -15 $a; kill -15 $a;;  esac; done 
ps -ax  | while read a b c d e; do case "$e" in *exe/settori*) echo kill -15 $a; kill -15 $a;;  esac; done 
ps -ax  | while read a b c d e; do case "$e" in *exe/ios*) echo kill -15 $a; kill -15 $a;;  esac; done 
ps -ax  | while read a b c d e; do case "$e" in *exe/printlabel*) echo kill -15 $a; kill -15 $a;;  esac; done 
ps -ax  | while read a b c d e; do case "$e" in *exe/peso*) echo kill -15 $a; kill -15 $a;;  esac; done 
ps -ax  | while read a b c d e; do case "$e" in *exe/bilancia*) echo kill -15 $a; kill -15 $a;;  esac; done 
ps -ax  | while read a b c d e; do case "$e" in *exe/barcode*) echo kill -15 $a; kill -15 $a;;  esac; done 
ipcs -q | while read a b c; do case "$a" in 0x000001[01][0-9a-f]) echo ipcrm msg $b; ipcrm msg $b ; ;;  esac; done 
ipcs -m | while read a b c; do case "$a" in 0x000001[01][0-9a-f]) echo ipcrm shm $b; ipcrm shm $b ; ;;  esac; done 
echo 'Cancellazione dati Terminata'
