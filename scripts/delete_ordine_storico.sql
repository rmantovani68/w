#!/bin/bash
echo "delete from ric_stor where ordprog='$1';"  | psql sap;
echo "delete from ord_stor where ordprog='$1';" | psql sap;
echo "delete from col_stor where ordprog='$1';" | psql sap;
echo "delete from rig_stor where ordprog='$1';" | psql sap;
