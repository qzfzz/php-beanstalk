--TEST--
Check for watch() and ignore()
--SKIPIF--
<?php if (!extension_loaded("beanstalk")) print "skip"; ?>
--FILE--
<?php 
$arrConfig = include __DIR__ . '/../include/config.inc';

$b = beanstalk_connect( $arrConfig['host'], $arrConfig['port'] );
beanstalk_putInTube( $b, 'test-tube-a', 'test' );
var_dump( beanstalk_watch( $b, 'test-tube-a' ));
var_dump( beanstalk_watch( $b, 'test-tube-a', true ));
var_dump( beanstalk_ignore( $b, 'test-tube-a' ));

var_dump( $b->watch( 'test-tube-a' ));
var_dump( $b->watch( 'test-tube-a', true ));
var_dump( $b->ignore( 'test-tube-a' ));

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
int(%d)
int(%d)
bool(true)
int(%d)
int(%d)