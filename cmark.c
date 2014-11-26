/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
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

/* If you declare any globals in php_cmark.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(cmark)
*/

/* True global resources - no need for thread safety here */
static int le_cmark;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("cmark.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_cmark_globals, cmark_globals)
    STD_PHP_INI_ENTRY("cmark.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_cmark_globals, cmark_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_cmark_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(cmark_markdown_to_html)
{
	char *arg = NULL, *result;
	int arg_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	result = cmark_markdown_to_html(arg, arg_len);

	if (result) {
		RETVAL_STRING(result, 1);
		free(result);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_cmark_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_cmark_init_globals(zend_cmark_globals *cmark_globals)
{
	cmark_globals->global_value = 0;
	cmark_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(cmark)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(cmark)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
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

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ cmark_functions[]
 *
 * Every user visible function must have an entry in cmark_functions[].
 */
const zend_function_entry cmark_functions[] = {
	PHP_FE(cmark_markdown_to_html,	NULL)
	PHP_FE_END	/* Must be the last line in cmark_functions[] */
};
/* }}} */

/* {{{ cmark_module_entry
 */
zend_module_entry cmark_module_entry = {
	STANDARD_MODULE_HEADER,
	"cmark",
	cmark_functions,
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
