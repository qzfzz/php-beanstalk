<?php
$t = microtime( true );

$b = beanstalk_open( "svn.huaer.dev" );
var_dump( BEANSTALK_TUBE_NOT_RETURN );
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
beanstalk_close( $b );
echo microtime( true ) - $t, '<br>';
