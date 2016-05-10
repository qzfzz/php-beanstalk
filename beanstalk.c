/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_beanstalk.h"
#include "Zend/zend_list.h"
#include "Zend/zend_alloc.h"
#include "Zend/zend_hash.h"
#include <string.h>

/* If you declare any globals in php_beanstalk.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(beanstalk)
*/
ZEND_DECLARE_MODULE_GLOBALS(beanstalk)
/* True global resources - no need for thread safety here */
#define PHP_DESCRIPTOR_BEANSTALK_RES_NAME "stream"
#define CRLF "\r\n"

//返回错误
#define RESPONSE_ERR_OUT_OF_MEMORY    "OutOfMemory",
#define RESPONSE_ERR_INTERNAL_ERROR   "InternalError"
#define RESPONSE_ERR_DRAINING         "Draining"
#define RESPONSE_ERR_BAD_FORMAT       "BadFormat"
#define RESPONSE_ERR_UNKNOWN_COMMAND  "UnknownCommand"

//返回的COMMAND
#define RESPONSE_INSERTED "INSERTED"
#define RESPONSE_BURIED "BURIED"
#define RESPONSE_EXPECTED_CRLF "EXPECTED_CRLF"
#define RESPONSE_JOB_TOO_BIG "JOB_TOO_BIG"
#define RESPONSE_USING "USING"
#define RESPONSE_DEADLINE_SOON "DEADLINE_SOON"
#define RESPONSE_RESERVED "RESERVED"
#define RESPONSE_DELETED "DELETED"
#define RESPONSE_NOT_FOUND "NOT_FOUND"
#define RESPONSE_RELEASED "RELEASED"
#define RESPONSE_WATCHING "WATCHING"
#define RESPONSE_NOT_IGNORED "NOT_IGNORED"
#define RESPONSE_FOUND "FOUND"
#define RESPONSE_KICKED "KICKED"
#define RESPONSE_OK "OK"
#define RESPONSE_TIMED_OUT "TIMED_OUT"
#define RESPONSE_TOUCHED "TOUCHED"
#define RESPONSE_PAUSED "PAUSED"

//返回COMMAND后有内容的
#define RESPONSE_DATA_RESERVED RESPONSE_RESERVED
#define RESPONSE_DATA_FOUND RESPONSE_FOUND
#define RESPONSE_DATA_OK RESPONSE_OK

#define COMMAND_PUT "put"
#define COMMAND_USE "use"
#define COMMAND_RESERVE "reserve"
#define COMMAND_RESERVE_WITH_TIMEOUT "reserve-with-timeout"
#define COMMAND_DELETE "delete"
#define COMMAND_RELEASE "release"
#define COMMAND_BURY "bury"
#define COMMAND_WATCH "watch"
#define COMMAND_IGNORE "ignore"
#define COMMAND_PEEK "peek"
#define COMMAND_PEEK_READY "peek-ready"
#define COMMAND_PEEK_DELAYED "peek-delayed"
#define COMMAND_PEEK_BURIED "peek-buried"
#define COMMAND_KICK "kick"
#define COMMAND_KICK_JOB "kick-job"
#define COMMAND_STATS_JOB "stats-job"
#define COMMAND_STATS_TUBE "stats-tube"
#define COMMAND_STATS "stats"
#define COMMAND_LIST_TUBES "list-tubes"
#define COMMAND_LIST_TUBE_USED "list-tube-used"
#define COMMAND_LIST_TUBES_WATCHED "list-tubes-watched"
#define COMMAND_TOUCH "touch"
#define COMMAND_PAUSE_TUBE "pause-tube"
#define COMMAND_SEPARATOR " "


#define RESPONSE_INT_DATA_WATCHING 1
#define RESPONSE_INT_DATA_KICKED 2
#define RESPONSE_INT_DATA_INSERTED 3

#define RESPONSE_NO_DATA_BURIED		1
#define RESPONSE_NO_DATA_DELETED	2
#define RESPONSE_NO_DATA_KICKED		3
#define RESPONSE_NO_DATA_PAUSED		4
#define RESPONSE_NO_DATA_RELEASED	5
#define RESPONSE_NO_DATA_TOUCHED	6

#define TUBE_RETURN 1
#define TUBE_NOT_RETURN 0
#define RETURN_WATCH_SIZE 1
#define NOT_RETURN_WATCH_SIZE 0

#define RETRY_TIMES 5
//#define SHORT_RESPONSE_NOT_IGNORED "NOT_IGNORED"
//#define SHORT_RESPONSE_BURIED "BURIED"
//#define SHORT_RESPONSE_EXPECTED_CRLF "EXPECTED_CRLF"
//#define SHORT_RESPONSE_JOB_TOO_BIG "JOB_TOO_BIG"
//#define SHORT_RESPONSE_USING "USING"
//#define SHORT_RESPONSE_DEADLINE_SOON "DEADLINE_SOON"
//#define SHORT_RESPONSE_RESERVED "RESERVED"
//#define SHORT_RESPONSE_DELETED "DELETED"
//#define SHORT_RESPONSE_NOT_FOUND "NOT_FOUND"
//#define SHORT_RESPONSE_RELEASED "RELEASED"
//#define SHORT_RESPONSE_FOUND "FOUND"
//#define SHORT_RESPONSE_OK "OK"
//#define SHORT_RESPONSE_TIMED_OUT "TIMED_OUT"
//#define SHORT_RESPONSE_TOUCHED "TOUCHED"
//#define SHORT_RESPONSE_PAUSED "PAUSED"


/**
 * beanstalk resource
 */
static int le_beanstalk;

static int useTube( php_stream *pStream, char* pTube, zval** return_value, int bList );
static char* explodeString( char* pStr, zval* array );
static void getStatsResponse( php_stream* pStream, char* pCmd, zval** return_value );
static __inline__ void getResponseComplexData( php_stream* pStream, zval** return_value, char* pCmd );
static void pauseResumeTube( php_stream* pStream, zval** return_value, char* pStrTube, long lDelay );
static int getListTubeResponse( php_stream* pStream, zval** return_value, char* pCmd );
static void getResponseIntData( php_stream* pStream, zval** return_value, char* pCmd, int iType );
static void getResponseWithNoData( php_stream* pStream, zval** return_value, char* pCmd, int iResponseType );

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("beanstalk.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_beanstalk_globals, beanstalk_globals)
    STD_PHP_INI_ENTRY("beanstalk.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_beanstalk_globals, beanstalk_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_beanstalk_compiled(string arg)
   Return a string to confirm that the module is compiled in */
//PHP_FUNCTION(confirm_beanstalk_compiled)
//{
//	char *arg = NULL;
//	int arg_len, len;
//	char *strg;
//
//	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
//		return;
//	}
//
//	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "beanstalk", arg);
//	RETURN_STRINGL(strg, len, 0);
//}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/

/**
 * beanstalk_close( $resource )
 *
 * @param resource $resource
 *
 * return true for success false for failure
 */
PHP_FUNCTION(beanstalk_close)
{
	php_stream *pStream;
	zval *zStream;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zStream ) == FAILURE )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid resource provided");

		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*, &zStream, -1, PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk );

	zend_hash_index_del(&EG(regular_list),Z_RESVAL_P( zStream ));
	RETURN_TRUE;
}

/**
 * beanstalk_open( $host = '127.0.0.1', $port = 11300 )
 * @param $host
 * @param $port
 *
 * return source id for success false for failure
 */
PHP_FUNCTION(beanstalk_open)
{
    char *host = "127.0.0.1", *transport, *errstr = NULL;
	int host_len = sizeof( "127.0.0.1" ) - 1, transport_len, implicit_tcp = 1, errcode = 0;
    int options = ENFORCE_SAFE_MODE;
    int flags = STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT;
	php_stream* pStream = NULL;
	int iHostLen = -1;
	long lPort = 11300;//default 11300

	char* pWBuf = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sl", &host, &iHostLen, &lPort ) == FAILURE) {
		RETURN_FALSE;
	}

	if( iHostLen != host_len )
	{
		host_len = iHostLen;
	}

	if(lPort)
	{
		int implicit_tcp = 1;
		if (strstr(host, "://"))
		{
			implicit_tcp = 0;
		}
		transport_len = spprintf(&transport, 0, "%s%s:%d",
				implicit_tcp ? "tcp://" : "", host, lPort);
	}
	else
	{
		transport = host;
		transport_len = host_len;
	}

	pStream = php_stream_xport_create( transport, transport_len,
			options, flags,
			NULL, NULL, NULL, &errstr, &errcode);

	if( errstr )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "[%d] %s", errcode, errstr);
	    efree(errstr);
	}

	if( !pStream )
	{
		RETURN_FALSE;
	}

	BEANSTALK_G(gstream) = pStream;
	ZEND_REGISTER_RESOURCE(return_value,pStream,le_beanstalk);
}

/**
 * parse data from response like
 * FOUND <id> <bytes>\r\n
 * <data>\r\n
 */
static __inline__ void getResponseComplexData(
										php_stream* pStream,
										zval** return_value,
										char* pCmd )
{
	if( !php_stream_write_string( pStream, pCmd ))
	{
		ZVAL_FALSE( *return_value );

		return;
	}

	char *pResponse = NULL;
	char *pRet = NULL;
	char *token;
	int i = 0;
	int jobID;

	if( pResponse = php_stream_get_line( pStream, NULL, 0, NULL ) )
	{
		php_printf( "%s=%s\r\n", pCmd, pResponse );

		if( !strncmp( pResponse, RESPONSE_DATA_FOUND, strlen( RESPONSE_DATA_FOUND )) ||
			!strncmp( pResponse, RESPONSE_DATA_RESERVED, strlen( RESPONSE_DATA_RESERVED )))
		{
			token = strtok( pResponse, COMMAND_SEPARATOR );
			while( token )
			{
				if( i == 1 )
				{//id
					jobID = atoi( token );
				}
				else if( i == 2 )
				{//length
					pRet = php_stream_gets( pStream, NULL, 0 );
				}
				++i;
				token = strtok( NULL, COMMAND_SEPARATOR );
			}

			array_init( *return_value );
			add_assoc_long( *return_value, "id", jobID );
			pRet[ strlen(pRet) - 2 ] = '\0';

			add_assoc_string( *return_value, "data", pRet, 1 );
			efree( pRet );
			efree( pResponse );
		}
		else
		{
			ZVAL_STRING( *return_value, pResponse, 1 );
			efree( pResponse );
		}
	}
	else
	{
		ZVAL_FALSE(*return_value );
	}
}

/**
 * beanstalk_peek( $resource, $lJobID )
 * @param $resource		resource id
 * @param $lJodID		job id
 *
 * return false for error string data for success
 */
PHP_FUNCTION(beanstalk_peek)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	long lJobID = -1;
	char* pWBuf;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zStream, &lJobID ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream || lJobID < 0 )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the params!");
		RETURN_FALSE;
	}

	spprintf(&pWBuf, 0, COMMAND_PEEK " %d" CRLF, lJobID );

	getResponseComplexData( pStream, &return_value, pWBuf );
	efree( pWBuf );
}


/**
 * beanstalk_peek( $resource, $strTube = 'default' )
 * @param $resource		resource id
 * @param $strTube		tube name
 *
 * return array data for success others for failure
 */
PHP_FUNCTION(beanstalk_peekReady)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	char* pTube = "default";
	int tubeLen = 0;
	char* pWBuf = NULL;//COMMAND_PEEK_READY CRLF;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|s", &zStream, &pTube, &tubeLen ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the first param!");
		RETURN_FALSE;
	}

	if( tubeLen )
	{
		spprintf( &pWBuf, 0, COMMAND_USE " %s" CRLF, pTube );

		if( -1 == useTube( pStream, pWBuf, &return_value, TUBE_NOT_RETURN ))
		{
			efree( pWBuf );
			RETURN_FALSE;
		}
		efree( pWBuf );
	}

	spprintf( &pWBuf, 0, COMMAND_PEEK_READY CRLF );
	getResponseComplexData( pStream, &return_value, pWBuf );
	efree( pWBuf );
}

/**
 * beanstalk_delete( $resource, $jobID )
 * @param $resource
 * @param $jobID
 *
 * return true for success false for failue
 * delete jobid\r\n
 */
PHP_FUNCTION(beanstalk_delete)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	long lJobID = 0;
	char* pWBuf;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zStream, &lJobID ) == FAILURE) {
		RETURN_FALSE;
	}

	if( lJobID <= 0 ){
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the second param!");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the first param!");
		RETURN_FALSE;
	}

	spprintf(&pWBuf, 0, "delete %d\r\n", lJobID );
	getResponseWithNoData( pStream, &return_value, pWBuf, RESPONSE_NO_DATA_DELETED );
	efree( pWBuf );
}

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
PHP_FUNCTION(beanstalk_put)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	char* pStr = NULL;
	int msgLen = 0;
	int iPri = 1024;
	int iDelay = 0;
	int iTtr = 60;

	char* pWBuf;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|lll", &zStream, &pStr, &msgLen ) == FAILURE) {
		RETURN_FALSE;
	}

	if( msgLen == 0 ){
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the second param!");
		RETURN_FALSE;
	}
	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the first param!");
		RETURN_FALSE;
	}

	spprintf(&pWBuf, 0, "put %d %d %d %d" CRLF "%s" CRLF, iPri, iDelay, iTtr, msgLen, pStr );
	getResponseIntData( pStream, &return_value, pWBuf, RESPONSE_INT_DATA_INSERTED );
	efree( pWBuf );
}



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
PHP_FUNCTION(beanstalk_putInTube)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	char* pStrMsg = NULL;
	char* pTube = NULL;
	int iTubeLen = 0;
	int msgLen = 0;
	long lPri = 1024;
	long lDelay = 0;
	long lTtr = 60;
	char* pWBuf;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss|lll", &zStream, &pTube, &iTubeLen, &pStrMsg, &msgLen ) == FAILURE) {
		RETURN_FALSE;
	}

	if( msgLen == 0 || iTubeLen == 0 ){
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the params!");
		RETURN_FALSE;
	}
	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the first param!");
		RETURN_FALSE;
	}

	spprintf( &pWBuf, 0, COMMAND_USE " %s" CRLF, pTube );
	if( -1 == useTube( pStream, pWBuf, &return_value, TUBE_NOT_RETURN ))
	{
		efree( pWBuf );
		RETURN_FALSE;
	}
	efree( pWBuf );

	spprintf(&pWBuf, 0, "put %d %d %d %d" CRLF "%s" CRLF, lPri, lDelay, lTtr, msgLen, pStrMsg );
	getResponseIntData( pStream, &return_value, pWBuf, RESPONSE_INT_DATA_INSERTED );
	efree( pWBuf );
}


/**
 * beanstalk_status( $resource )
 * @param $resource
 *
 * return stats array
 */
PHP_FUNCTION(beanstalk_stats)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	char* pWBuf = COMMAND_STATS CRLF;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zStream ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the param!");
		RETURN_FALSE;
	}

	getStatsResponse( pStream, pWBuf, &return_value );
}

/**
 * beanstalk_statsJob( $resource, $lJobID )
 *
 * @param $resource
 * @param $lJobID
 *
 * return false for failure job's status array
 */
PHP_FUNCTION(beanstalk_statsJob)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	long iJobID = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zStream, &iJobID ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream || iJobID < 0 )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the params!");
		RETURN_FALSE;
	}

	char* pWBuf = NULL;
	spprintf( &pWBuf, 0, "%s %d" CRLF, COMMAND_STATS_JOB, iJobID );
	getStatsResponse( pStream, pWBuf, &return_value );
	efree( pWBuf );
}

/**
 * beanstalk_statsTube( $resource, $strTube )
 *
 * @param $resource
 * @param $strTube tube name
 *
 * return tube's status array for success false for failure
 */
PHP_FUNCTION(beanstalk_statsTube)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	char* pTube = NULL;
	int iTubeLen = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zStream, &pTube, &iTubeLen ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream || 0 == iTubeLen )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the params!");
		RETURN_FALSE;
	}

	char* pWBuf = NULL;
	spprintf( &pWBuf, 0, COMMAND_STATS_TUBE " %s" CRLF, pTube );
	getStatsResponse( pStream, pWBuf, &return_value );
	efree( pWBuf );
}

/**
 * 响应后不带数据如DELETED TOUCHED等
 *
 * 形如： "TOUCHED\r\n" to indicate success.
 */
static void getResponseWithNoData( php_stream* pStream, zval** return_value, char* pCmd, int iResponseType )
{
	if( !php_stream_write_string( pStream, pCmd ))
	{
		ZVAL_FALSE( *return_value );
		return;
	}

	char* pRet = php_stream_gets( pStream, NULL, 0 );

	if( pRet )
	{
//		php_printf( "%s=%s\r\n", pCmd, pRet );
		switch( iResponseType )
		{
			case RESPONSE_NO_DATA_BURIED:
				if( !strncmp( RESPONSE_BURIED, pRet, strlen( RESPONSE_BURIED ) ))
				{
					efree( pRet );
					ZVAL_TRUE( *return_value );
					return;
				}
				ZVAL_FALSE( *return_value );
				return;
			case RESPONSE_NO_DATA_DELETED:
				if( !strncmp( RESPONSE_DELETED, pRet, strlen( RESPONSE_DELETED ) ))
				{
					efree( pRet );
					ZVAL_TRUE( *return_value );
					return;
				}
				ZVAL_FALSE( *return_value );
				return;
			case RESPONSE_NO_DATA_KICKED:
				if( !strncmp( RESPONSE_KICKED, pRet, strlen( RESPONSE_KICKED ) ))
				{
					efree( pRet );
					ZVAL_TRUE( *return_value );
					return;
				}
				ZVAL_FALSE( *return_value );
				return;
			case RESPONSE_NO_DATA_PAUSED:
				if( !strncmp( RESPONSE_PAUSED, pRet, strlen( RESPONSE_PAUSED ) ))
				{
					efree( pRet );
					ZVAL_TRUE( *return_value );
					return;
				}
				ZVAL_FALSE( *return_value );
				return;
			case RESPONSE_NO_DATA_RELEASED:
				if( !strncmp( RESPONSE_RELEASED, pRet, strlen( RESPONSE_RELEASED ) ))
				{
					efree( pRet );
					ZVAL_TRUE( *return_value );
					return;
				}
				ZVAL_FALSE( *return_value );
				return;
			case RESPONSE_NO_DATA_TOUCHED:
				if( !strncmp( RESPONSE_TOUCHED, pRet, strlen( RESPONSE_TOUCHED ) ))
				{
					efree( pRet );
					ZVAL_TRUE( *return_value );
					return;
				}
				ZVAL_FALSE( *return_value );
				return;

			default:
				ZVAL_FALSE( *return_value );
				return;
		}

		ZVAL_FALSE( *return_value );
		return;
	}

	efree( pRet );
	ZVAL_FALSE( *return_value );
}

/**
 * beanstalk_bury( $resource, $lJobID, $lPri = 1024 )
 *
 * @param $resource
 * @param $lJobID
 * @param $lPri = 1024
 *
 * return false for failure true for success
 */
PHP_FUNCTION(beanstalk_bury)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	long lJobID = -1;
	long lPri = 1024;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|l", &zStream, &lJobID, &lPri ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream || -1 == lJobID )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the params!");
		RETURN_FALSE;
	}

	char* pWBuf = NULL;
	spprintf( &pWBuf, 0, COMMAND_BURY " %d %d" CRLF, lJobID, lPri );

	getResponseWithNoData( pStream, &return_value, pWBuf, RESPONSE_NO_DATA_BURIED );

	efree( pWBuf );
}

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
PHP_FUNCTION(beanstalk_ignore)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	char* strTube = NULL;
	int iTubeLen = 0;
	char* pWBuf = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zStream, &strTube, &iTubeLen ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the params!");
		RETURN_FALSE;
	}

	if( !iTubeLen )
	{
		RETURN_FALSE;
	}
	else
	{
		if( !strcmp( strTube, "default" ))
		{
			RETURN_FALSE;
		}

		spprintf( &pWBuf, 0, COMMAND_IGNORE " %s" CRLF, strTube );
	}

	getResponseIntData( pStream, &return_value, pWBuf, RESPONSE_INT_DATA_WATCHING );
	efree( pWBuf );
}

/**
 * 响应中带整型数据
 * 形如： "INSERTED <id>\r\n"
 */
static void getResponseIntData( php_stream* pStream, zval** return_value, char* pCmd, int iResType )
{
	size_t sRet;
	char* pRes = NULL;
	int iRetry = 0;

	do
	{
		if( !php_stream_write_string( pStream, pCmd ))
		{
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"network error occur!");
			ZVAL_FALSE( *return_value );
			return;
		}

		char* pRes = php_stream_get_line( pStream, NULL, 0, &sRet );

		if( !strncmp( pRes, RESPONSE_WATCHING, strlen( RESPONSE_WATCHING ) ) ||
				!strncmp( pRes, RESPONSE_KICKED, strlen( RESPONSE_KICKED ) ) ||
				!strncmp( pRes, RESPONSE_INSERTED, strlen( RESPONSE_INSERTED ) ) )
		{
			switch( iResType )
			{
				case RESPONSE_INT_DATA_WATCHING:
					ZVAL_LONG( *return_value, atoi( pRes + strlen( RESPONSE_WATCHING ) + 1 ) );

					break;
				case RESPONSE_INT_DATA_KICKED:
					ZVAL_LONG( *return_value, atoi( pRes + strlen( RESPONSE_KICKED ) + 1 ) );

					break;
				case RESPONSE_INT_DATA_INSERTED:
					ZVAL_LONG( *return_value, atoi( pRes + strlen( RESPONSE_INSERTED ) + 1 ) );

					break;

				default:
					efree( pRes );
					ZVAL_FALSE( *return_value );
					return;
			}
			efree( pRes );

			return;
		}


		if( pRes )
		{
			efree( pRes );
		}
	}
	while( ++iRetry < RETRY_TIMES );//retry three times

	if( pRes )
	{
		efree( pRes );
	}

	ZVAL_FALSE( *return_value );
}


/**
 * beanstalk_kick( $resource, $max = 30 )
 *
 * @param	resource	$resource
 * @param 	int			$max
 *
 * return false for error others for kicked jobs
 */
PHP_FUNCTION(beanstalk_kick)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	long lMax = 30;
	char* pWBuf = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zStream, &lMax ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the params!");
		RETURN_FALSE;
	}

	if( lMax <= 0 )
	{
		lMax = 30;
	}

	spprintf( &pWBuf, 0, COMMAND_KICK " %d" CRLF, lMax );

	getResponseIntData( pStream, &return_value, pWBuf, RESPONSE_INT_DATA_KICKED );

	efree( pWBuf );
}

/**
 * beanstalk_kickJob( $resource, $iJobID )
 *
 * @param	resource	$resource
 * @param	int			$iJobID
 *
 * return true for success false for error or not found etc.
 */
PHP_FUNCTION(beanstalk_kickJob)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	long lJobID = -1;
	char* pWBuf = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zStream, &lJobID ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream || lJobID < 0 )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the params!");
		RETURN_FALSE;
	}

	spprintf( &pWBuf, 0, COMMAND_KICK_JOB " %d" CRLF, lJobID );

	getResponseWithNoData( pStream, &return_value, pWBuf, RESPONSE_NO_DATA_KICKED );

	efree( pWBuf );
}

/**
 * 0 for success
 * others for failure
 *
 * e.g.：OK <bytes>\r\n
 *	    <data>\r\n
 *
 * this data format is not same as beanstalk_peek ...
 */
static int getListTubeResponse( php_stream* pStream, zval** return_value, char* pCmd )
{
	if( !php_stream_write_string( pStream, pCmd ))
	{
		return -1;
	}

	size_t sRet = 0;
	char* pResponse = php_stream_get_line( pStream, NULL, 0, &sRet );
	size_t iLen = 0;
	char *pRet = NULL;
	char *token = NULL;
	int i = 0;

	if( pResponse && strncmp( pResponse, RESPONSE_OK, 2 ))
	{
		efree( pResponse );
		return -1;
//		php_printf( "%s=%d", pResponse, sRet );
	}
	token = strtok( pResponse, COMMAND_SEPARATOR );
	while( token )
	{
		if( i == 1 )
		{//id
			iLen = atoi( token );

			sRet = iLen;
			iLen = 0;
			pRet = emalloc( sRet );
			while( iLen < sRet && !php_stream_eof( pStream ))
			{
				iLen += php_stream_read( pStream, pRet + iLen, sRet );
			}

			if( iLen > 0 )
			{
				array_init( *return_value );
				i = 0;
				token = strtok( pRet + 2, CRLF );
				while( token )
				{
					if( !i++ )
					{
						token = strtok( NULL, CRLF );

						continue;
					}

					if( *token != '-' )
					{
						token = strtok( NULL, CRLF );

						continue;
					}

					add_next_index_stringl( *return_value, token + 2, strlen( token ) - 2, 1 );
					token = strtok( NULL, CRLF );
				}

				efree( pRet );
			}

			return 0;
		}
		++i;
		token = strtok( NULL, COMMAND_SEPARATOR );
	}

	efree( pResponse );

	ZVAL_FALSE( *return_value );

	return -1;
//	RETURN_FALSE;
}

/**
 * beanstalk_listTubes( $resource )
 * @param $resource
 *
 * return array for success false for failure
 */
PHP_FUNCTION(beanstalk_listTubes)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	char* pWBuf = COMMAND_LIST_TUBES CRLF;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zStream ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the param!");
		RETURN_FALSE;
	}


	int iTry = 0;
	int iRet = 0;
	do
	{
		iRet = getListTubeResponse( pStream, &return_value, pWBuf );
	}while( ++iTry < 3 );

	if( iRet )
	{
		RETURN_FALSE;
	}
}

/**
 * beanstalk_listTubes( $resource )
 * @param $resource
 *
 * return array for success false for failure
 */
PHP_FUNCTION(beanstalk_listTubesWatched)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	char* pWBuf = COMMAND_LIST_TUBES_WATCHED CRLF;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zStream ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the param!");
		RETURN_FALSE;
	}

	int iTry = 0;
	int iRet = 0;
	do
	{
		iRet = getListTubeResponse( pStream, &return_value, pWBuf );
	}while( ++iTry < 3 );

	if( iRet )
	{
		RETURN_FALSE;
	}

}

/**
 * beanstalk_listTubeUsed( $resource, $bAskServer = 0 )
 * @param $resource
 * @param $bAskServer
 *
 * return string or true for success others for failure
 */
PHP_FUNCTION(beanstalk_listTubeUsed)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	long lAskServer = 0;
	char* pWBuf = COMMAND_LIST_TUBE_USED CRLF;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &zStream, &lAskServer ) == FAILURE) {
		RETURN_FALSE;
	}

	if( !lAskServer )
	{
		RETURN_STRING( "default", 1 );
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the param!");
		RETURN_FALSE;
	}

	useTube( pStream, pWBuf, &return_value, lAskServer );

}

static void pauseResumeTube( php_stream* pStream, zval** return_value, char* pStrTube, long lDelay )
{
	char* pWBuf = NULL;

	spprintf( &pWBuf, 0, COMMAND_PAUSE_TUBE " %s %d" CRLF, pStrTube, lDelay );

	getResponseWithNoData( pStream, return_value, pWBuf, RESPONSE_NO_DATA_PAUSED );

	efree( pWBuf );
}

/**
 * beanstalk_pauseTube( $resource, $strTube, $lDelay )
 *
 * @param $resource
 * @param $strTube tube name
 * @param $lDelay delay for pause
 *
 * return false for failure true for success
 */
PHP_FUNCTION(beanstalk_pauseTube)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	char* pStrTube = NULL;
	int iTubeLen = -1;
	long lDelay = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsl", &zStream, &pStrTube, &iTubeLen, &lDelay ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream || iTubeLen < 0 )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the params!");
		RETURN_FALSE;
	}

	if( lDelay < 0 )
	{
		lDelay = 0;
	}

	pauseResumeTube( pStream, &return_value, pStrTube, lDelay );
}

/**
 *	beanstalk_resumeTube( $resource, $strTube )
 *
 *	@param $resource
 *	@param $strTube tube name
 *
 *	return false for success true for success
 */
PHP_FUNCTION(beanstalk_resumeTube)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	char* pStrTube = NULL;
	int iTubeLen = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zStream, &pStrTube, &iTubeLen ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream || iTubeLen < 0 )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the params!");
		RETURN_FALSE;
	}

	pauseResumeTube( pStream, &return_value, pStrTube, 0 );
}

/**
 * beanstalk_peekDelayed( $resource, $strTube = 'default')
 *
 * @param $resource
 * @param $strTube tube name
 *
 * return false for error others for success
 */
PHP_FUNCTION(beanstalk_peekDelayed)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	char* pTube = "default";
	int tubeLen = 0;
	char* pWBuf = NULL;//COMMAND_PEEK_DELAYED CRLF;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|s", &zStream, &pTube, &tubeLen ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the first param!");
		RETURN_FALSE;
	}

	if( !tubeLen && !strncmp( pTube, "default", sizeof( "default" ) - 1))
	{
		spprintf( &pWBuf, 0, COMMAND_USE " %s" CRLF, pTube );
		if( -1 == useTube( pStream, pWBuf, &return_value, TUBE_NOT_RETURN ))
		{
			efree( pWBuf );
			RETURN_FALSE;
		}
		efree( pWBuf );

	}

	spprintf( &pWBuf, 0, COMMAND_PEEK_DELAYED CRLF );
	getResponseComplexData( pStream, &return_value, pWBuf );
	efree( pWBuf );
}

/**
 * beanstalk_peekBuried( $resource, $strTube = "default" )
 *
 * @param $resource
 * @param $strTube tube name
 *
 * return false for error others for success
 */
PHP_FUNCTION(beanstalk_peekBuried)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	char* pTube = NULL;
	int tubeLen = 0;
	char* pWBuf = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|s", &zStream, &pTube, &tubeLen ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the first param!");
		RETURN_FALSE;
	}

	if( !tubeLen && !strncmp( pTube, "default", sizeof( "default" ) - 1))
	{
		spprintf( &pWBuf, 0, COMMAND_USE " %s" CRLF, pTube );
		if( -1 == useTube( pStream, pWBuf, &return_value, TUBE_NOT_RETURN ))
		{
			efree( pWBuf );
			RETURN_FALSE;
		}
		efree( pWBuf );

	}

	if( !php_stream_write_string( pStream, pWBuf ))
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"network error occur!");
		RETURN_FALSE;
	}

	spprintf( &pWBuf, 0, COMMAND_PEEK_BURIED CRLF );
	getResponseComplexData( pStream, &return_value, pWBuf );
	efree( pWBuf );
}

/**
 * beanstalk_touch( $resource, $jobID )
 * @param resource $resource
 * @param long	   $jobID
 *
 * return
 * true for success
 * false for the job does not exist or is not reserved by the client.
 */
PHP_FUNCTION(beanstalk_touch)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	long lJobID = 0;
	char* pWBuf = NULL;//COMMAND_TOUCH CRLF;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zStream, &lJobID ) == FAILURE) {
		RETURN_FALSE;
	}

	if( !lJobID )
	{
		//for error
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the params!");
		RETURN_FALSE;
	}

	spprintf( &pWBuf, 0, COMMAND_TOUCH " %d" CRLF, lJobID );

	getResponseWithNoData( pStream, &return_value, pWBuf, RESPONSE_NO_DATA_TOUCHED );

	efree( pWBuf );
}

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
PHP_FUNCTION(beanstalk_release)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	long lJobID = 0;
	long lPri = 1024;
	long lDelay = 0;
	char* pWBuf = NULL;//COMMAND_TOUCH CRLF;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|ll", &zStream, &lJobID, &lPri, &lDelay ) == FAILURE) {
		RETURN_FALSE;
	}

	if( !lJobID )
	{
		//for error
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the params!");
		RETURN_FALSE;
	}

	spprintf( &pWBuf, 0, COMMAND_RELEASE " %d %d %d" CRLF, lJobID, lPri, lDelay );

	getResponseWithNoData( pStream, &return_value, pWBuf, RESPONSE_NO_DATA_RELEASED );
	efree( pWBuf );

}

/**
 * beanstalk_reserve( $rid, $lTimeOut )
 *
 * @param	resource	$rid
 * @param	long		$lTimeOut for seconds
 *
 * return array()
 */
PHP_FUNCTION(beanstalk_reserve)
{
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	long lTimeOut = 0;
	char* pWBuf = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &zStream, &lTimeOut ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the params!");
		RETURN_FALSE;
	}

	if( lTimeOut <= 0 )//reserve
	{
		spprintf(&pWBuf, 0, COMMAND_RESERVE CRLF );
	}
	else
	{
		spprintf(&pWBuf, 0, COMMAND_RESERVE_WITH_TIMEOUT " %d" CRLF, lTimeOut );
	}

	if( !php_stream_write_string( pStream, pWBuf ))
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "network error occur!");
		RETURN_FALSE;
	}

	getResponseComplexData( pStream, &return_value, pWBuf );
	efree( pWBuf );

}

//PHP_FUNCTION(beanstalk_reserveFromTube)
//{
//}


/**
 * beanstalk_useTube( @resource, $strTube )
 *
 * @param $resource
 * @param $strTube
 *
 * return false for error others for true
 */
PHP_FUNCTION(beanstalk_useTube)
{//
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	char* strTube = NULL;
	int iTubeLen = 0;
	char* pWBuf = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zStream, &strTube, &iTubeLen ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream || iTubeLen == 0)
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the params!");
		RETURN_FALSE;
	}

	spprintf( &pWBuf, 0, COMMAND_USE " %s" CRLF, strTube );

	if( !useTube( pStream, pWBuf, &return_value, TUBE_RETURN ))
	{
		efree( pWBuf );
		return;
	}

	efree( pWBuf );
}

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
PHP_FUNCTION(beanstalk_watch)
{//
	php_stream* pStream = NULL;
	zval* zStream = NULL;
	char* strTube = NULL;
	long bWatchSize = 0;
	int iTubeLen = 0;
	char* pWBuf = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|sl", &zStream, &strTube, &iTubeLen, &bWatchSize ) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE( pStream, php_stream*,&zStream, -1,PHP_DESCRIPTOR_BEANSTALK_RES_NAME, le_beanstalk);

	if( !zStream || !pStream )
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invalid param provided, check the params!");
		RETURN_FALSE;
	}

	if( !iTubeLen )
	{
		if( bWatchSize )
		{
			spprintf( &pWBuf, 0, COMMAND_WATCH " %s" CRLF, "default" );
		}
		else
		{
			RETURN_TRUE;//by default watch default tube
		}
	}
	else
	{
		spprintf( &pWBuf, 0, COMMAND_WATCH " %s" CRLF, strTube );
	}

	int iRetry = 0;
	do
	{
		if( !php_stream_write_string( pStream, pWBuf ))
		{
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"network error occur!");
			RETURN_FALSE;
		}

		size_t sRet;
		char* pRes = php_stream_get_line( pStream, NULL, 0, &sRet );

		if( !strncmp( pRes, RESPONSE_WATCHING, strlen( RESPONSE_WATCHING ) ))
		{
			if( bWatchSize )
			{
				RETVAL_LONG( atoi( pRes + strlen( RESPONSE_WATCHING ) + 1 ) );
				efree( pRes );
				efree( pWBuf );

				return;
			}
			else
			{
				efree( pRes );
				efree( pWBuf );

				RETURN_TRUE;
			}
		}


		efree( pRes );
	}
	while( ++iRetry < RETRY_TIMES );

	efree( pWBuf );
	RETURN_FALSE;
};

//PHP_FUNCTION(beanstalk_watchOnly)
//{
//}


/* {{{ php_beanstalk_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_beanstalk_init_globals(zend_beanstalk_globals *beanstalk_globals)
{
	beanstalk_globals->global_value = 0;
	beanstalk_globals->global_string = NULL;
}
*/
/* }}} */
//static int le_beanstalk_stream;

static void php_beanstalk_phpstream_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_stream *stream = (php_stream*)rsrc->ptr;
	if( stream )
		php_stream_close( stream );
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(beanstalk)
{
	le_beanstalk = zend_register_list_destructors_ex( php_beanstalk_phpstream_dtor, NULL, PHP_DESCRIPTOR_BEANSTALK_RES_NAME, module_number );

	/*
	 * list return
	 */
	REGISTER_LONG_CONSTANT("BEANSTALK_TUBE_RETURN", TUBE_RETURN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("BEANSTALK_TUBE_NOT_RETURN", TUBE_NOT_RETURN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("BEANSTALK_WATCH_SIZE_RETURN", RETURN_WATCH_SIZE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("BEANSTALK_WATCH_SIZE_NOT_RETURN", NOT_RETURN_WATCH_SIZE, CONST_CS | CONST_PERSISTENT);

	/* If you have INI entries, uncomment these lines
	 * REGISTER_INI_ENTRIES();
	 */


	BEANSTALK_G(gstream) = NULL;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(beanstalk)
{
	/*
	 * uncomment this line if you have INI entries
	 */
//	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(beanstalk)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(beanstalk)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(beanstalk)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "beanstalk support", "enabled");
	php_info_print_table_row(2, "Version", PHP_BEANSTALK_VERSION);
	php_info_print_table_row(2, "Supported php version", SUPPORTED_PHP_VERSION );
	php_info_print_table_row(2, "Author", "Bruce Tsisen" );
	php_info_print_table_row(2, "Supports", "qzfzz@163.com");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}

/**
 * @param php_stream 	*pStream
 * @param char* 		pCmd
 *
 *
 *  return
 *  0: for success
 *  1: for network error
 *  2: for response error
 */
static __inline__ void getStatsResponse( php_stream* pStream, char* pCmd, zval** return_value )
{
	size_t sRet = 0;
	char *pResponse = NULL;
	size_t iLen = 0;
	char *pRet = NULL;
	char *token = NULL;

	int iPos = 0;
	char *pPos;
	int i = 0;
	int iRetry = 0;

retry:

	if( !php_stream_write_string( pStream, pCmd ))
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"network error occur!");
		ZVAL_FALSE(*return_value);
		return;
	}

	if( pResponse = php_stream_get_line( pStream, NULL, 0, &sRet ) )
	{
		//retry three times
		if( 2 == strlen( pResponse ) && strncmp( pResponse, RESPONSE_DATA_OK, 2 ) && ++iRetry < 4 )
		{
			goto retry;
		}

		if( !strncmp( pResponse, RESPONSE_DATA_OK, 2 ))
		{//RESPONSE_DATA_OK
			token = strtok( pResponse, COMMAND_SEPARATOR );
			while( token )
			{
				if( i == 1 )
				{//id
					iLen = atoi( token );
					sRet = iLen;
					iLen = 0;
					pRet = emalloc( sRet );

					while( iLen < sRet && !php_stream_eof( pStream ))
					{
						iLen += php_stream_read( pStream, pRet + iLen, sRet - iLen );
					}

					break;
				}
				++i;
				token = strtok( NULL, COMMAND_SEPARATOR );
			}

			memset( pRet + sRet, '\0', strlen( pRet ) - iLen );

			array_init( *return_value );
			token = strtok( pRet, "\r\n" );
			i = 0;
			BSKeyVal bsKV;

			while( token )
			{
				if( i++ == 0 )
				{
					token = strtok( NULL, "\r\n" );
					continue;
				}

				bsKV.key.pStr = token;
				pPos = strchr( token, ':' );

				if( pPos )
				{
					iPos = pPos - token;
					bsKV.key.iLen = iPos + 1;
				}

				bsKV.value.pStr = token + iPos + 2;
				bsKV.value.iLen = strlen( token ) - iPos - 1;
				add_assoc_stringl_ex( *return_value, bsKV.key.pStr, bsKV.key.iLen, bsKV.value.pStr, bsKV.value.iLen, 1 );
				token = strtok( NULL, "\r\n" );
			}

			if( pRet )
			{
				efree( pRet );
			}

			efree( pResponse );

			return;
		}

		if( pRet )
		{
			efree( pRet );
		}

		efree( pResponse );
	}
	else
	{
		if( ++iRetry < RETRY_TIMES )
		{
			goto retry;
		}
		// for error
		ZVAL_FALSE(*return_value);
		return;
	}
}

/**
 * return 0 for success -1 for failure
 */
static int useTube( php_stream *pStream, char* pCmd, zval** return_value, int bList )
{
	int i = 0;

	do
	{
		if( !php_stream_write_string( pStream, pCmd ))
		{
			php_printf( "error\r\n" );
			ZVAL_FALSE( *return_value );
			return -1;
		}

		size_t sRet;
		char* pRes = php_stream_get_line( pStream, NULL, 0, &sRet );

		if( !strncmp( pRes, RESPONSE_USING, strlen( RESPONSE_USING ) ))
		{
			if( bList )
			{
				ZVAL_STRINGL( *return_value, pRes + strlen( RESPONSE_USING ) + 1, sRet - strlen( RESPONSE_USING ) - 3, 1 );
			}
			else
			{
				ZVAL_TRUE( *return_value );
			}
			efree( pRes );

			return 0;
		}
		else if( ++i < RETRY_TIMES )
		{
			--i;
			continue;
		}

		if( pRes )
		{
			efree( pRes );
		}
	}
	while( ++i < RETRY_TIMES );//retry three times

	ZVAL_FALSE( *return_value );
	return -1;
}



/* }}} */

/* {{{ beanstalk_functions[]
 *
 * Every user visible function must have an entry in beanstalk_functions[].
 */
const zend_function_entry beanstalk_functions[] = {
//	PHP_FE(confirm_beanstalk_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(beanstalk_open,	NULL)
	PHP_FE(beanstalk_close,	NULL)
	PHP_FE(beanstalk_put,	NULL)
	PHP_FE(beanstalk_peekReady,	NULL)
	PHP_FE(beanstalk_peek,	NULL)
	PHP_FE(beanstalk_delete, NULL)
	PHP_FE(beanstalk_stats, NULL)
	PHP_FE(beanstalk_bury, NULL)
	PHP_FE(beanstalk_ignore, NULL)
	PHP_FE(beanstalk_kick, NULL)
	PHP_FE(beanstalk_kickJob, NULL)
	PHP_FE(beanstalk_listTubes, NULL)
	PHP_FE(beanstalk_listTubesWatched, NULL)
	PHP_FE(beanstalk_listTubeUsed, NULL)
	PHP_FE(beanstalk_pauseTube, NULL)
	PHP_FE(beanstalk_resumeTube, NULL)
	PHP_FE(beanstalk_peekDelayed, NULL)
	PHP_FE(beanstalk_peekBuried, NULL)
	PHP_FE(beanstalk_putInTube, NULL)
	PHP_FE(beanstalk_release, NULL)
	PHP_FE(beanstalk_reserve, NULL)
	//PHP_FE(beanstalk_reserveFromTube, NULL)
	PHP_FE(beanstalk_statsJob, NULL)
	PHP_FE(beanstalk_statsTube, NULL)
	PHP_FE(beanstalk_touch, NULL)
	PHP_FE(beanstalk_useTube, NULL)
	PHP_FE(beanstalk_watch, NULL)
	//PHP_FE(beanstalk_watchOnly, NULL)
	PHP_FE_END	/* Must be the last line in beanstalk_functions[] */
};
/* }}} */

/* {{{ beanstalk_module_entry
 */
zend_module_entry beanstalk_module_entry = {
	STANDARD_MODULE_HEADER,
	"beanstalk",
	beanstalk_functions,
	PHP_MINIT(beanstalk),
	PHP_MSHUTDOWN(beanstalk),
	PHP_RINIT(beanstalk),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(beanstalk),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(beanstalk),
	PHP_BEANSTALK_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_BEANSTALK
ZEND_GET_MODULE(beanstalk)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
