select lpcduds,lpcdfor,sum(lpqtlet) from rese_letture_prodotti where lpflsca='S' and lpstato!='S' and lpprlot=18 and (lplabel~'^A' or lplabel~'^B') group by lpcduds,lpcdfor order by lpcduds,lpcdfor;
