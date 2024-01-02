#!/bin/bash

# st - Thu Jan 10 10:43:43 CET 2002
# applet di spegnimento macchina database

zenity --title "Start Displays" --question --text "Confermi lo start dei displays ?" 360 360

case $? in
	0) cd /u/prj/wella/scripts;./start_displays.sh;;
	1) ;;
	255);;
esac
