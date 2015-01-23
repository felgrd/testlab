LIB_DATABASE = libtl_database.so
OBJ_LIB_DATABASE = database.o
H_LIB_DATABASE = database.h

LIB_UTILS = libtl_utils.so
OBJ_LIB_UTILS = utils.o
H_LIB_UTILS = utils.h

LIB_CSPIPE = libtl_cspipe.so
OBJ_LIB_CSPIPE = cspipe.o
H_LIB_CSPIPE = cspipe.h

CFLAGS = -Wall -I/usr/include/mysql -fpic
LIBS_MYSQL = -L/usr/local/lib/mysql -lmysqlclient

all: $(LIB_DATABASE) $(LIB_UTILS) $(LIB_CSPIPE)

$(LIB_DATABASE): $(OBJ_LIB_DATABASE)
	$(CC) -shared -o $(LIB_DATABASE) $(OBJ_LIB_DATABASE) $(LIBS_MYSQL)

$(LIB_UTILS): $(OBJ_LIB_UTILS)
	$(CC) -shared -o $(LIB_UTILS) $(OBJ_LIB_UTILS)

$(LIB_CSPIPE): $(OBJ_LIB_CSPIPE)
	$(CC) -shared -o $(LIB_CSPIPE) $(OBJ_LIB_CSPIPE)

clean:
	rm -rf $(LIB_DATABASE) $(OBJ_LIB_DATABASE)
	rm -rf $(LIB_UTLIS) $(OBJ_LIB_UTILS)
	rm -rf $(LIB_CSPIPE) $(OBJ_LIB_CSPIPE)

install: all
	sudo cp -u $(LIB_UTILS) /usr/lib/$(LIB_UTILS)
	sudo cp -u $(LIB_DATABASE) /usr/lib/$(LIB_DATABASE)
	sudo cp -u $(LIB_CSPIPE) /usr/lib/$(LIB_CSPIPE)
	sudo ldconfig
	sudo mkdir -p /usr/include/testlab
	sudo cp -u $(H_LIB_UTILS) /usr/include/testlab/$(H_LIB_UTILS)
	sudo cp -u $(H_LIB_DATABASE) /usr/include/testlab/$(H_LIB_DATABASE)
	sudo cp -u $(H_LIB_CSPIPE) /usr/include/testlab/$(H_LIB_CSPIPE)

uinstall:
	sudo rm -rf /usr/lib/libtl_utils.so
	sudo rm -rf /usr/lib/libtl_database.so
	sudo rm -rf /usr/lib/libtl_cspipe.so
	sudo rm -rf /usr/include/testlab
