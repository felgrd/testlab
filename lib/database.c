#include <stdio.h>
#include <stdlib.h>    // atoi
#include <string.h>    // strdup
#include <syslog.h>    // syslog
#include "database.h"


MYSQL* database_connect(void){
	MYSQL	*conn;

	// Inicializace pripojeni
	conn = mysql_init(NULL);
	if(conn == NULL){
		syslog(LOG_NOTICE, "MySQL init failed");
		return NULL;
	}

	// Pripojeni k serveru
	if (mysql_real_connect(conn, DB_SERVER, DB_USER, DB_PASSWORD, \
	DB_DATABASE, 0, NULL, 0) == NULL) {

		// Report chyby
		syslog(LOG_NOTICE, "MySQL error %u (%s)", mysql_errno (conn), \
		mysql_error(conn));

		// Uzavreni spojeni
		mysql_close(conn);

		return NULL;
	}

	return conn;
}

void database_disconnect(MYSQL *conn){
	mysql_close(conn);
}

long long int database_insert(const char *buffer_query){
	MYSQL 			*conn;
	MYSQL_RES 		*res;
	long long int 	result;

	// Pripojeni do databaze
	conn = database_connect();
	if(conn == NULL){
		return 0;
	}

	// Odeslani SQL prikazu
	if(mysql_query(conn, buffer_query) != 0) {
		return 0;
	}

	// Prijem odpovedi
	res = mysql_use_result(conn);
	if(res == NULL){
		if(mysql_errno(conn) != 0) {
			// Chyba odpovedi
			mysql_close(conn);
			return 0;
		}else{
			// Vraceni id vlozeneho zaznamu
			result = mysql_insert_id(conn);
		}
	}

	// Ukonceni spojeni
	database_disconnect(conn);

	return result;
}

char*** database_select(const char *buffer_query){
	MYSQL 		*conn;
	MYSQL_RES 	*res;
	MYSQL_ROW	row;
	int			i, j;
	int 		num_rows;
	int			num_fields;
	char		***result;

	// Pripojeni do databaze
	conn = database_connect();
	if(conn == NULL){
		return NULL;
	}

	// Odeslani SQL prikazu
	if(mysql_query(conn, buffer_query) != 0) {
		fprintf(stderr, "MySQL result error: %s\n", mysql_error(conn));
		return NULL;
	}

	// Prijem odpovedi
	res = mysql_store_result(conn);

	if(res != NULL){

		// Pocet polozek dotazu
		num_rows = mysql_num_rows(res);
		num_fields = mysql_num_fields(res);

		// Alokace radku pro databazovy dotaz
		result = (char ***)malloc((num_rows + 1) * sizeof(char **));

		// Prochazeni radkama
		for(i = 0; (row = mysql_fetch_row(res)) != NULL; i++){

			// Alokace sloupcu pro aktualni radek
			result[i] = (char **)malloc((num_fields + 1) * sizeof(char *));

			// Prochazeni sloupcema
			for (j = 0; j < mysql_num_fields(res); j++){

				if(row[j] == NULL){
					// Kopirovani v pripade prazdneho pole v databazi
					result[i][j] = strdup("-");
				}else{
					// Kopirovani vysledku do pole
					result[i][j] = strdup(row[j]);
				}
			}
			// Ukonceni radku
			result[i][j] = NULL;
		}

		// Ukonceni pole vysledku
		result[i] = NULL;

		// Uvolneni vysledku SQL dotazu
		mysql_free_result(res);

	}else{
		result = NULL;
	}

	// Ukonceni spojeni
	database_disconnect(conn);

	return result;
}


long long int database_ins_release(char *date, char *type){
	int		result;
	char	buffer_query[512];

	// Sestaveni dotazu
	result = snprintf(buffer_query, sizeof(buffer_query), "INSERT INTO" \
	" fwreleases (date, type) VALUES ('%s', '%s')", date, type);

	// Kontrola sestaveneho dotazu
	if(result < 2){
		return 0;
	}

	// Provedeni SQL prikazu
	return database_insert(buffer_query);
}

long long int database_ins_build_product(int release, int product, int build){
	int		result;
	char	buffer_query[512];

	// Sestaveni dotazu
	result = snprintf(buffer_query, sizeof(buffer_query), "INSERT INTO" \
	" builds_product (idfwreleases, idproducts, state) VALUES ('%d',"\
	" '%d', '%d' )", release, product, build);

	// Kontrola sestaveneho dotazu
	if(result < 3){
		return 0;
	}

	// Provedeni SQL prikazu
	return database_insert(buffer_query);
}

long long int database_ins_build_platform(int release, int platform, int build){
	int		result;
	char	buffer_query[512];

	// Sestaveni dotazu
	result = snprintf(buffer_query, sizeof(buffer_query), "INSERT INTO" \
	" builds_platform (idfwreleases, idplatforms, state) VALUES ('%d',"\
	" '%d', '%d' )", release, platform, build);

	// Kontrola sestaveneho dotazu
	if(result < 3){
		return 0;
	}

	// Provedeni SQL prikazu
	return database_insert(buffer_query);
}

long long int database_ins_checkout(int release, int platform, int checkout){
	int		result;
	char	buffer_query[512];

	// Sestaveni dotazu
	result = snprintf(buffer_query, sizeof(buffer_query), "INSERT INTO" \
	" checkout (idfwreleases, idplatforms, state) VALUES ('%d', '%d'," \
	" '%d' )", release, platform, checkout);

	// Kontrola sestaveneho dotazu
	if(result < 3){
		return 0;
	}

	// Provedeni SQL prikazu
	return database_insert(buffer_query);
}

long long int database_logger(long long int release, char *event){
	int		result;
	char	buffer_query[512];

	// Sestavani dotazu
	result = snprintf(buffer_query, sizeof(buffer_query), "INSERT INTO" \
	" logs (idfwreleases, event) VALUES ('%lld', '%s')", release, \
	event);

	// Kontrola sestaveni dotazu
	if(result < 5){
		return 0;
	}

	// Provedeni SQL prikazu
	return database_insert(buffer_query);
}

long long int database_ins_log(int procedure, int router, long long int release, char *event){
	int		result;
	char	buffer_query[512];

	// Sestavani dotazu
	result = snprintf(buffer_query, sizeof(buffer_query), "INSERT INTO" \
	" logs (idprocedures, idrouters, idfwreleases, event) VALUES" \
	"('%d', '%d', '%lld', '%s')", procedure, router, release, event);

	// Kontrola sestaveni dotazu
	if(result < 5){
		return 0;
	}

	// Provedeni SQL prikazu
	return database_insert(buffer_query);
}

long long int database_ins_res_procedure(int procedure, int router, long long int release, int test, char *value){
	int		result;
	char	buffer_query[512];

	// Sestavani dotazu
	result = snprintf(buffer_query, sizeof(buffer_query), "INSERT INTO" \
	" tests_procedure (idprocedures, idrouters, idfwreleases, result, " \
	" value) VALUES ('%d', '%d', '%lld', '%d', '%s')", procedure, router, \
	release, test, value);

	// Kontrola sestaveni dotazu
	if(result < 6){
		return 0;
	}

	// Provedeni SQL prikazu
	return database_insert(buffer_query);
}

long long int database_ins_res_function(int function, int router, long long int release, int test){
	int		result;
	char	buffer_query[512];

	// Sestavani dotazu
	result = snprintf(buffer_query, sizeof(buffer_query), "INSERT INTO" \
	" tests_function (idfunctions, idrouters, idfwreleases, result) " \
	" VALUES ('%d', '%d', '%lld', '%d')", function, router, release, \
	test);

	// Kontrola sestaveni dotazu
	if(result < 5){
		return 0;
	}

	// Provedeni SQL prikazu
	return database_insert(buffer_query);
}

long long int database_ins_res_router(int router, long long int release, int test){
	int		result;
	char	buffer_query[512];

	// Sestavani dotazu
	result = snprintf(buffer_query, sizeof(buffer_query), "INSERT INTO" \
	" tests_router (idrouters, idfwreleases, result) VALUES"\
	" ('%d', '%lld', '%d')", router, release, test);

	// Kontrola sestaveni dotazu
	if(result < 4){
		return 0;
	}

	// Provedeni SQL prikazu
	return database_insert(buffer_query);
}

char*** database_sel_platforms(void){
	char	buffer_query[512];

	// Sestaveni dotazu
	strcpy(buffer_query, "SELECT idplatforms, name from platforms");

	// Provedeni SQL prikazu
	return database_select(buffer_query);
}

char*** database_sel_products(int platform){
	int 	result;
	char	buffer_query[512];

	// Kontrola parametri platform
	if(platform <= 0){
		return NULL;
	}

	// Sestaveni dotazu
	result = snprintf(buffer_query, sizeof(buffer_query), "SELECT " \
	"idproducts, name FROM products WHERE idplatforms=%d", platform);

	// Kontrola sestaveneho dotazu
	if(result < 2){
		fprintf(stderr, "MySQL: Bad query\n");
		return NULL;
	}

	return database_select(buffer_query);
}

char*** database_sel_routers(void){
	char	buffer_query[512];

	// Sestaveni dotazu
	strcpy(buffer_query, "SELECT idrouters, name, port, address," \
	" protocol from routers");

	// Provedeni SQL prikazu
	return database_select(buffer_query);
}

char*** database_sel_functions(int router){
	int		result;
	char	buffer_query[512];

	// Sestaveni dotazu
	result = snprintf(buffer_query, sizeof(buffer_query), "SELECT " \
	"functions.idfunctions, functions.name from functions join " \
	"routers_has_functions ON functions.idfunctions = " \
	"routers_has_functions.idfunctions WHERE " \
	"routers_has_functions.idrouters=%d ORDER BY functions.order", router);

	// Kontrola sestaveneho datazu
	if(result < 2){
		return NULL;
	}

	// Provedeni SQL prikazu
	return database_select(buffer_query);
}

char*** database_sel_procedures(int function){
	int		result;
	char	buffer_query[512];

	// Sestaveni dotazu
	result = snprintf(buffer_query, sizeof(buffer_query), "SELECT " \
	"idprocedures, name, unit FROM procedures WHERE idfunctions=%d", \
	function);

	// Kontrola sestaveneho dotazu
	if(result < 2){
		return NULL;
	}

	// Provedeni SQL scriptu
	return database_select(buffer_query);
}

char* database_sel_product(int router){
	int		result;
	char	***products;
	char	buffer_query[512];

	// Sestaveni SQL dotazu
	result = snprintf(buffer_query, sizeof(buffer_query), \
	"SELECT products.name FROM products LEFT JOIN \
	routers ON routers.idproducts = products.idproducts WHERE \
	routers.idrouters = '%d'", router);

	// Kontrola sestaveneho dotazu
	if(result < 3){
		return NULL;
	}

	// Provedeni SQL dotazu
	products = database_select(buffer_query);

	// Kontrola vysledku dotazu
	if(products == NULL || products[0] == NULL){
		return NULL;
	}

	// Vraceni vysledku
	return products[0][0];
}

char** database_sel_sim(int router, int position){
	int   result;
	char  ***sims;
	char  buffer_query[512];

	// Sestaveni SQL dotazu
	result = snprintf(buffer_query, sizeof(buffer_query), "SELECT idsims," \
	" ip, number, operator FROM sims WHERE sims.idrouters = '%d'" \
	" AND sims.position = '%d'", router, position);

	// Kontrola sestaveneho dotazu
	if(result < 3){
		return NULL;
	}

	// Provedeni SQL dotazu
	sims = database_select(buffer_query);

	// Kontrola vysledku
	if(sims == NULL){
		return NULL;
	}

	// Vraceni vysledku
	return sims[0];
}

char** database_sel_interface(int router, int position){
	int    result;
	char   ***interfaces;
	char   buffer_query[512];

	// Sestaveni SQL dotazu
	result = snprintf(buffer_query, sizeof(buffer_query), "SELECT idinterfaces,"\
	" type, ip, netmask, port FROM interfaces WHERE idrouters = '%d'"\
	" AND type = '%d'", router, position);

	// Kontrola sestaveneho dotazu
	if(result < 3){
		return NULL;
	}

	// Provedeni SQL dotazu
	interfaces = database_select(buffer_query);

	// Kontrola vysledku
	if(interfaces == NULL){
		return NULL;
	}

	// Vraceni vysledku
	return interfaces[0];
}

int database_sel_timeout(int state){
	int   result;
	char  ***timeout;
	char  buffer_query[512];

	// Sestaveni SQL
	result = snprintf(buffer_query, sizeof(buffer_query), "SELECT timeout FROM \
	states WHERE idstates = '%d'", state);

	// Kontrola sestaveni dotazu
	if(result < 3){
		return 0;
	}

	// Provedeni SQL dotazu
	timeout = database_select(buffer_query);

	// Kontrola vysledku dotazu
	if(timeout == NULL || timeout[0] == NULL){
		return 0;
	}

	// Vraceni timeoutu prevedeneho na cislo
	return atoi(timeout[0][0]);
}

void database_results_free(char ***items){
	int	i, j;

	if(items != NULL){
		for(i = 0; items[i] != NULL; i++){
			for(j = 0; items[i][j] != NULL; j++){
				free(items[i][j]);
			}
			free(items[i]);
		}
		free(items);
	}
}

void database_result_free(char **items){
	int	i;

	if(items != NULL){
		for(i = 0; items[i] != NULL; i++){
			free(items[i]);
		}
		free(items);
	}
}

int database_result_size(char ***items){
	int i;

	// Spocitani velikosti pole
	for(i = 0; items[i] != NULL; i++);

	return i;
}
