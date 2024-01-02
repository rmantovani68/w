# ----------------------------------------
# Cancellazione dei processi e dei dati
# IPC utlizzati
# ----------------------------------------
echo 'Cancellazione dati in corso ...'
ps ax  | while read a b c d e; do case "$e" in *exe/main*)       echo SIGTERM $a; kill -SIGTERM $a;;  esac; done
ps ax  | while read a b c d e; do case "$e" in *exe/settori*)    echo SIGTERM $a; kill -SIGTERM $a;;  esac; done
ps ax  | while read a b c d e; do case "$e" in *exe/ios*)        echo SIGTERM $a; kill -SIGTERM $a;;  esac; done
ps ax  | while read a b c d e; do case "$e" in *exe/volum*)      echo SIGTERM $a; kill -SIGTERM $a;;  esac; done
ps ax  | while read a b c d e; do case "$e" in *exe/printlabel*) echo SIGTERM $a; kill -SIGTERM $a;;  esac; done
ps ax  | while read a b c d e; do case "$e" in *exe/sock*)       echo SIGTERM $a; kill -SIGTERM $a;;  esac; done
ipcs -q | while read a b c; do case "$a" in 0x00000100) echo ipcrm msg $b; ipcrm msg $b ; ;;  esac; done 
ipcs -m | while read a b c; do case "$a" in 0x00000100) echo ipcrm shm $b; ipcrm shm $b ; ;;  esac; done 
ipcs -q | while read a b c; do case "$a" in 0x00000101) echo ipcrm msg $b; ipcrm msg $b ; ;;  esac; done 
ipcs -m | while read a b c; do case "$a" in 0x00000101) echo ipcrm shm $b; ipcrm shm $b ; ;;  esac; done 
ipcs -q | while read a b c; do case "$a" in 0x00000102) echo ipcrm msg $b; ipcrm msg $b ; ;;  esac; done 
ipcs -m | while read a b c; do case "$a" in 0x00000102) echo ipcrm shm $b; ipcrm shm $b ; ;;  esac; done 
ipcs -q | while read a b c; do case "$a" in 0x00000103) echo ipcrm msg $b; ipcrm msg $b ; ;;  esac; done 
ipcs -m | while read a b c; do case "$a" in 0x00000103) echo ipcrm shm $b; ipcrm shm $b ; ;;  esac; done 
ipcs -q | while read a b c; do case "$a" in 0x00000104) echo ipcrm msg $b; ipcrm msg $b ; ;;  esac; done 
ipcs -m | while read a b c; do case "$a" in 0x00000104) echo ipcrm shm $b; ipcrm shm $b ; ;;  esac; done 
ipcs -q | while read a b c; do case "$a" in 0x00000109) echo ipcrm msg $b; ipcrm msg $b ; ;;  esac; done 
ipcs -m | while read a b c; do case "$a" in 0x00000109) echo ipcrm shm $b; ipcrm shm $b ; ;;  esac; done 
echo 'Cancellazione dati Terminata'
