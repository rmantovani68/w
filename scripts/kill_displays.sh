# ----------------------------------------
# Cancellazione dei processi e dei dati
# IPC utlizzati
# ----------------------------------------
echo 'Cancellazione dati in corso ...'
ps ax  | while read a b c d e; do case "$e" in *display*)       echo SIGTERM $a; kill -SIGTERM $a;;  esac; done
