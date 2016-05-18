--TEST--
Check for beanstalk presence
--SKIPIF--
<?php if (!extension_loaded("beanstalk")) print "skip"; ?>
--FILE--
<?php 
$arrConfig = include __DIR__ . '/../include/config.inc';
$b = beanstalk_connect( $arrConfig['host'], $arrConfig['port'] );

$jobID = beanstalk_put( $b, "message" );
$job = beanstalk_peekReady( $b );
$strStats = var_export( beanstalk_statsJob( $b, $job['id'] ), true );
var_dump( preg_match( "/array/", $strStats ));

$strStats = var_export( $b->statsJob( $job['id'] ), true );
var_dump( preg_match( "/array/", $strStats ));
beanstalk_delete( $b, $jobID );

beanstalk_close( $b );
/*
	you can add regression tests for your extension here

  the output of your test code has to be equal to the
  text in the --EXPECT-- section below for the tests
  to pass, differences between the output and the
  expected text are interpreted as failure

	see php5/README.TESTING for further information on
  writing regression tests 				
done
*/
?>
--EXPECTF--
int(1)
int(1)