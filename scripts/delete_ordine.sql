#!/bin/bash
echo "delete from ric_ord where ordprog='$1';"  | psql sap;
echo "delete from ric_art where ordprog='$1';"  | psql sap;
echo "delete from ord_prod where ordprog='$1';" | psql sap;
echo "delete from col_prod where ordprog='$1';" | psql sap;
echo "delete from rig_prod where ordprog='$1';" | psql sap;
