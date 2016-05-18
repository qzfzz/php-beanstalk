--TEST--
Check for reserve
--SKIPIF--
<?php if (!extension_loaded("beanstalk")) print "skip"; ?>
--FILE--
<?php 
$arrConfig = include __DIR__ . '/../include/config.inc';
$b = beanstalk_connect( $arrConfig['host'], $arrConfig['port'] );
$jobID = beanstalk_putInTube( $b, 'test-tube-a', 'test' );

var_dump( beanstalk_pauseTube( $b, 'test-tube-a', 100 ));
var_dump( beanstalk_resumeTube( $b, 'test-tube-a' ));

var_dump( $b->pauseTube( 'test-tube-a', 100 ));
var_dump( $b->resumeTube( 'test-tube-a' ));

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
bool(true)
bool(true)
bool(true)
bool(true)