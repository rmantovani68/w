#!/bin/bash

# st - Thu Jan 10 10:43:43 CET 2002
# applet di spegnimento macchina database

zenity --title "spegnimento DBserver" --question --text "Confermi lo spegnimento dei PC ?" 360 360

case $? in
	0) cd /u/prj/wella/scripts;./spegni_tutto.sh;;
	1) ;;
	255);;
esac
