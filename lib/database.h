#ifndef __DATABASE_H
#define __DATABASE_H

#include <mysql.h>

/*
 *  Definice parametu pripojeni
 */
 
#define DB_SERVER				"localhost"
#define DB_USER					"test"
#define DB_PASSWORD				"test"
#define DB_DATABASE				"test"
#define DB_SOCKET_NAME			NULL
#define DB_PORT_NUMBER			0
#define DB_CONNECTION_FLAGS		0

/*
 *  Definice pristupu k sloupcum tabulek
 */

// Tabulka Platforms
#define DB_PLATFORMS_ID		0
#define DB_PLATFORMS_NAME	1

// Tabulka Products
#define DB_PRODUCTS_ID			0
#define DB_PRODUCTS_NAME		1
#define DB_PRODUCTS_APPNAME		2

// Tabulka Routers
#define DB_ROUTERS_ID			0
#define DB_ROUTERS_NAME			1
#define DB_ROUTERS_PORT			2
#define DB_ROUTERS_ADDRESS		3
#define DB_ROUTERS_PROTOCOL		4

// Tabulka Functions
#define DB_FUNCTIONS_ID			0
#define DB_FUNCTIONS_NAME		1
#define DB_FUNCTIONS_ORDER		2

// Tabulka Procedures
#define DB_PROCEDURES_ID		0
#define DB_PROCEDURES_NAME		1
#define DB_PROCEDURES_UNIT		2

MYSQL* database_connect(void);

void database_disconnect(MYSQL *conn);

long long int database_insert(const char *buffer_query);

char*** database_select(const char *buffer_query);

long long int database_ins_release(char *date, char *type);

long long int database_ins_build_product(int release, int product, int build);

long long int database_ins_build_platform(int release, int platform, int build);

long long int database_ins_checkout(int release, int platform, int checkout);

long long int database_logger(long long int release, char *event);

long long int database_ins_log(int procedure, int router, long long int release, char *event);

long long int database_ins_res_procedure(int procedure, int router, long long int release, int result, char *value);

long long int database_ins_res_function(int function, int router, long long int release, int result);

long long int database_ins_res_router(int router, long long int release, int result);

char*** database_sel_platforms(void);

char*** database_sel_products(int platform);

char*** database_sel_routers(void);

char*** database_sel_functions(int router);

char*** database_sel_procedures(int function);

char* database_sel_product(int router);

void database_result_free(char ***items);

int database_result_size(char ***items);


#endif
