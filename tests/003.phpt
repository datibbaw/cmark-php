--TEST--
Check cmark incremental parser
--FILE--
<?php
use CommonMark\Parser;

$file = fopen(sprintf
    ("%s/test.md", dirname(__FILE__)), "r");

$parser = new Parser();

while (($line = fgets($file))) {
    $parser->parse($line);
}

$md = $parser->end();

fclose($file);

echo $md->getHTML();
?>
--EXPECT--
<h1>h1</h1>
<h2>h2</h2>
<p>paragraph</p>
<ul>
<li>item 1</li>
<li>item 2</li>
</ul>


