SERVER = testlab
USER = testlab
REMOTE_DIR = /opt/testlab/api

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

BIN_SLOG = tl_slog
OBJ_SLOG = tl_slog.o

BIN_KLOG = tl_klog
OBJ_KLOG = tl_klog.o

BIN_RLOG = tl_rlog
OBJ_RLOG = tl_rlog.o

BIN_CHANGE_PARAM = tl_paramchange
OBJ_CHANGE_PARAM = tl_paramchange.o

BIN_INFO_PARAM = tl_paraminfo
OBJ_INFO_PARAM = tl_paraminfo.o

BIN_MOBILE_READY = tl_mobileready
OBJ_MOBILE_READY = tl_mobileready.o

BIN_SIM = tl_sim
OBJ_SIM = tl_sim.o

BIN_PORT = tl_port
OBJ_PORT = tl_port.o

BIN_INTERFACE = tl_interface
OBJ_INTERFACE = tl_interface.o

CFLAGS = -Wall -I../lib -I/usr/include/mysql
LIB_UTILS = -ltl_utils
LIB_DATABASE = -ltl_database
LIB_CSPIPE = -ltl_cspipe

all: $(BIN_CHECK_PRODUCT) $(BIN_REMOTE) $(BIN_REMOTE_CHANGE) \
	$(BIN_REMOTE_INFO) $(BIN_STATUS) $(BIN_UPDATE_FW) \
	$(BIN_UPDATE_CONF) $(BIN_ROUTER_READY) $(BIN_SLOG) $(BIN_RLOG) $(BIN_KLOG) \
	$(BIN_CHANGE_PARAM) $(BIN_MOBILE_READY) $(BIN_INTERFACE) $(BIN_PORT) \
	$(BIN_SIM) $(BIN_INFO_PARAM)

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

$(BIN_SLOG) : $(OBJ_SLOG)
	$(CC) -Wall -o $(BIN_SLOG) $(OBJ_SLOG) $(LIB_CSPIPE)

$(BIN_KLOG) : $(OBJ_KLOG)
	$(CC) -Wall -o $(BIN_KLOG) $(OBJ_KLOG) $(LIB_CSPIPE)

$(BIN_RLOG) : $(OBJ_RLOG)
	$(CC) -Wall -o $(BIN_RLOG) $(OBJ_RLOG) $(LIB_CSPIPE)

$(BIN_CHANGE_PARAM) : $(OBJ_CHANGE_PARAM)
	$(CC) -Wall -o $(BIN_CHANGE_PARAM) $(OBJ_CHANGE_PARAM) $(LIB_CSPIPE)

$(BIN_INFO_PARAM) : $(OBJ_INFO_PARAM)
	$(CC) -Wall -o $(BIN_INFO_PARAM) $(OBJ_INFO_PARAM) $(LIB_CSPIPE)

$(BIN_MOBILE_READY) : $(OBJ_MOBILE_READY)
	$(CC) -Wall -o $(BIN_MOBILE_READY) $(OBJ_MOBILE_READY) $(LIB_CSPIPE)

$(BIN_SIM) : $(OBJ_SIM)
	$(CC) -Wall -o $(BIN_SIM) $(OBJ_SIM) $(LIB_DATABASE)

$(BIN_PORT) : $(OBJ_PORT)
	$(CC) -Wall -o $(BIN_PORT) $(OBJ_PORT) $(LIB_DATABASE)

$(BIN_INTERFACE) : $(OBJ_INTERFACE)
	$(CC) -Wall -o $(BIN_INTERFACE) $(OBJ_INTERFACE) $(LIB_DATABASE)

clean:
	rm -f $(BIN_REMOTE) $(OBJ_REMOTE)
	rm -f $(BIN_REMOTE_CHANGE) $(OBJ_REMOTE_CHANGE)
	rm -f $(BIN_REMOTE_INFO) $(OBJ_REMOTE_INFO)
	rm -f $(BIN_STATUS) $(OBJ_STATUS)
	rm -f $(BIN_CHECK_PRODUCT) $(OBJ_CHECK_PRODUCT)
	rm -f $(BIN_UPDATE_FW) $(OBJ_UPDATE_FW)
	rm -f $(BIN_UPDATE_CONF) $(OBJ_UPDATE_CONF)
	rm -f $(BIN_ROUTER_READY) $(OBJ_ROUTER_READY)
	rm -f $(BIN_SLOG) $(OBJ_SLOG)
	rm -f $(BIN_KLOG) $(OBJ_KLOG)
	rm -f $(BIN_RLOG) $(OBJ_RLOG)
	rm -f $(BIN_CHANGE_PARAM) $(OBJ_CHANGE_PARAM)
	rm -f $(BIN_INFO_PARAM) $(OBJ_INFO_PARAM)
	rm -f $(BIN_MOBILE_READY) $(OBJ_MOBILE_READY)
	rm -f $(BIN_SIM) $(OBJ_SIM)
	rm -f $(BIN_PORT) $(OBJ_PORT)
	rm -f $(BIN_INTERFACE) $(OBJ_INTERFACE)

install: all
	sudo cp -u $(BIN_REMOTE) /usr/bin/$(BIN_REMOTE)
	sudo cp -u $(BIN_REMOTE_CHANGE) /usr/bin/$(BIN_REMOTE_CHANGE)
	sudo cp -u $(BIN_REMOTE_INFO) /usr/bin/$(BIN_REMOTE_INFO)
	sudo cp -u $(BIN_STATUS) /usr/bin/$(BIN_STATUS)
	sudo cp -u $(BIN_CHECK_PRODUCT) /usr/bin/$(BIN_CHECK_PRODUCT)
	sudo cp -u $(BIN_UPDATE_FW) /usr/bin/$(BIN_UPDATE_FW)
	sudo cp -u $(BIN_UPDATE_CONF) /usr/bin/$(BIN_UPDATE_CONF)
	sudo cp -u $(BIN_ROUTER_READY) /usr/bin/$(BIN_ROUTER_READY)
	sudo cp -u $(BIN_SLOG) /usr/bin/$(BIN_SLOG)
	sudo cp -u $(BIN_KLOG) /usr/bin/$(BIN_KLOG)
	sudo cp -u $(BIN_RLOG) /usr/bin/$(BIN_RLOG)
	sudo cp -u $(BIN_CHANGE_PARAM) /usr/bin/$(BIN_CHANGE_PARAM)
	sudo cp -u $(BIN_INFO_PARAM) /usr/bin/$(BIN_INFO_PARAM)
	sudo cp -u $(BIN_MOBILE_READY) /usr/bin/$(BIN_MOBILE_READY)
	sudo cp -u $(BIN_SIM) /usr/bin/$(BIN_SIM)
	sudo cp -u $(BIN_PORT) /usr/bin/$(BIN_PORT)
	sudo cp -u $(BIN_INTERFACE) /usr/bin/$(BIN_INTERFACE)

uinstall:
	sudo rm -f /usr/bin/$(BIN_REMOTE)
	sudo rm -f /usr/bin/$(BIN_REMOTE_CHANGE)
	sudo rm -f /usr/bin/$(BIN_REMOTE_INFO)
	sudo rm -f /usr/bin/$(BIN_STATUS)
	sudo rm -f /usr/bin/$(BIN_CHECK_PRODUCT)
	sudo rm -f /usr/bin/$(BIN_UPDATE_FW)
	sudo rm -f /usr/bin/$(BIN_UPDATE_CONF)
	sudo rm -f /usr/bin/$(BIN_ROUTER_READY)
	sudo rm -f /usr/bin/$(BIN_SLOG)
	sudo rm -f /usr/bin/$(BIN_KLOG)
	sudo rm -f /usr/bin/$(BIN_RLOG)
	sudo rm -f /usr/bin/$(BIN_CHANGE_PARAM)
	sudo rm -f /usr/bin/$(BIN_INFO_PARAM)
	sudo rm -f /usr/bin/$(BIN_MOBILE_READY)
	sudo rm -f /usr/bin/$(BIN_SIM)
	sudo rm -f /usr/bin/$(BIN_PORT)
	sudo rm -f /usr/bin/$(BIN_INTERFACE)

remote-install: all
	scp $(BIN_REMOTE) $(SERVER)@$(USER):$(REMOTE_DIR)/$(BIN_REMOTE)
	scp $(BIN_REMOTE_CHANGE) $(SERVER)@$(USER):$(REMOTE_DIR)/$(BIN_REMOTE_CHANGE)
	scp $(BIN_REMOTE_INFO) $(SERVER)@$(USER):$(REMOTE_DIR)/$(BIN_REMOTE_INFO)
	scp $(BIN_STATUS) $(SERVER)@$(USER):$(REMOTE_DIR)/$(BIN_STATUS)
	scp $(BIN_CHECK_PRODUCT) $(SERVER)@$(USER):$(REMOTE_DIR)/$(BIN_CHECK_PRODUCT)
	scp $(BIN_UPDATE_FW) $(SERVER)@$(USER):$(REMOTE_DIR)/$(BIN_UPDATE_FW)
	scp $(BIN_UPDATE_CONF) $(SERVER)@$(USER):$(REMOTE_DIR)/$(BIN_UPDATE_CONF)
	scp $(BIN_ROUTER_READY) $(SERVER)@$(USER):$(REMOTE_DIR)/$(BIN_ROUTER_READY)
	scp $(BIN_SLOG) $(SERVER)@$(USER):$(REMOTE_DIR)/$(BIN_SLOG)
	scp $(BIN_KLOG) $(SERVER)@$(USER):$(REMOTE_DIR)/$(BIN_KLOG)
	scp $(BIN_RLOG) $(SERVER)@$(USER):$(REMOTE_DIR)/$(BIN_RLOG)
	scp $(BIN_CHANGE_PARAM) $(SERVER)@$(USER):$(REMOTE_DIR)/$(BIN_CHANGE_PARAM)
	scp $(BIN_INFO_PARAM) $(SERVER)@$(USER):$(REMOTE_DIR)/$(BIN_INFO_PARAM)
	scp $(BIN_MOBILE_READY) $(SERVER)@$(USER):$(REMOTE_DIR)/$(BIN_MOBILE_READY)
	scp $(BIN_SIM) $(SERVER)@$(USER):$(REMOTE_DIR)/$(BIN_SIM)
	scp $(BIN_PORT) $(SERVER)@$(USER):$(REMOTE_DIR)/$(BIN_PORT)
	scp $(BIN_INTERFACE) $(SERVER)@$(USER):$(REMOTE_DIR)/$(BIN_INTERFACE)
