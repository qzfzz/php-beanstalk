<?php

define( 'TUBE_RETURN', 1 );
define( 'TUBE_NOT_RETURN', 0 );
define( 'RETURN_WATCH_SIZE', 1 );
define( 'NOT_RETURN_WATCH_SIZE', 0 );
define( 'BEANSTALK_ASK_SERVER', 1 );
define( 'BEANSTALK_NOT_ASK_SERVER', 0 );

/**
 * beanstalk_close( $resource )
 *
 * @param resource $resource
 *
 * return true for success false for failure
 */
function beanstalk_close( $resource );

/**
 * beanstalk_open( $host = '127.0.0.1', $port = 11300 )
 * @param $host
 * @param $port
 *
 * return source id for success false for failure
 */
function beanstalk_open( $host = '127.0.0.1', $port = 11300 );

/**
 * beanstalk_peek( $resource, $lJobID )
 * @param $resource		resource id
 * @param $lJodID		job id
 *
 * return false for error string data for success
 */
function beanstalk_peek( $resource, $lJobID );


/**
 * beanstalk_peek( $resource, $strTube = 'default' )
 * @param $resource		resource id
 * @param $strTube		tube name
 *
 * return array data for success others for failure
 */
function beanstalk_peekReady( $resource, $strTube = 'default' );

/**
 * beanstalk_delete( $resource, $jobID )
 * @param $resource
 * @param $jobID
 *
 * return true for success false for failue
 * delete jobid\r\n
 */
function beanstalk_delete( $resource, $jobID );

/**
 * beanstalk_put( $beanstalkResource, $strMsg, $iPri = 1024, $iDelay = 0, $iTtr = 60 )
 *
 * @param resource 	$beanstalkResource
 * @param string 	$data	The job data
 * @param int    	$iPri	From 0 (most urgent) to 0xFFFFFFFF (least urgent)
 * @param int    	$iDelay	Seconds to wait before job becomes ready
 * @param int    	$iTtr	Time To Run: seconds a job can be reserved for
 *
 * return jobID for success false for failure;
 */
function beanstalk_put( $beanstalkResource, $strMsg, $iPri = 1024, $iDelay = 0, $iTtr = 60 );


/**
 * beanstalk_putInTube( $resource, $strTube, $strMsg, $lPri, $lDelay, $lTtr )
 *
 * @param resource	$id;
 * @param string 	$tube;
 * @param string	$message;
 * @param long		$privilege
 * @param long		$delay
 * @param long		$ttr
 *
 * return job id or false for error
 */
function beanstalk_putInTube( $resource, $strTube, $strMsg, $lPri, $lDelay, $lTtr );


/**
 * beanstalk_status( $resource )
 * @param $resource
 *
 * return stats array
 */
function beanstalk_stats( $resource );

/**
 * beanstalk_statsJob( $resource, $lJobID )
 *
 * @param $resource
 * @param $lJobID
 *
 * return false for failure job's status array
 */
function beanstalk_statsJob( $resource, $lJobID );

/**
 * beanstalk_statsTube( $resource, $strTube )
 *
 * @param $resource
 * @param $strTube tube name
 *
 * return tube's status array for success false for failure
 */
function beanstalk_statsTube( $resource, $strTube );

/**
 * beanstalk_bury( $resource, $lJobID, $lPri = 1024 )
 *
 * @param $resource
 * @param $lJobID
 * @param $lPri = 1024
 *
 * return false for failure true for success
 */
function beanstalk_bury( $resource, $lJobID, $lPri = 1024 );


/**
 * beanstalk_ignore( $resource, $strTube )
 *
 * @param resource	$resource
 * @param string	$strTube
 *
 * return:
 * integers for watched size
 * false for error
 */
function beanstalk_ignore( $resource, $strTube );


/**
 * beanstalk_kick( $resource, $max = 30 )
 *
 * @param	resource	$resource
 * @param 	int			$max
 *
 * return false for error others for kicked jobs
 */
function beanstalk_kick( $resource, $max = 30 );

/**
 * beanstalk_kickJob( $resource, $iJobID )
 *
 * @param	resource	$resource
 * @param	int			$iJobID
 *
 * return true for success false for error or not found etc.
 */
function beanstalk_kickJob( $resource, $iJobID );

/**
 * beanstalk_listTubes( $resource )
 * @param $resource
 *
 * return array for success false for failure
 */
function beanstalk_listTubes( $resource );

/**
 * beanstalk_listTubes( $resource )
 * @param $resource
 *
 * return array for success false for failure
 */
function beanstalk_listTubesWatched( $resource );

/**
 * beanstalk_listTubeUsed( $resource, $bAskServer = BEANSTALK_NOT_ASK_SERVER )
 * @param $resource
 * @param $bAskServer
 *
 * return string or true for success others for failure
 */
function beanstalk_listTubeUsed(  $resource, $bAskServer = 0  );

/**
 * beanstalk_pauseTube( $resource, $strTube, $lDelay )
 *
 * @param $resource
 * @param $strTube tube name
 * @param $lDelay delay for pause
 *
 * return false for failure true for success
 */
function beanstalk_pauseTube( $resource, $strTube, $lDelay );

/**
 *	beanstalk_resumeTube( $resource, $strTube )
 *
 *	@param $resource
 *	@param $strTube tube name
 *
 *	return false for success true for success
 */
function beanstalk_resumeTube( $resource, $strTube );

/**
 * beanstalk_peekDelayed( $resource, $strTube = 'default')
 *
 * @param $resource
 * @param $strTube tube name
 *
 * return false for error others for success
 */
function beanstalk_peekDelayed( $resource, $strTube = 'default' );

/**
 * beanstalk_peekBuried( $resource, $strTube = "default" )
 *
 * @param $resource
 * @param $strTube tube name
 *
 * return false for error others for success
 */
function beanstalk_peekBuried( $resource, $strTube = "default" );

/**
 * beanstalk_touch( $resource, $jobID )
 * @param resource $resource
 * @param long	   $jobID
 *
 * return
 * true for success
 * false for the job does not exist or is not reserved by the client.
 */
function beanstalk_touch( $resource, $jobID );

/**
 * beanstalk_release( $resource, $lJobID, $lPri = 1024, $lDelay = 0 )
 *
 * @param $resource
 * @param $lJobID
 * @param $lPri = 1024
 * @param $lDelay = 0
 *
 * return false for error others for true
 */
function beanstalk_release( $resource, $lJobID, $lPri = 1024, $lDelay = 0 );

/**
 * beanstalk_reserve( $rid, $lTimeOut )
 *
 * @param	resource	$rid
 * @param	long		$lTimeOut for seconds
 *
 * return array()
 */
function beanstalk_reserve( $resource, $lTimeOut );


//function beanstalk_reserveFromTube)
//{
//}


/**
 * beanstalk_useTube( $resource, $strTube )
 *
 * @param $resource
 * @param $strTube
 *
 * return false for error others for true
 */
function beanstalk_useTube( $resource, $strTube );


/**
 * beanstalk_watch( $resource, $tube = 'default', BEANSTALK_WATCH_SIZE_NOT_RETURN )
 *
 * @param	resource	$resource
 * @param	string		$tube
 * @param	int			$bWatchSize
 *
 * return true or watchsize for success false for error
 *
 * BEANSTALK_WATCH_SIZE_RETURN
 * BEANSTALK_WATCH_SIZE_NOT_RETURN
 */
function beanstalk_watch( $resource, $tube = 'default', $lRet = BEANSTALK_WATCH_SIZE_NOT_RETURN);

/*
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('beanstalk')) {
	dl('beanstalk.' . PHP_SHLIB_SUFFIX);
}
$module = 'beanstalk';
$functions = get_extension_funcs($module);
echo "Functions available in the test extension:$br\n";
foreach($functions as $func) {
    echo $func."$br\n";
}
echo "$br\n";
//$function = 'confirm_' . $module . '_compiled';
if (extension_loaded($module)) {
//	$str = $function($module);
} else {
	$str = "Module $module is not compiled into PHP";
}
*/
//echo "$str\n";
?>
