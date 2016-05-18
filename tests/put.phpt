--TEST--
test for beanstalk put putInTube
--SKIPIF--
<?php if (!extension_loaded("beanstalk")) print "skip"; ?>
--FILE--
<?php 
$arrConfig = include __DIR__ . '/../include/config.inc';
$b = beanstalk_open( $arrConfig[ 'host' ], $arrConfig[ 'port' ] );
$lPut = beanstalk_put( $b, "hello this is a test" );
$lPutInTube = beanstalk_putInTube( $b, 'tubea', "abc" );
var_dump( $lPut );
var_dump( $lPutInTube );

var_dump( beanstalk_delete( $b, $lPut));
var_dump( beanstalk_delete( $b, $lPutInTube ));

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
int(%d)
int(%d)
bool(true)
bool(true)