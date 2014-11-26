--TEST--
Check cmark basic functionality
--FILE--
<?php 
use CommonMark\Parser;

$parser = new Parser();

var_dump($parser);
?>
--EXPECTF--
object(CommonMark\Parser)#%d (%d) {
}

