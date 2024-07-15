scp *.txt roberto@wella1:/u/prj/wella/muletest 
export CURR_DATETIME=`date +%Y-%m-%d-%H_%M_%S` 
mv dett_colli.txt dett_colli.txt.$CURR_DATETIME 
mv ordcni.txt ordcni.txt.$CURR_DATETIME 
mv col_cni.txt col_cni.txt.$CURR_DATETIME

