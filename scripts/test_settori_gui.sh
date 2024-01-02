#!/bin/bash

# st - Thu Jan 10 10:43:43 CET 2002
# applet di spegnimento macchina database

zenity --title "TEST SETTORI" --width=560 --height=760 --list --text "Test Settori" --print-column=ALL --column "ISOLA" --column "SETTORE" 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 2 1 2 2 2 3 2 4 2 5 2 6 2 7  > test_settori.out

case $? in
	0) 
		test_out=`cat test_settori.out`
		isola=`echo $test_out | cut -d '|' -f1`
		settore=`echo $test_out | cut -d '|' -f2`
		echo isola :  $isola 
		echo settore :  $settore 
		cd /u/prj/wella/exe; ./settori -a -t -i $isola -s $settore -q &
		zenity --title "TEST IN CORSO" --width=360 --height=360 --info --text "Test Isola $isola Settore $settore in corso\npremere ok per terminare" 
		case $? in
			0) killall -15 settori;;
			1) ;;
			255);;
		esac
		;;
	1) ;;
	255);;
esac
