<?php
	$username="rampenke_edtv24";
	$password="edtv24";
	$database="rampenke_edtv24";
  $server='localhost';
   
	/* soak in the passed variable or set our own */
	$number_of_channels = isset($_GET['limit']) ? intval($_GET['limit']) : 24;
	$start_channel = isset($_GET['start']) ? intval($_GET['start']) : 0;
  $call_back = isset($_GET['callback']) ? $_GET['callback'] : 0;
  
	/* connect to the db */
	$link = mysql_connect($server, $username, $password) or die('Cannot connect to the DB');
	mysql_select_db($database,$link) or die('Cannot select the DB');

	/* grab the channels from the db */
	$query="SELECT * FROM current_channels ORDER BY id LIMIT $number_of_channels";
	$result = mysql_query($query,$link) or die('Errant query:  '.$query);

	/* create one master array of the records */
	$channels = array();
	if(mysql_num_rows($result)) {
		while($row = mysql_fetch_assoc($result)) {
    $id = $row['id'];
    $title = $row['title'];
    $rating = $row['rating'];
    $synopsis = $row['synopsis'];
    
    $thumbnail = $row['thumbnail'];
    $thumbnail = base64_encode($thumbnail);
    
    $poster = $row['poster'];
    $poster = base64_encode($poster);    
    
    $url = $row['url'];    
    $urllq = $row['urllq'];        
    $record = array( 'title' => $title, 'rating' => $rating,
              'synopsis' => $synopsis, 'thumbnail' => $thumbnail,
              'poster' => $poster, 'url' => $url, 'urllq' => $urllq);
			/*$channels[] = $row;*/
      $channels[] = $record;
		}
	}

  /* output in necessary format */
  header('Content-type: text/javascript;charset=ISO-8859-1');
  echo $call_back;
  echo '(';
  echo json_encode(array('channels'=>$channels));
  echo ')';
	/* disconnect from the db */
	@mysql_close($link);
?>