<?

class cl_template {
	var $title;
	var $header;
	var $footer;
	var $page;
	var $bgcolor;
	var $body;
	var $bSetHeader;
	var $bSetFooter;


	Function SetPageTitle($title)
	{
		$this->title=$title;
	}

	Function SetHeader()
	{
		if(!isset($this->bSetHeader)){
			$this->bSetHeader=1;
		}
		$this->header="
	<html>

	<head>
	<title>$this->title</title>
	<table border=\"0\" width=\"100%\" cols=\"3\" nosave>
			<tr>
					<td nosave><img src=\"cni-ril-titolo.gif\" width=\"354\"
					height=\"165\"></td>
					<td>&nbsp;</td>
					<td>WELLA ITALIA<p>Impianto PAPERLESS <br>
					Castiglione delle Stiviere (MN) <br>
					<img src=\"logo_wella.gif\" width=\"64\" height=\"50\"></p>
					</td>
			</tr>
	</table>

	</head>

	<body background=\"cni-ril-sfondo.gif\" bgcolor=\"#C0C0C0\">
		";

	}

	Function SetFooter()
	{
		if(!isset($this->bSetFooter)){
			$this->bSetHeader=1;
		}
		$this->footer = "
	</body>
	</html>
		";
	}

	Function SetBody($body)
	{
		$this->body=$body;
	}

	Function ShowPage()
	{
		if(!isset($this->bSetHeader)){
			$this->SetHeader();
		}
		if(!isset($this->bSetFooter)){
			$this->SetFooter();
		}
		$this->page = $this->header.$this->body.$this->footer;
		return($this->page);
	}
}



?>
