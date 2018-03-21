<!DOCTYPE>
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=no"/>
  <title>Searchresult</title>

  <!-- JS -->
  <script language="javascript" type="text/javascript">
  function checkfun(){
    var folder = document.getElementById("folder").value;
    if(folder==""){
        self.location='/'; 
        return false;
        }
    else
      return true;
  }
  </script>
  <script src="http://www.jq22.com/jquery/jquery-1.10.2.js"></script>
  <script type="text/javascript" src="page.js"></script>

  <!-- CSS  -->
  <link href="https://fonts.googleapis.com/icon?family=Material+Icons" rel="stylesheet">
  <link href="css/materialize.css" type="text/css" rel="stylesheet" media="screen,projection"/>
  <link href="css/style.css" type="text/css" rel="stylesheet" media="screen,projection"/>
  <style type="text/css">
  	@media only screen and (max-width: 992px) {
  	  nav .logo_sty {
  	  	margin-left: -35%;
  	  }
  	  nav .formblock{
  	  	margin-left: 10%;
  	  }
  	  nav .submitblock{
  	  	width: 60%
  	  }
  }
  </style>
</head>
<body>
  <nav class="white" role="navigation">
    <div style="margin-left: 5%">
      <a id="logo-container" href="/" class="brand-logo logo_sty" style="display: inline;">DC Search</a>
    </div>
    <div style="margin-left: 20%;">
    	<form class="fw_items" action="result.php" method="POST" style="width: 80%">
			<input class="blacktext" type="text" style="color: #000000; width: 70%" name="Searchtext" id="folder" value="<?=$_POST["Searchtext"]; ?>" maxlength="2048">
			<button onclick="return checkfun();" type="submit" class="btn waves-effect waves-light teal lighten-1">DC 搜索</button>
	  </form> 
    </div>
  </nav>
  <div class="container">
    <ul>
      <?php
        $result = isset($_POST['Searchtext']) ? $_POST['Searchtext'] : '';
		$result = iconv("utf-8","gb2312//IGNORE",$result);
        $output = array();
        $signal = 0;
		$size = 0;
	    $command = '.\Client.exe ' . $result;
	    exec($command,$output,$status);
        echo "<div class='row'>";
        echo "<div class='col l2'>本次搜索用时{$output[$signal]}ms</div>";
		echo "<div class='col l3'>我们为您找到{$output[$signal+1]}个结果</div>";
        echo "</div>";
		$size = intval($output[$signal+1]);
        for($signal = 3; $signal <= $size*3 ; $signal+=3){
			if(!empty($output[$signal-1])&&!empty($output[$signal])){
				if(!empty($output[$signal-1])){
					$output[$signal - 1] = iconv("gb2312","utf-8//IGNORE",$output[$signal - 1]);
				}
				if(!empty($output[$signal])){
					$output[$signal] = iconv("gb2312","utf-8//IGNORE",$output[$signal]);
				}
				echo "<div class='card-panel hoverable'>";
				echo "<li class = 'left-align'><h5 class='flow text'><a href={$output[$signal - 1]} target='_blank'>{$output[$signal]}</a></h5></li>";
				echo "<li class='flow text'>{$output[$signal - 1]}</li>";
				if(!empty($output[$signal+1])){
					echo "<li class = 'light truncate'><p class='flow text'>{$output[$signal + 1]}</p></li>";
				}
				echo "</div>";
			}
        }
      ?>
    </ul>
  </div>
</body>
</html>