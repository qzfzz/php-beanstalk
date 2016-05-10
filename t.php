<?php
$t = microtime( true );

$arrConfig = include( __DIR__ . '/include/config.inc'); 
$b = beanstalk_open( $arrConfig['host'], $arrConfig['port'] );

$lJobID = beanstalk_put( $b, "message" );
//var_dump( $lJobID);
$job = beanstalk_reserve( $b );
var_dump( $job );
$job = ( beanstalk_peekReady($b));
//var_dump( $job );
var_dump( beanstalk_statsJob( $b, $lJobID ));
//$strData = var_export( $job, true );
//var_dump( preg_match( "/array/", $strData ));

//var_dump( beanstalk_release( $b, $job['id'] ));

beanstalk_close( $b );

exit();
$lJobID = beanstalk_put( $b, "message" );
var_dump( $lJobID );
var_dump( beanstalk_peek( $b, 22 ));
var_dump( beanstalk_statsJob( $b, 22 ));
$strPeek = var_export( beanstalk_peek( $b, 22 ), true );
var_dump( preg_match( "/array/", $strPeek ));
var_dump( $strPeek );

beanstalk_close( $b );
exit();
$strPeek = var_export( beanstalk_peekReady( $b ), true );
var_dump( $strPeek );
var_dump( preg_match( "/array/", $strPeek ));

exit();
//var_dump( $b );
var_dump( beanstalk_putInTube( $b, 'tubea', 'msg 1 ' ));
/*var_dump( beanstalk_listTubes($b));
var_dump( beanstalk_listTubes($b));
var_dump( beanstalk_listTubes($b));
*/
var_dump( beanstalk_listTubes($b));
var_dump( beanstalk_stats( $b ));
var_dump( beanstalk_listTubes($b));
var_dump( beanstalk_listTubes($b));
var_dump( beanstalk_stats( $b ));
var_dump( beanstalk_listTubes($b));
var_dump( beanstalk_stats( $b ));
var_dump( beanstalk_listTubes($b));
var_dump( beanstalk_stats( $b ));
var_dump( beanstalk_listTubes($b));
var_dump( beanstalk_stats( $b ));
var_dump( beanstalk_listTubes($b));
var_dump( beanstalk_stats( $b ));
var_dump( beanstalk_listTubes($b));
var_dump( beanstalk_stats( $b ));
var_dump( beanstalk_listTubes($b));
var_dump( beanstalk_stats( $b ));
beanstalk_close( $b );
exit();
/*
for( $i = 0; $i < 10; ++$i )
{
	var_dump( beanstalk_put( $b, 'abcdefg' ));
}
var_dump( beanstalk_reserve( $b, 100 ));
var_dump( beanstalk_reserve( $b ));
var_dump( beanstalk_reserve( $b ));
var_dump( beanstalk_reserve( $b ));

beanstalk_close( $b );
*/


$job = beanstalk_reserve( $b );
$strData = var_export( $job, true );
var_dump( preg_match( "/array/", $strData ));

var_dump( beanstalk_release( $b, $job['id'] ));

beanstalk_close( $b );
exit();

( beanstalk_reserve( $b ));

$iSum = 0;
for( $i = 0; $i < 100000000; ++$i )
	$iSum *= $i;

	echo __LINE__, PHP_EOL;
//var_dump( beanstalk_reserve( $b ));
//var_dump( beanstalk_reserve( $b ));
//var_dump( beanstalk_statsJob( $b, 1 ));
var_dump( beanstalk_stats( $b ));
beanstalk_close( $b );
exit();
$b = beanstalk_open( $arrConfig['host'], $arrConfig['port'] );
exit();
$i = 0;
while(  $job = beanstalk_peekReady($b))
{
	if( !is_array( $job ))
	  break;
	//$job = beanstalk_peekReady( $b );
	
	if( isset(  $job['id'] ))
	{
		var_dump( beanstalk_delete( $b, $job['id'] ));
	}
	else
	{
		var_dump( $job );
	}
}
/*
var_dump(  beanstalk_reserve( $b, 100 ));
var_dump(  beanstalk_reserve( $b, 100 ));
var_dump(  beanstalk_reserve( $b, 100 ));
var_dump(  beanstalk_reserve( $b, 100 ));
var_dump(  beanstalk_reserve( $b, 100 ));
var_dump(  beanstalk_reserve( $b, 100 ));
var_dump(  beanstalk_reserve( $b, 100 ));
var_dump(  beanstalk_reserve( $b, 100 ));
*/
//$strData = var_export( $job, true );
//var_dump( preg_match( "/array/", $strData ));
beanstalk_close( $b );
//var_dump( $job );

$b = beanstalk_open( $arrConfig['host'], $arrConfig['port'] );
var_dump( beanstalk_stats( $b ));
beanstalk_close( $b );

exit();
//test for open
//$b = beanstalk_open( $arrConfig['host'], $arrConfig['port'] );
//var_dump( beanstalk_useTube( $b, 'default'));
//var_dump( beanstalk_watch( $b, 'xx'));
//beanstalk_close( $b );

/*
$b = beanstalk_open( $arrConfig['host'], $arrConfig['port'] );
beanstalk_putInTube( $b, 'test-tube-a', 'test' );
beanstalk_close( $b );
*/

$b = beanstalk_open( $arrConfig['host'], $arrConfig['port'] );
//var_dump( beanstalk_watch( $b, 'test-tube-a' ));
var_dump( beanstalk_reserve( $b ));
beanstalk_close( $b );

exit();

//test for put 

 //  $lJobID = beanstalk_put( $b, 31 );
//	var_dump( $lJobID );
/*

var_dump( beanstalk_delete( $b, $lJobID));
var_dump( beanstalk_delete( $b, $lJobID ));

*/
//$strRet = var_export( beanstalk_peekReady( $b ), true );
//var_dump( $strRet );

//var_dump( preg_match( "/array/", $strRet ));
//var_dump( beanstalk_peek( $b, 171918));

//$job = beanstalk_peekReady( $b );
//var_dump( beanstalk_listTubes( $b));
//var_dump( beanstalk_statsTube( $b, 'xx' ));
//var_dump( beanstalk_statsJob( $b, $job['id'] )); 
//var_dump( preg_match( "/array/", $strPeekReady ));
//var_dump( beanstalk_stats( $b ));
//var_dump(beanstalk_listTubes( $b ));
//var_dump( beanstalk_listTubeUsed( $b ));
//beanstalk_close( $b );

exit();
//echo 'abc';
//var_dump( beanstalk_stats( $b ));
//for( $i = 0; $i < 100000; ++$i )
//{
	//var_dump( beanstalk_put( $b, 'time:' . time()));
	//var_dump( beanstalk_putInTube( $b, 'tubea', 'time:' . time()));
//}
//var_dump( beanstalk_putInTube( $b, 'aaaaaaaaaa', 'message' ));
//var_dump( beanstalk_putInTube( $b, 'aaaaaaaaa', 'message' ));
//var_dump( beanstalk_putInTube( $b, 'aaaaaaaa', 'message' ));
//var_dump( beanstalk_watch( $b, 'abcdefg', 1 ));
//var_dump( beanstalk_watch( $b, 'abcdefg0', 1 ));
//var_dump( beanstalk_watch( $b, 'aaaaaaaaaa', 1 ));
//var_dump( beanstalk_listTubesWatched( $b ));
var_dump( beanstalk_listTubes( $b ));
//var_dump( beanstalk_watch( $b, 'default', 1 ));
//var_dump( beanstalk_useTube( $b, "xx" ));
var_dump( beanstalk_listTubeUsed( $b, 1 ));
//var_dump( beanstalk_listTubes( $b ));
//var_dump( beanstalk_touch( $b, 54 ));
//var_dump( beanstalk_reserve( $b, 100000 ));
//var_dump( beanstalk_bury( $b, 1 ));
//var_dump( beanstalk_ignore( $b, 'abcdefg' ));
//var_dump( beanstalk_ignore( $b, 'abcdef' ));
//var_dump( beanstalk_ignore( $b, 'default' ));
//var_dump( beanstalk_stats( $b ));
//var_dump( beanstalk_listTubes($b ));
//var_dump( beanstalk_peek( $b, 53 ));
//var_dump( beanstalk_peek( $b, 70722 ));

//$job = beanstalk_peekReady($b);
//var_dump( $job );
//sleep(10);
//var_dump( beanstalk_stats( $b ));
//sleep(1);
/*
var_dump( beanstalk_statsJob( $b, $job['id'] ));
var_dump( beanstalk_stats( $b));//NULL
var_dump( beanstalk_stats( $b));//NULL
var_dump( beanstalk_stats( $b));//NULL
var_dump( beanstalk_stats( $b));//NULL
var_dump( beanstalk_stats( $b));//NULL
var_dump( beanstalk_statsJob( $b, $job['id'] ));
var_dump( beanstalk_statsJob( $b, $job['id'] ));//NULL
var_dump( beanstalk_statsJob( $b, $job['id'] ));
*/

//var_dump( beanstalk_reserve( $b ));
//var_dump( beanstalk_reserve( $b ));
//var_dump( beanstalk_reserve( $b ));

//var_dump( beanstalk_release( $b, 1 ) );
//var_dump( beanstalk_release( $b, 2 ) );
//var_dump( beanstalk_release( $b, 3 ) );
//var_dump(beanstalk_kickJob( $b, 2));
//var_dump( beanstalk_pauseTube( $b, "tube-a", 10 )); 
//var_dump( beanstalk_pauseTube( $b, "tube-a", 0 )); 
//var_dump( beanstalk_kick( $b, 3000 ));
var_dump( beanstalk_close( $b ));
echo microtime( true ) - $t, '<br>';
