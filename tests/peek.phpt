--TEST--
test for beanstalk peek peekReady
--SKIPIF--
<?php if (!extension_loaded("beanstalk")) print "skip"; ?>
--FILE--
<?php 
$arrConfig = include __DIR__ . '/../include/config.inc';
$b = beanstalk_connect( $arrConfig['host'], $arrConfig['port'] );

//op
$lJobID = beanstalk_put( $b, "message" );
$strPeek = var_export( beanstalk_peek( $b, $lJobID ), true );
var_dump( preg_match( "/array/", $strPeek ));
$strPeek = var_export( beanstalk_peekReady( $b ), true );
var_dump( preg_match( "/array/", $strPeek ));
beanstalk_delete( $b, $lJobID );
//beanstalk_close( $b );

//oo
$lJobID = $b->put( "message" );
$strPeek = var_export( $b->peek( $lJobID ), true );
var_dump( preg_match( "/array/", $strPeek ));
$strPeek = var_export( $b->peekReady(), true );
var_dump( preg_match( "/array/", $strPeek ));
beanstalk_delete( $b, $lJobID );

beanstalk_close( $b );

/*
	you can add regression tests for your extension here

  the output of your test code has to be equal to the
  text in the --EXPECT-- section below for the tests
  to pass, differences between the output and the
  expected text are interpreted as failure

	see php5/README.TESTING for further information on
  writing regression tests 				
done    resource(%d) of type (stream)
*/
?>
--EXPECTF--
int(1)
int(1)
int(1)
int(1)