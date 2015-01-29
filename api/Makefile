BIN_CHECK_PRODUCT = tl_checkproduct
OBJ_CHECK_PRODUCT = tl_checkproduct.o

BIN_ROUTER_READY = tl_routerready
OBJ_ROUTER_READY = tl_routerready.o

BIN_REMOTE = tl_remote
OBJ_REMOTE = tl_remote.o

BIN_REMOTE_CHANGE = tl_remotechange
OBJ_REMOTE_CHANGE = tl_remotechange.o

BIN_REMOTE_INFO = tl_remoteinfo
OBJ_REMOTE_INFO = tl_remoteinfo.o

BIN_STATUS = tl_status
OBJ_STATUS = tl_status.o

BIN_UPDATE_FW = tl_updatefw
OBJ_UPDATE_FW = tl_updatefw.o

BIN_UPDATE_CONF = tl_updateconf
OBJ_UPDATE_CONF = tl_updateconf.o

CFLAGS = -Wall -I/usr/include/testlab -I/usr/include/mysql
LIB_UTILS = -ltl_utils
LIB_DATABASE = -ltl_database
LIB_CSPIPE = -ltl_cspipe

all: $(BIN_CHECK_PRODUCT) $(BIN_REMOTE) $(BIN_REMOTE_CHANGE) \
	 $(BIN_REMOTE_INFO) $(BIN_STATUS) $(BIN_UPDATE_FW) \
	 $(BIN_UPDATE_CONF) $(BIN_ROUTER_READY)

$(BIN_REMOTE) : $(OBJ_REMOTE)
	$(CC) -Wall -o $(BIN_REMOTE) $(OBJ_REMOTE) $(LIB_CSPIPE)

$(BIN_REMOTE_CHANGE) : $(OBJ_REMOTE_CHANGE)
	$(CC) -Wall -o $(BIN_REMOTE_CHANGE) $(OBJ_REMOTE_CHANGE) $(LIB_CSPIPE)

$(BIN_REMOTE_INFO) : $(OBJ_REMOTE_INFO)
	$(CC) -Wall -o $(BIN_REMOTE_INFO) $(OBJ_REMOTE_INFO) $(LIB_CSPIPE)

$(BIN_STATUS) : $(OBJ_STATUS)
	$(CC) -Wall -o $(BIN_STATUS) $(OBJ_STATUS) $(LIB_CSPIPE)

$(BIN_CHECK_PRODUCT) : $(OBJ_CHECK_PRODUCT)
	$(CC) -Wall -o $(BIN_CHECK_PRODUCT) $(OBJ_CHECK_PRODUCT) $(LIB_DATABASE)

$(BIN_ROUTER_READY) : $(OBJ_ROUTER_READY)
	$(CC) -Wall -o $(BIN_ROUTER_READY) $(OBJ_ROUTER_READY) $(LIB_DATABASE) $(LIB_CSPIPE) $(LIB_UTILS)

$(BIN_UPDATE_FW) : $(OBJ_UPDATE_FW)
	$(CC) -Wall -o $(BIN_UPDATE_FW) $(OBJ_UPDATE_FW) $(LIB_CSPIPE) $(LIB_UTILS)

$(BIN_UPDATE_CONF) : $(OBJ_UPDATE_CONF)
	$(CC) -Wall -o $(BIN_UPDATE_CONF) $(OBJ_UPDATE_CONF) $(LIB_CSPIPE) $(LIB_UTILS)

clean:
	rm -rf $(BIN_REMOTE) $(OBJ_REMOTE)
	rm -rf $(BIN_REMOTE_CHANGE) $(OBJ_REMOTE_CHANGE)
	rm -rf $(BIN_REMOTE_INFO) $(OBJ_REMOTE_INFO)
	rm -rf $(BIN_STATUS) $(OBJ_STATUS)
	rm -rf $(BIN_CHECK_PRODUCT) $(OBJ_CHECK_PRODUCT)
	rm -rf $(BIN_UPDATE_FW) $(OBJ_UPDATE_FW)
	rm -rf $(BIN_UPDATE_CONF) $(OBJ_UPDATE_CONF)
	rm -rf $(BIN_ROUTER_READY) $(OBJ_ROUTER_READY)

install: all
	sudo cp -u $(BIN_REMOTE) /usr/bin/$(BIN_REMOTE)
	sudo cp -u $(BIN_REMOTE_CHANGE) /usr/bin/$(BIN_REMOTE_CHANGE)
	sudo cp -u $(BIN_REMOTE_INFO) /usr/bin/$(BIN_REMOTE_INFO)
	sudo cp -u $(BIN_STATUS) /usr/bin/$(BIN_STATUS)
	sudo cp -u $(BIN_CHECK_PRODUCT) /usr/bin/$(BIN_CHECK_PRODUCT)
	sudo cp -u $(BIN_UPDATE_FW) /usr/bin/$(BIN_UPDATE_FW)
	sudo cp -u $(BIN_UPDATE_CONF) /usr/bin/$(BIN_UPDATE_CONF)
	sudo cp -u $(BIN_ROUTER_READY) /usr/bin/$(BIN_ROUTER_READY)

uinstall:
	sudo rm -rf /usr/bin/$(BIN_REMOTE)
	sudo rm -rf /usr/bin/$(BIN_REMOTE_CHANGE)
	sudo rm -rf /usr/bin/$(BIN_REMOTE_INFO)
	sudo rm -rf /usr/bin/$(BIN_STATUS)
	sudo rm -rf /usr/bin/$(BIN_CHECK_PRODUCT)
	sudo rm -rf /usr/bin/$(BIN_UPDATE_FW)
	sudo rm -rf /usr/bin/$(BIN_UPDATE_CONF)
	sudo rm -rf /usr/bin/$(BIN_ROUTER_READY)
