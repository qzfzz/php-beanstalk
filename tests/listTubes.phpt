--TEST--
Check for beanstalk listTubes listTubesWatched listTubeUsed
--SKIPIF--
<?php if (!extension_loaded("beanstalk")) print "skip"; ?>
--FILE--
<?php 
$arrConfig = include __DIR__ . '/../include/config.inc';
$b = beanstalk_connect( $arrConfig['host'], $arrConfig['port'] );

$jobID = beanstalk_putInTube( $b, 'tubea', 'message1' );
$strRet = var_export( beanstalk_listTubes( $b ), true );
var_dump( preg_match( "/array/", $strRet ));
$strRet = var_export( beanstalk_listTubesWatched( $b ), true );
var_dump( preg_match( "/array/", $strRet ));
var_dump(beanstalk_listTubeUsed( $b ));

$strRet = var_export( $b->listTubes(), true );
var_dump( preg_match( "/array/", $strRet ));
$strRet = var_export( $b->listTubesWatched(), true );
var_dump( preg_match( "/array/", $strRet ));
var_dump($b->listTubeUsed());

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
string(%d) "%s"
int(1)
int(1)
string(%d) "%s"