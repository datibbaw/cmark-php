--TEST--
Check cmark document parser
--FILE--
<?php
use CommonMark\Parser;

$md = Parser::parseDocument(file_get_contents(sprintf
    ("%s/test.md", dirname(__FILE__))));

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


