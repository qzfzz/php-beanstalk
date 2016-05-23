<?php
$t = microtime( true );

$arrConfig = include( __DIR__ . '/include/config.inc'); 
$b = beanstalk_connect( $arrConfig['host'], $arrConfig['port'] );

$iTest = 6;

$arrErr = array();
$iHit = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
//peek peek ready
//op
$lJobID = beanstalk_put( $b, "message" );
$strPeek = var_export( beanstalk_peek( $b, $lJobID ), true );
if( preg_match( "/array/", $strPeek ))
{
	++$iHit;
}
else 
{
	$arrErr[] = "peek fail!";
}

$strPeek = var_export( beanstalk_peekReady( $b ), true );
if( preg_match( "/array/", $strPeek ))
{
	++$iHit;
}
else
{
	$arrErr[] = "peekReady fail!";
}

if( beanstalk_delete( $b, $lJobID ))
{
	++$iHit;
}
else
{
	$arrErr[] = "delete fail!";
}

//oo
$lJobID = $b->put( "message" );
$strPeek = var_export( $b->peek( $lJobID ), true );
if( preg_match( "/array/", $strPeek ))
{
	++$iHit;
}
else
{
	$arrErr[] = "oo peek fail!";
}

$strPeek = var_export( $b->peekReady(), true );
if( preg_match( "/array/", $strPeek ))
{
	++$iHit;
}
else
{
	$arrErr[] = "oo peekReady fail!";
}

if( beanstalk_delete( $b, $lJobID ))
{
	++$iHit;
}
else
{
	$arrErr[] = "oo delete fail!";
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//listTubes
$jobID = beanstalk_putInTube( $b, 'tubea', 'message1' );
$strRet = var_export( beanstalk_listTubes( $b ), true );
if( preg_match( "/array/", $strRet ))
{
	
}
else 
{
	$arrErr[] = 'listTubes fail!';
}

$strRet = var_export( beanstalk_listTubesWatched( $b ), true );
if( preg_match( "/array/", $strRet ))
{
	
}
else
{
	$arrErr[] = 'listTubesWatched fail!';
}

if( preg_match( "/[a-zA-Z]+/", beanstalk_listTubeUsed( $b )))
{
	
}
else
{
	$arrErr[] = 'listTubeUsed fail!';
}

$strRet = var_export( $b->listTubes(), true );
if( preg_match( "/array/", $strRet ))
{
	
}
else
{
	$arrErr[] = 'listTube fail!';
}

$strRet = var_export( $b->listTubesWatched(), true );
if( preg_match( "/array/", $strRet ))
{
	
}
else
{
	$arrErr[] = 'oo listTubesWatched fail!';
}


if( preg_match( "/[0-9a-zA-Z]+/", $b->listTubeUsed()))
{
	
}
else
{
	$arrErr[] = 'oo listTubeUsed fail!';
}

beanstalk_delete( $b, $jobID );



////////////////////////////////////////////////////////////////////////////////////////
//pause_resume
$jobID = beanstalk_putInTube( $b, 'test-tube-a', 'test' );

if( beanstalk_pauseTube( $b, 'test-tube-a', 100 ))
{
	
}
else
{
	$arrErr[] = 'putInPut fail!';
	
}

if( beanstalk_resumeTube( $b, 'test-tube-a' ))
{
	
}
else
{
	$arrErr[] = 'resumeTube fail!';
	
}


if( $b->pauseTube( 'test-tube-a', 100 ))
{
	
}
else 
{
	$arrErr[] = 'pause fail!';
	
}

if( $b->resumeTube( 'test-tube-a' ))
{
	
}
else 
{
	$arrErr[] = 'resume fail!';
	
}

beanstalk_delete( $b, $jobID );


////////////////////////////////////////////////////////////////////////////////////////
//bury_kick

$jobID = beanstalk_put( $b, "msg2" );

$job = beanstalk_reserve( $b );
// $strData = var_export( $job, true );
// var_dump( preg_match( "/array/", $strData ));

if( beanstalk_bury( $b, $job['id'] ) )
{
	
}
else 
{
	$arrErr[] = 'bury fail!';
	
}

if( beanstalk_kickJob( $b, $job['id'] ) )
{
	
}	
else 
{
	$arrErr[] = 'kick fail!';
	
}

if( beanstalk_kick( $b, 30 ) !== false )
{
	
}
else 
{
	$arrErr[] = 'kick fail!';
}
beanstalk_delete( $b, $jobID );


////////////////////////////////////////////////////////////////////////////////////////
// put

$lPut = beanstalk_put( $b, "hello this is a test"  );
$lPutInTube = beanstalk_putInTube( $b, 'tubea', "abc"  );
if( $lPut  )
{
	
}
else 
{
	$arrErr[] = 'put fail!';
}

if( $lPutInTube  )
{
	
}
else 
{
	$arrErr[] = 'putInTube fail!';
	
}

if( beanstalk_delete( $b, $lPut ) )
{
	
}
else
{
	$arrErr[] = 'delete fail!';
}

if( beanstalk_delete( $b, $lPutInTube  ) )
{
	
}
else
{
	$arrErr[] = 'delete fail!';
}


$lPut = $b->put( "hello this is a test"  );
$lPutInTube = $b->putInTube( 'tubea', "abc"  );
if( $lPut  )
{
	
}

if( $lPutInTube  )
{
	
}

if( $b->delete( $lPut ) )
{
	
}

if( $b->delete( $lPutInTube  ) )
{
	
}

//////////////////////////////////////////////////////////////////////////////////
//reserve

$jobID = beanstalk_put( $b, 'msg2' );
$job = beanstalk_reserve( $b );
$strData = var_export( $job, true );
if( preg_match( "/array/", $strData ))
{
	
}
else
{
	$arrErr[] = 'reserve fail!';
}


if( beanstalk_release( $b, $job['id'] ))
{
	
}
else
{
	$arrErr[] = 'release fail!';
}


if( beanstalk_delete( $b, $jobID ))
{
	
}


//////////////////////////////////////////////////////////////////////////////////////////
//stats
$strStats = var_export( beanstalk_stats( $b ), true );
if( preg_match( "/array/", $strStats ))
{
	
}
else
{
	$arrErr[] = 'stats fail!';
}

$strStats = var_export( $b->stats(), true );
if( preg_match( "/array/", $strStats ))
{
	
}
else
{
	$arrErr[] = 'stats fail!';
}

/////////////////////////////////////////////////////////////////////////////////////////////
//statusJob
$jobID = beanstalk_put( $b, "message" );
$job = beanstalk_peekReady( $b );
$strStats = var_export( beanstalk_statsJob( $b, $job['id'] ), true );
if( preg_match( "/array/", $strStats ))
{
	
}
else
{
	$arrErr[] = 'statsJob fail!';
}

$strStats = var_export( $b->statsJob( $job['id'] ), true );
if( preg_match( "/array/", $strStats ))
{
	
}
else
{
	$arrErr[] = 'statsJob fail!';
}

beanstalk_delete( $b, $jobID );



////////////////////////////////////////////////////////////////////////////////////////////////
//statusTube
$jobID = beanstalk_putInTube( $b, 'test-tube-a', 'test' );

$strStats = var_export( beanstalk_statsTube( $b, 'test-tube-a' ), true );
if( preg_match( "/array/", $strStats ))
{
	
}
else
{
	$arrErr[] = 'statsTube fail!';
}

$strStats = var_export( $b->statsTube( 'test-tube-a' ), true );
if( preg_match( "/array/", $strStats ))
{
	
}
else
{
	$arrErr[] = 'statsTube fail!';
}

beanstalk_delete( $b, $jobID );


/////////////////////////////////////////////////////////////////////////////////////////////////////
//useTube

$jobID = beanstalk_putInTube( $b, 'test-tube-a', 'test' );
if( beanstalk_useTube( $b, 'test-tube-a' ))
{
	
}

$b->putInTube( 'test-tube-a', 'test' );
if( $b->useTube( 'test-tube-a' ))
{
	
}

beanstalk_delete( $b, $jobID );

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//watch_ignore
if( beanstalk_watch( $b, 'test-tube-a' ))
{
	
}
else
{
	$arrErr[] = 'watch fail!';
}

if( beanstalk_watch( $b, 'test-tube-a', true ))
{
	
}
else
{
	$arrErr[] = 'watch fail!';
}


if( beanstalk_ignore( $b, 'test-tube-a' ))
{
	
}
else
{
	$arrErr[] = 'ignore fail!';
}


if( $b->watch( 'test-tube-a' ))
{
	
}
else
{
	$arrErr[] = 'watch fail!';
}


if( $b->watch( 'test-tube-a', true ))
{
	
}


if( $b->ignore( 'test-tube-a' ))
{
	
}
else
{
	$arrErr[] = 'ignore fail!';
}


beanstalk_close( $b  );

if( !empty( $arrErr ))
{
	echo "test fail!\r\n";
	
	var_dump( $arrErr );
}

echo 'test cost: ' . ( microtime( true ) - $t),  ' seconds!<br>';
