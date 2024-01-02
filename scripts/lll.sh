#!/bin/bash
cd /u/prj/wella/export_sap && /bin/tar zcvf backup/backup_export_`/bin/date +\%Y\%m\%d`.tgz  `/usr/bin/find -mtime +100 -type f` --remove-files --exclude='./backup'
#cd /u/prj/wella/export_sap && /usr/bin/find -mtime +2000 -type f
