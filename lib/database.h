#ifndef __DATABASE_H
#define __DATABASE_H

#include <mysql.h>

/**
 *  @file database.h
 */

/// Database server.
#define DB_SERVER				"localhost"
/// User name to database server.
#define DB_USER					"test"
/// Passwor to database server.
#define DB_PASSWORD				"test"
/// Name of database.
#define DB_DATABASE				"test"
/// Socket name for connect to database. NULL - Local database.
#define DB_SOCKET_NAME			NULL
/// Port number for connect to database. 0 - Local database.
#define DB_PORT_NUMBER			0
/// Connection flags for connect to database. 0 - Local database.
#define DB_CONNECTION_FLAGS		0

/// Table platform, column idplatforms.
#define DB_PLATFORMS_ID		0
/// Table platform, column name.
#define DB_PLATFORMS_NAME	1

/// Table products, column idproducts.
#define DB_PRODUCTS_ID			0
/// Table products, column name
#define DB_PRODUCTS_NAME		1

/// Table Routers, column idrouters
#define DB_ROUTERS_ID			0
/// Table Routers, column name
#define DB_ROUTERS_NAME			1
/// Table Routers, column port
#define DB_ROUTERS_PORT			2
/// Table Routers, column address
#define DB_ROUTERS_ADDRESS		3
/// Table Routers, column protocol
#define DB_ROUTERS_PROTOCOL		4

/// Table Functions, column idfunctions
#define DB_FUNCTIONS_ID			0
/// Table Functions, column name
#define DB_FUNCTIONS_NAME		1
/// Table Functions, column order
#define DB_FUNCTIONS_ORDER		2

/// Table procedures, column idprocedures
#define DB_PROCEDURES_ID		0
/// Table procedures, column name
#define DB_PROCEDURES_NAME		1
/// Table procedures, column unit
#define DB_PROCEDURES_UNIT		2

/**
 * Function database_connect connect to the database and return pointer
 * on MYSQL connection.
 *
 * @return Pointer on MYSQL connection.
 */
MYSQL* database_connect(void);

/**
 * Function database_disconnect disconnect from the database.
 *
 * @param *conn Pointer on MYSQL connection
 */
void database_disconnect(MYSQL *conn);

/**
 * Function database_insert insert items to the database.
 *
 * @param *buffer_querry Pointer to buffer with database query.
 *
 * @return long long int id of inserted item.
 */
long long int database_insert(const char *buffer_query);

/**
 * Function database_select selects and returns data from database.
 *
 * @param *buffer_querry Pointer to buffer with database query.
 *
 * @returns Two dimensional array of pointer to char. First dimension
 * is number of items and second dimension is column in database. Column
 * are defined in macros.
 */
char*** database_select(const char *buffer_query);

/**
 * Function database_ins_release insert new release to database.
 *
 * @param data Pointer to char with date of release.
 * @param type Pointer to char with type of release.
 *
 * @return long long int id of inserted release.
 */
long long int database_ins_release(char *date, char *type);

/**
 * Function database_ins_build_product insert new result of product
 * build to database.
 *
 * @param release Integer value of release id.
 * @param product Integer value of product id.
 * @param build Integer value with product build result.
 *
 * @return long long int id of inserted product build.
 */
long long int database_ins_build_product(int release, int product, int build);

/**
 * Function database_ins_build_platform insert new result of platform
 * build to database.
 *
 * @param release Integer value of release id.
 * @param platform Integer value of platform id.
 * @param build Integer value with product build result.
 *
 * @return long long int id of inserted platform build.
 */
long long int database_ins_build_platform(int release, int platform, int build);

/**
 * Function database_ins_checkout insert new result of checkout project
 * from repository to database.
 *
 * @param release Integer value of release id.
 * @param platform Integer value of platform id.
 * @param checkout Integer value with project checkout result.
 *
 * @return long long int id of inserted checkout project.
 */
long long int database_ins_checkout(int release, int platform, int checkout);

/**
 * Function database_ins_logger insert new message of testlab to database.
 *
 * @param release Integer value of release id.
 * @param event Pointer to new message.
 *
 * @return long long int id of inserted log message.
 */
long long int database_logger(long long int release, char *event);

/**
 * Function database_ins_log insert new message of test router to database.
 *
 * @param procedure Integer value of procedure id.
 * @param router Integer value of router id.
 * @param release Integer value of release id.
 * @param event Pointer to new message.
 *
 * @return long long int id of inserted log message.
 */
long long int database_ins_log(int procedure, int router, long long int release, char *event);

/**
 * Function database_ins_res_procedure insert result of tests procedure
 * to database.
 *
 * @param procedure Integer value of procedure id.
 * @param router Integer value of router id.
 * @param release Integer value of release id.
 * @param result Integer value with test procedure result.
 * @param value Pointer to char with test procedure value.
 *
 * @result long long int id of inserted test procedure result.
 */
long long int database_ins_res_procedure(int procedure, int router, long long int release, int result, char *value);

/**
 * Function database_ins_res_procedure insert result of tests procedure
 * to database.
 *
 * @param procedure Integer value of procedure id.
 * @param router Integer value of router id.
 * @param release Integer value of release id.
 * @param result Integer value with test procedure result.
 * @param value Pointer to char with test procedure value.
 *
 * @result long long int id of inserted test procedure result.
 */
long long int database_ins_res_function(int function, int router, long long int release, int result);

/**
 * Function database_ins_res_procedure insert result of tests router
 * to database.
 *
 * @param router Integer value of router id.
 * @param release Integer value of release id.
 * @param result Integer value with test procedure result.
 *
 * @result long long int id of inserted test router result.
 */
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
