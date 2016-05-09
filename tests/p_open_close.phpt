--TEST--
Check for beanstalk presence
--SKIPIF--
<?php if (!extension_loaded("beanstalk")) print "skip"; ?>
--FILE--
<?php 
$b = beanstalk_open( "svn.huaer.dev" );
var_dump( $b );
var_dump( beanstalk_close( $b ));
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
resource(%d) of type (stream)
bool(true)