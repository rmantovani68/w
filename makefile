# ------------------------------------
# Makefile per WELLA - Easypicking 3.0
# ------------------------------------
# Copyright CNI srl 1995

all:
	(cd common; make all)
	(cd main; make all)
#	(cd main; make all)
#	(cd barcode; make all)
#	(cd bilancia; make all)
#	(cd isola; make all)
#	(cd peso; make all)
	(cd printlabel; make all)
#	(cd settori-CAN; make all)
#	(cd ios-CAN; make all)
	(cd sock; make all)
	(cd volum; make all)
	(cd settori; make all)
#	(cd ios; make all)
	(cd display; make all)

clean:
	(cd common; make clean)
	(cd main; make clean)
#	(cd barcode; make clean)
#	(cd bilancia; make clean)
#	(cd isola; make clean)
#	(cd peso; make clean)
	(cd printlabel; make clean)
#	(cd settori-CAN; make clean)
#	(cd ios-CAN; make clean)
	(cd sock; make clean)
	(cd volum; make clean)
	(cd settori; make clean)
#	(cd ios; make clean)
	(cd display; make clean)

main:
	(cd main; make all)

printlabel:
	(cd printlabel; make all)

volum:
	(cd volum; make all)

settori:
	(cd settori; make all)

ios:
	(cd ios; make all)

peso:
	(cd peso; make all)

bilancia:
	(cd bilancia; make all)

barcode:
	(cd barcode; make all)

sock:
	(cd sock; make all)

isola:
	(cd isola; make all)
