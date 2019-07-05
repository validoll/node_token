Node Token
============

INTRODUCTION
------------

The Node Token module allow to use dedicated token types for
each node bundle like `[node-article:uid]`.

 * For a full description of the module, visit the project page:
   https://www.drupal.org/project/node_token

 * To submit bug reports and feature suggestions, or to track changes:
   https://www.drupal.org/project/issues/node_token
   
MOTIVATION
----------

### Token replacement

There is node types with fields:

* Article (`article`)
  * Body (`body`)
  * Foo (`field_foo`)
* Basic page (`page`)
  * Body (`body`)
  * Bar (`field_bar`)

For Drupal core node token you will use it as

```php
  $body = Drupal::token()->replace('[node:body]', ['node' => $node]);
```

But if you need to use 2 or more nodes with different node type in one token 
replacement you can yse this module.
The module provides dedicate token type for each node type 
like `node-{node_type}`

```php
  $body = Drupal::token()->replace(
      '[node-article:foo] [node-page:bar]', 
      [
        'node-article' => $article,
        'node-page' => $page,
      ]
  );
```

### Token tree

For Drupal core node token type in token tree you will see fields 
of all node types.
For example above you will see

* Node
  * Body
  * Foo
  * Bar

If you have many node types with many fields token tree will be very large.

`Node Token` display only fields of node type for token type

* Node Article
  * Body
  * Foo
* Node Basic page
  * Body
  * Bar


### Multiple instances of the same token type

This module doesn't provide a possibility to use multiple instances 
of the same token type in token replacement.
Pls see https://www.drupal.org/project/drupal/issues/1920688 
for more information.


REQUIREMENTS
------------

This module requires the following modules:

 * token (https://www.drupal.org/project/token)

INSTALLATION
------------
Install as you would normally install a contributed Drupal module. Visit:
https://www.drupal.org/documentation/install/modules-themes/modules-8
for further information.

CONFIGURATION
-------------

 * No configuration is needed.

MAINTAINERS
-----------

 * Vyacheslav Malchik (validoll) - https://www.drupal.org/u/validoll
