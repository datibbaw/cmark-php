--TEST--
Check cmark list creation
--FILE--
<?php
use CommonMark\Parser;
use CommonMark\Node;

$list = new Node(NODE::ORDERED);
$item = new Node(NODE::ITEM);
$text = new Node(NODE::HTML);

$text->setContent("item");
$item->appendChild($text);
$list->appendChild($item);

echo $list->getHTML();
?>
--EXPECT--
<ul>
<li>
item</li>
</ul>


