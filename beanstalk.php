<?php

define( 'TUBE_RETURN', 1 );
define( 'TUBE_NOT_RETURN', 0 );
define( 'RETURN_WATCH_SIZE', 1 );
define( 'NOT_RETURN_WATCH_SIZE', 0 );
define( 'BEANSTALK_ASK_SERVER', 1 );
define( 'BEANSTALK_NOT_ASK_SERVER', 0 );

/**
 * beanstalk_close( $beanstalkObj )
 *
 * @param resource $beanstalkObj
 *
 * return true for success false for failure
 */
function beanstalk_close( $beanstalkObj );

/**
 * beanstalk_connect( $host = '127.0.0.1', $port = 11300 )
 * @param $host
 * @param $port
 *
 * return source id for success false for failure
 */
function beanstalk_connect( $host = '127.0.0.1', $port = 11300 );

/**
 * beanstalk_peek( $beanstalkObj, $lJobID )
 * @param $beanstalkObj		resource id
 * @param $lJodID		job id
 *
 * return false for error string data for success
 */
function beanstalk_peek( $beanstalkObj, $lJobID );


/**
 * beanstalk_peek( $beanstalkObj, $strTube = 'default' )
 * @param $beanstalkObj		resource id
 * @param $strTube		tube name
 *
 * return array data for success others for failure
 */
function beanstalk_peekReady( $beanstalkObj, $strTube = 'default' );

/**
 * beanstalk_delete( $beanstalkObj, $jobID )
 * @param $beanstalkObj
 * @param $jobID
 *
 * return true for success false for failue
 * delete jobid\r\n
 */
function beanstalk_delete( $beanstalkObj, $jobID );

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
 * beanstalk_putInTube( $beanstalkObj, $strTube, $strMsg, $lPri, $lDelay, $lTtr )
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
function beanstalk_putInTube( $beanstalkObj, $strTube, $strMsg, $lPri, $lDelay, $lTtr );


/**
 * beanstalk_status( $beanstalkObj )
 * @param $beanstalkObj
 *
 * return stats array
 */
function beanstalk_stats( $beanstalkObj );

/**
 * beanstalk_statsJob( $beanstalkObj, $lJobID )
 *
 * @param $beanstalkObj
 * @param $lJobID
 *
 * return false for failure job's status array
 */
function beanstalk_statsJob( $beanstalkObj, $lJobID );

/**
 * beanstalk_statsTube( $beanstalkObj, $strTube )
 *
 * @param $beanstalkObj
 * @param $strTube tube name
 *
 * return tube's status array for success false for failure
 */
function beanstalk_statsTube( $beanstalkObj, $strTube );

/**
 * beanstalk_bury( $beanstalkObj, $lJobID, $lPri = 1024 )
 *
 * @param $beanstalkObj
 * @param $lJobID
 * @param $lPri = 1024
 *
 * return false for failure true for success
 */
function beanstalk_bury( $beanstalkObj, $lJobID, $lPri = 1024 );


/**
 * beanstalk_ignore( $beanstalkObj, $strTube )
 *
 * @param resource	$beanstalkObj
 * @param string	$strTube
 *
 * return:
 * integers for watched size
 * false for error
 */
function beanstalk_ignore( $beanstalkObj, $strTube );


/**
 * beanstalk_kick( $beanstalkObj, $max = 30 )
 *
 * @param	resource	$beanstalkObj
 * @param 	int			$max
 *
 * return false for error others for kicked jobs
 */
function beanstalk_kick( $beanstalkObj, $max = 30 );

/**
 * beanstalk_kickJob( $beanstalkObj, $iJobID )
 *
 * @param	resource	$beanstalkObj
 * @param	int			$iJobID
 *
 * return true for success false for error or not found etc.
 */
function beanstalk_kickJob( $beanstalkObj, $iJobID );

/**
 * beanstalk_listTubes( $beanstalkObj )
 * @param $beanstalkObj
 *
 * return array for success false for failure
 */
function beanstalk_listTubes( $beanstalkObj );

/**
 * beanstalk_listTubes( $beanstalkObj )
 * @param $beanstalkObj
 *
 * return array for success false for failure
 */
function beanstalk_listTubesWatched( $beanstalkObj );

/**
 * beanstalk_listTubeUsed( $beanstalkObj, $bAskServer = false )
 * @param $beanstalkObj
 * @param $bAskServer
 *
 * return string or true for success others for failure
 */
function beanstalk_listTubeUsed(  $beanstalkObj, $bAskServer = false  );

/**
 * beanstalk_pauseTube( $beanstalkObj, $strTube, $lDelay )
 *
 * @param $beanstalkObj
 * @param $strTube tube name
 * @param $lDelay delay for pause
 *
 * return false for failure true for success
 */
function beanstalk_pauseTube( $beanstalkObj, $strTube, $lDelay );

/**
 *	beanstalk_resumeTube( $beanstalkObj, $strTube )
 *
 *	@param $beanstalkObj
 *	@param $strTube tube name
 *
 *	return false for success true for success
 */
function beanstalk_resumeTube( $beanstalkObj, $strTube );

/**
 * beanstalk_peekDelayed( $beanstalkObj, $strTube = 'default')
 *
 * @param $beanstalkObj
 * @param $strTube tube name
 *
 * return false for error others for success
 */
function beanstalk_peekDelayed( $beanstalkObj, $strTube = 'default' );

/**
 * beanstalk_peekBuried( $beanstalkObj, $strTube = "default" )
 *
 * @param $beanstalkObj
 * @param $strTube tube name
 *
 * return false for error others for success
 */
function beanstalk_peekBuried( $beanstalkObj, $strTube = "default" );

/**
 * beanstalk_touch( $beanstalkObj, $jobID )
 * @param resource $beanstalkObj
 * @param long	   $jobID
 *
 * return
 * true for success
 * false for the job does not exist or is not reserved by the client.
 */
function beanstalk_touch( $beanstalkObj, $jobID );

/**
 * beanstalk_release( $beanstalkObj, $lJobID, $lPri = 1024, $lDelay = 0 )
 *
 * @param $beanstalkObj
 * @param $lJobID
 * @param $lPri = 1024
 * @param $lDelay = 0
 *
 * return false for error others for true
 */
function beanstalk_release( $beanstalkObj, $lJobID, $lPri = 1024, $lDelay = 0 );

/**
 * beanstalk_reserve( $rid, $lTimeOut )
 *
 * @param	resource	$rid
 * @param	long		$lTimeOut for seconds
 *
 * return array()
 */
function beanstalk_reserve( $beanstalkObj, $lTimeOut );


//function beanstalk_reserveFromTube)
//{
//}


/**
 * beanstalk_useTube( $beanstalkObj, $strTube )
 *
 * @param $beanstalkObj
 * @param $strTube
 *
 * return false for error others for true
 */
function beanstalk_useTube( $beanstalkObj, $strTube );


/**
 * beanstalk_watch( $beanstalkObj, $tube = 'default', $bWatchSize = false )
 *
 * @param	resource	$beanstalkObj
 * @param	string		$tube
 * @param	bool		$bWatchSize
 *
 * return true or watchsize for success false for error
 */
function beanstalk_watch( $beanstalkObj, $tube = 'default', $bWatchSize = false);

/**
 * oo version of beanstalk's php client
 */
class Beanstalk
{
	public function connect( $host = '127.0.0.1', $port = 11300 ){}
	
	public function close(){}
	
	public function put( $strMsg, $iPri = 1024, $iDelay = 0, $iTtr = 60 ){}
	
	public function peek( $lJobID ){}
	
	public function peekReady( $strTube = 'default' ){}
	
	public function delete( $jobID ){}
	
	public function stats(){}
	
	public function bury( $lJobID, $lPri = 1024 ){}
	
	public function ignore( $strTube ){}
	
	public function kick( $max = 30 ){}
	
	public function kickJob( $iJobID ){}
	
	public function listTubes(){}
	
	public function listTubesWatched(){}
	
	public function listTubeUsed( $bAskServer = false ){}
	
	public function pauseTube( $strTube, $lDelay ){}
	
	public function resumeTube( $strTube ){}
	
	public function peekDelayed( $strTube = 'default' ){}
	
	public function peekBuried( $strTube = "default" ){}
	
	public function putInTube( $strTube, $strMsg, $lPri, $lDelay, $lTtr ){}
	
	public function release( $lJobID, $lPri = 1024, $lDelay = 0 ){}
	
	public function reserve( $lTimeOut ){}
	
	//public function beanstalk_reserveFromTube, NULL)
	
	public function statsJob( $lJobID ){}
	
	public function statsTube( $strTube ){}
	
	public function touch( $jobID ){}
	
	public function useTube( $strTube ){}
	
	public function watch( $tube = 'default', $bRetWatchSize = false ){}
}
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
