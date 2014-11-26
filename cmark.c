/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_cmark.h"
#include <cmark.h>

zend_class_entry *ce_Parser;
zend_class_entry *ce_Node;

typedef struct _php_cmark_parser_t {
    zend_object       std;
    cmark_doc_parser *p;
} php_cmark_parser_t;

#define php_cmark_parser_new()    ((php_cmark_parser_t*) ecalloc(1, sizeof(php_cmark_parser_t)))
#define php_cmark_parser_fetch(t) ((php_cmark_parser_t*) zend_object_store_get_object(t TSRMLS_CC))

typedef struct _php_cmark_node_t {
    zend_object       std;
    cmark_node       *n;
} php_cmark_node_t;

#define php_cmark_node_new(t)   ((php_cmark_node_t*) ecalloc(1, sizeof(php_cmark_node_t)))
#define php_cmark_node_fetch(t) ((php_cmark_node_t*) zend_object_store_get_object(t TSRMLS_CC))

static inline void php_cmark_parser_free(void *object TSRMLS_DC) {
    php_cmark_parser_t *parser = (php_cmark_parser_t*) object;
    
    zend_object_std_dtor(&parser->std TSRMLS_CC);
    
    cmark_free_doc_parser(parser->p);
    
    efree(parser);
}

static inline zend_object_value php_cmark_parser_create(zend_class_entry *ce TSRMLS_DC) {
    zend_object_value value;
    php_cmark_parser_t *parser = php_cmark_parser_new();
    
    zend_object_std_init(&parser->std, ce TSRMLS_CC);
    object_properties_init(&parser->std, ce);
    
    parser->p = cmark_new_doc_parser();
    
    value.handle = zend_objects_store_put(
        parser, 
        (zend_objects_store_dtor_t) zend_objects_destroy_object, 
        php_cmark_parser_free, 
        NULL TSRMLS_CC);
    value.handlers = zend_get_std_object_handlers();
    
    return value;
}

static inline void php_cmark_node_free(void *object TSRMLS_DC) {
    php_cmark_node_t *node = (php_cmark_node_t*) object;
    
    zend_object_std_dtor(&node->std TSRMLS_CC);
    
    //cmark_node_destroy(node->n);
    
    efree(node);
}

static inline zend_object_value php_cmark_node_create(zend_class_entry *ce TSRMLS_DC) {
    zend_object_value value;
    php_cmark_node_t *node = php_cmark_node_new();

    zend_object_std_init(&node->std, ce TSRMLS_CC);
    object_properties_init(&node->std, ce);
    
    value.handle = zend_objects_store_put(
        node, 
        (zend_objects_store_dtor_t) zend_objects_destroy_object, 
        php_cmark_node_free, 
        NULL TSRMLS_CC);
    value.handlers = zend_get_std_object_handlers();
    
    return value;
}

PHP_METHOD(Parser, parseDocument) {
    zval *zinput;
    php_cmark_node_t   *pnode;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zinput) != SUCCESS || !zinput) {
        return;
    }
    
    switch (Z_TYPE_P(zinput)) {
        case IS_STRING:
            object_init_ex(return_value, ce_Node);
            pnode = php_cmark_node_fetch(return_value);
            pnode->n = cmark_parse_document
                (Z_STRVAL_P(zinput), Z_STRLEN_P(zinput));
        break;
    }
}

PHP_METHOD(Parser, parse) {
    zval *zinput;
    php_cmark_parser_t   *parser = php_cmark_parser_fetch(getThis());
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zinput) != SUCCESS || !zinput) {
        return;
    }
    
    switch (Z_TYPE_P(zinput)) {
        case IS_STRING:
            cmark_process_line(parser->p, Z_STRVAL_P(zinput), Z_STRLEN_P(zinput));
        break;
        
        case IS_RESOURCE:
            /* cast to FILE* and read from that */
        break;
    }
}

PHP_METHOD(Parser, end) {
    zval *zinput;
    php_cmark_parser_t   *parser = php_cmark_parser_fetch(getThis());
    php_cmark_node_t     *node;
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    object_init_ex(return_value, ce_Node);
    node = 
        php_cmark_node_fetch(return_value);
    node->n = cmark_finish(parser->p);
}

ZEND_BEGIN_ARG_INFO_EX(php_cmark_no_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_cmark_one_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, source)
ZEND_END_ARG_INFO()

zend_function_entry php_cmark_parser_methods[] = {
    PHP_ME(Parser, parse,         php_cmark_one_arginfo,    ZEND_ACC_PUBLIC)
    PHP_ME(Parser, end,           php_cmark_no_arginfo,     ZEND_ACC_PUBLIC)
    PHP_ME(Parser, parseDocument, php_cmark_one_arginfo,    ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    PHP_FE_END
};

PHP_METHOD(Node, getHTML) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    char *html = 
        cmark_render_html(node->n);
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    RETVAL_STRING(html, 1);
    free(html);
}

PHP_METHOD(Node, getType) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    RETURN_LONG(cmark_node_get_type(node->n));
}

PHP_METHOD(Node, getContent) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    const char *content = 
        cmark_node_get_string_content(node->n);
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    if (!content) {
        return;
    }
    
    RETURN_STRING((char*)content, 1);
}

PHP_METHOD(Node, setContent) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    zval *zinput;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zinput) != SUCCESS || !zinput) {
        return;
    }
    
    if (Z_TYPE_P(zinput) != IS_STRING) {
        return;
    }
    
    RETURN_BOOL(cmark_node_set_string_content(node->n, Z_STRVAL_P(zinput)));
}

PHP_METHOD(Node, getHeaderLevel) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    RETURN_LONG(cmark_node_get_header_level(node->n));
}

PHP_METHOD(Node, setHeaderLevel) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    long level;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &level) != SUCCESS) {
        return;
    }
    
    RETURN_LONG(cmark_node_set_header_level(node->n, level));
}

PHP_METHOD(Node, getListType) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    RETURN_LONG(cmark_node_get_list_type(node->n));
}

PHP_METHOD(Node, setListType) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    long type;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &type) != SUCCESS) {
        return;
    }
    
    RETURN_LONG(cmark_node_set_list_type(node->n, type));
}

PHP_METHOD(Node, getListStart) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    RETURN_LONG(cmark_node_get_list_start(node->n));
}

PHP_METHOD(Node, setListStart) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    long start;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &start) != SUCCESS) {
        return;
    }
    
    RETURN_LONG(cmark_node_set_list_start(node->n, start));
}

PHP_METHOD(Node, getListTight) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    RETURN_LONG(cmark_node_get_list_tight(node->n));
}

PHP_METHOD(Node, setListTight) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    long tight;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &tight) != SUCCESS) {
        return;
    }
    
    RETURN_LONG(cmark_node_set_list_tight(node->n, tight));
}

PHP_METHOD(Node, getFenceInfo) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    const char *info = cmark_node_get_fence_info(node->n);
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    if (!info) {
        return;
    }
    
    RETURN_STRING((char*)info, 1);
}

PHP_METHOD(Node, setFenceInfo) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    char *info = NULL;
    int   len;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &info, &len) != SUCCESS) {
        return;
    }
    
    if (!info) {
        return;
    }
    
    RETURN_LONG(cmark_node_set_fence_info(node->n, info));
}

PHP_METHOD(Node, getURL) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    const char *url = cmark_node_get_url(node->n);
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    if (!url) {
        return;
    }
    
    RETURN_STRING((char*)url, 1);
}

PHP_METHOD(Node, setURL) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    char *url = NULL;
    int   len;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &url, &len) != SUCCESS) {
        return;
    }
    
    if (!url) {
        return;
    }
    
    RETURN_LONG(cmark_node_set_url(node->n, url));
}

PHP_METHOD(Node, getTitle) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    const char *title = cmark_node_get_title(node->n);
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    if (!title) {
        return;
    }
    
    RETURN_STRING((char*)title, 1);
}

PHP_METHOD(Node, setTitle) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    char *title = NULL;
    int   len;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &title, &len) != SUCCESS) {
        return;
    }
    
    if (!title) {
        return;
    }
    
    RETURN_LONG(cmark_node_set_title(node->n, title));
}

PHP_METHOD(Node, getStartLine) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    
    if (zend_parse_paramters_none() != SUCCESS) {
        return;
    }
    
    RETURN_LONG(cmark_node_get_start_line(node->n));
}

PHP_METHOD(Node, getStartColumn) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    
    if (zend_parse_paramters_none() != SUCCESS) {
        return;
    }
    
    RETURN_LONG(cmark_node_get_start_column(node->n));
}

PHP_METHOD(Node, getEndLine) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    
    if (zend_parse_paramters_none() != SUCCESS) {
        return;
    }
    
    RETURN_LONG(cmark_node_get_end_line(node->n));
}

PHP_METHOD(Node, getNext) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    php_cmark_node_t *next;
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    object_init_ex(return_value, ce_Node);
    next = php_cmark_node_fetch(return_value);
    next->n = cmark_node_next(node->n);
}

PHP_METHOD(Node, getPrevious) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    php_cmark_node_t *previous;
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    object_init_ex(return_value, ce_Node);
    previous = php_cmark_node_fetch(return_value);
    previous->n = cmark_node_previous(node->n);
}

PHP_METHOD(Node, getParent) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    php_cmark_node_t *parent;
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    object_init_ex(return_value, ce_Node);
    parent = php_cmark_node_fetch(return_value);
    parent->n = cmark_node_parent(node->n);
}

PHP_METHOD(Node, getFirstChild) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    php_cmark_node_t *child;
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    object_init_ex(return_value, ce_Node);
    child = php_cmark_node_fetch(return_value);
    child->n = cmark_node_first_child(node->n);
}

PHP_METHOD(Node, getLastChild) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    php_cmark_node_t *child;
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    object_init_ex(return_value, ce_Node);
    child = php_cmark_node_fetch(return_value);
    child->n = cmark_node_last_child(node->n);
}

PHP_METHOD(Node, unlink) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    cmark_node_unlink(node->n);
}

PHP_METHOD(Node, insertBefore) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    zval *sibling;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &sibling, ce_Node) != SUCCESS) {
        return;
    }
    
    RETURN_LONG(cmark_node_insert_before(node->n, php_cmark_node_fetch(sibling)->n));
}

PHP_METHOD(Node, insertAfter) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    zval *sibling;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &sibling, ce_Node) != SUCCESS) {
        return;
    }
    
    RETURN_LONG(cmark_node_insert_after(node->n, php_cmark_node_fetch(sibling)->n));
}

PHP_METHOD(Node, prependChild) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    zval *child;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &child, ce_Node) != SUCCESS) {
        return;
    }
    
    RETURN_LONG(cmark_node_prepend_child(node->n, php_cmark_node_fetch(child)->n));
}

PHP_METHOD(Node, appendChild) {
    php_cmark_node_t *node = php_cmark_node_fetch(getThis());
    zval *child;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &child, ce_Node) != SUCCESS) {
        return;
    }
    
    RETURN_LONG(cmark_node_append_child(node->n, php_cmark_node_fetch(child)->n));
}

ZEND_BEGIN_ARG_INFO_EX(php_cmark_node_setHeaderLevel_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, level)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_cmark_node_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, node)
ZEND_END_ARG_INFO()

zend_function_entry php_cmark_node_methods[] = {
    PHP_ME(Node, getHTML,        php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, getType,        php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, getContent,     php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, setContent,     php_cmark_one_arginfo,                 ZEND_ACC_PUBLIC)
    PHP_ME(Node, getHeaderLevel, php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, setHeaderLevel, php_cmark_one_arginfo,                 ZEND_ACC_PUBLIC)
    PHP_ME(Node, setListType,    php_cmark_one_arginfo,                 ZEND_ACC_PUBLIC)
    PHP_ME(Node, getListType,    php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, getListStart,   php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, setListStart,   php_cmark_one_arginfo,                 ZEND_ACC_PUBLIC)
    PHP_ME(Node, getListTight,   php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, setListTight,   php_cmark_one_arginfo,                 ZEND_ACC_PUBLIC)
    PHP_ME(Node, getFenceInfo,   php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, setFenceInfo,   php_cmark_one_arginfo,                 ZEND_ACC_PUBLIC)
    PHP_ME(Node, getURL,         php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, setURL,         php_cmark_one_arginfo,                 ZEND_ACC_PUBLIC)
    PHP_ME(Node, getTitle,       php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, setTitle,       php_cmark_one_arginfo,                 ZEND_ACC_PUBLIC)
    PHP_ME(Node, getStartLine,   php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, getStartColumn, php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, getEndLine,     php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, getNext,        php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, getPrevious,    php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, getParent,      php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, getFirstChild,  php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, getLastChild,   php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, unlink,         php_cmark_no_arginfo,                  ZEND_ACC_PUBLIC)
    PHP_ME(Node, insertBefore,   php_cmark_node_arginfo,                ZEND_ACC_PUBLIC)
    PHP_ME(Node, insertAfter,    php_cmark_node_arginfo,                ZEND_ACC_PUBLIC)
    PHP_ME(Node, prependChild,   php_cmark_node_arginfo,                ZEND_ACC_PUBLIC)
    PHP_ME(Node, appendChild,    php_cmark_node_arginfo,                ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(cmark)
{
    zend_class_entry ce;
    
    INIT_NS_CLASS_ENTRY(ce, 
        "CommonMark", "Parser", php_cmark_parser_methods);
    ce_Parser = zend_register_internal_class(&ce TSRMLS_CC);
    ce_Parser->create_object = php_cmark_parser_create;

    INIT_NS_CLASS_ENTRY(ce,
        "CommonMark", "Node", php_cmark_node_methods);
    ce_Node   = zend_register_internal_class(&ce TSRMLS_CC);
    ce_Node->create_object = php_cmark_node_create;

#define REGISTER_NODE_CONSTANT(n, v) zend_declare_class_constant_long(ce_Node, ZEND_STRL(#n), v TSRMLS_CC)
    REGISTER_NODE_CONSTANT(QUOTE,     CMARK_NODE_BLOCK_QUOTE);
    REGISTER_NODE_CONSTANT(LIST,      CMARK_NODE_LIST);
    REGISTER_NODE_CONSTANT(ITEM,      CMARK_NODE_LIST_ITEM);
    REGISTER_NODE_CONSTANT(CODE,      CMARK_NODE_CODE_BLOCK);
    REGISTER_NODE_CONSTANT(HTML,      CMARK_NODE_HTML);
    REGISTER_NODE_CONSTANT(PARA,      CMARK_NODE_PARAGRAPH);
    REGISTER_NODE_CONSTANT(HEADER,    CMARK_NODE_HEADER);
    REGISTER_NODE_CONSTANT(RULE,      CMARK_NODE_HRULE);
    REGISTER_NODE_CONSTANT(REF,       CMARK_NODE_REFERENCE_DEF);
    REGISTER_NODE_CONSTANT(FIRST,     CMARK_NODE_FIRST_BLOCK);
    REGISTER_NODE_CONSTANT(LAST,      CMARK_NODE_LAST_BLOCK);
    REGISTER_NODE_CONSTANT(TEXT,      CMARK_NODE_TEXT);
    REGISTER_NODE_CONSTANT(SBR,       CMARK_NODE_SOFTBREAK);
    REGISTER_NODE_CONSTANT(LBR,       CMARK_NODE_LINEBREAK);
    REGISTER_NODE_CONSTANT(EMPH,      CMARK_NODE_EMPH);
    REGISTER_NODE_CONSTANT(STRONG,    CMARK_NODE_STRONG);
    REGISTER_NODE_CONSTANT(LINK,      CMARK_NODE_LINK);
    REGISTER_NODE_CONSTANT(IMG,       CMARK_NODE_IMAGE);
    REGISTER_NODE_CONSTANT(FIRSTI,    CMARK_NODE_FIRST_INLINE);
    REGISTER_NODE_CONSTANT(LASTI,     CMARK_NODE_LAST_INLINE);
    REGISTER_NODE_CONSTANT(NONE,      CMARK_NO_LIST);
    REGISTER_NODE_CONSTANT(UNORDERED, CMARK_BULLET_LIST);
    REGISTER_NODE_CONSTANT(ORDERED,   CMARK_ORDERED_LIST);
#undef REGISTER_NODE_CONSTANT

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(cmark)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(cmark)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(cmark)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(cmark)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "cmark support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ cmark_module_entry
 */
zend_module_entry cmark_module_entry = {
	STANDARD_MODULE_HEADER,
	"cmark",
	NULL,
	PHP_MINIT(cmark),
	PHP_MSHUTDOWN(cmark),
	PHP_RINIT(cmark),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(cmark),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(cmark),
	PHP_CMARK_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CMARK
ZEND_GET_MODULE(cmark)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
