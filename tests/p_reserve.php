<?php 
$arrConfig = include __DIR__ . '/../include/config.inc';
$b = beanstalk_open( $arrConfig['host'], $arrConfig['port'] );
$job = beanstalk_reserve( $b );
$strData = var_export( $job, true );
var_dump( preg_match( "/array/", $strData ));

var_dump( beanstalk_release( $b, $job['id'] ));

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
