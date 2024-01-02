
<?php
	require_once 'DB.php';
	$db =& DB::connect('pgsql://roberto:3zin@database/sap');
	if (PEAR::isError($db)) {
		printf("1<br>");
    die($db->getMessage());
	}
		printf("2<br>");

	$szBuffer=sprintf("update prova set value='2' where id='1';");
	$pre = $db->prepare($szBuffer);
		printf("3<br>");
	if (PEAR::isError($pre)) {
    die($pre->getMessage());
	}
		printf("4<br>");
	$res = $db->execute($pre);
	if (PEAR::isError($res)) {
		die($res->getMessage());
	}
	if($db->affectedRows()==1){
		printf("<br>Modifica Operatore");
		printf("<br>Avvenuta correttamente");
	} else {
		printf("<br>Errore in ");
		printf("<br>Modifica Operatore");
	}
	$db->freePrepared($pre);

	//$res->free();
?>
