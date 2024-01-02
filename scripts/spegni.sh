#!/bin/bash

# st - Thu Jan 10 10:43:43 CET 2002
# applet di spegnimento macchina database

gdialog --title "spegnimento DBserver" --yesno "Confermi lo spegnimento del PC DATABASE ?" 360 360

case $? in
	0) rsh database poweroff;;
	1) ;;
	255);;
esac
