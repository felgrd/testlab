SERVER = testlab
USER = testlab

BIN_TESTLAB = testlab
OBJ_TESTLAB = testlab.o

BIN_CHECKOUT = tl_checkout
OBJ_CHECKOUT = checkout.o

BIN_COMPILE = tl_compile
OBJ_COMPILE = compile.o

BIN_TEST = tl_test
OBJ_TEST = test.o

BIN_CLEAR = tl_clear
OBJ_CLEAR = clear.o

BIN_REMOTE_EXIT = tl_remote_exit
OBJ_REMOTE_EXIT = remote_exit.o

BIN_REMOTE_SERVER = tl_remote_server
OBJ_REMOTE_SERVER = telnet.o ssh.o remote_server.o

CFLAGS = -Wall -I/usr/include/testlab -I/usr/include/mysql
LIB_UTILS = -ltl_utils
LIB_DATABASE = -ltl_database
LIB_CSPIPE = -ltl_cspipe

all: $(BIN_TESTLAB) $(BIN_CHECKOUT) $(BIN_COMPILE) $(BIN_TEST) \
     $(BIN_CLEAR) $(BIN_REMOTE_EXIT) $(BIN_REMOTE_SERVER)

$(BIN_TESTLAB) : $(OBJ_TESTLAB)
	$(CC) -Wall -o $(BIN_TESTLAB) $(OBJ_TESTLAB) $(LIB_UTILS) $(LIB_DATABASE)

$(BIN_CHECKOUT) : $(OBJ_CHECKOUT)
	$(CC) -Wall -o $(BIN_CHECKOUT) $(OBJ_CHECKOUT) $(LIB_UTILS) $(LIB_DATABASE)

$(BIN_COMPILE) : $(OBJ_COMPILE)
	$(CC) -Wall -o $(BIN_COMPILE) $(OBJ_COMPILE) $(LIB_UTILS) $(LIB_DATABASE)

$(BIN_TEST) : $(OBJ_TEST)
	$(CC) -Wall -o $(BIN_TEST) $(OBJ_TEST) $(LIB_UTILS) $(LIB_DATABASE)

$(BIN_CLEAR) : $(OBJ_CLEAR)
	$(CC) -Wall -o $(BIN_CLEAR) $(OBJ_CLEAR) $(LIB_UTILS) $(LIB_DATABASE)

$(BIN_REMOTE_EXIT) : $(OBJ_REMOTE_EXIT)
	$(CC) -Wall -o $(BIN_REMOTE_EXIT) $(OBJ_REMOTE_EXIT) $(LIB_CSPIPE)

$(BIN_REMOTE_SERVER) : $(OBJ_REMOTE_SERVER)
	$(CC) -Wall -o $(BIN_REMOTE_SERVER) $(OBJ_REMOTE_SERVER) $(LIB_CSPIPE)

clean:
	rm -f $(BIN_TESTLAB) $(OBJ_TESTLAB)
	rm -f $(BIN_CHECKOUT) $(OBJ_CHECKOUT)
	rm -f $(BIN_COMPILE) $(OBJ_COMPILE)
	rm -f $(BIN_TEST) $(OBJ_TEST)
	rm -f $(BIN_CLEAR) $(OBJ_CLEAR)
	rm -f $(BIN_REMOTE_EXIT) $(OBJ_REMOTE_EXIT)
	rm -f $(BIN_REMOTE_SERVER) $(OBJ_REMOTE_SERVER)

install: all
	sudo cp -u $(BIN_TESTLAB) /usr/bin/$(BIN_TESTLAB)
	sudo cp -u $(BIN_CHECKOUT) /usr/bin/$(BIN_CHECKOUT)
	sudo cp -u $(BIN_COMPILE) /usr/bin/$(BIN_COMPILE)
	sudo cp -u $(BIN_TEST) /usr/bin/$(BIN_TEST)
	sudo cp -u $(BIN_CLEAR) /usr/bin/$(BIN_CLEAR)
	sudo cp -u $(BIN_REMOTE_EXIT) /usr/bin/$(BIN_REMOTE_EXIT)
	sudo cp -u $(BIN_REMOTE_SERVER) /usr/bin/$(BIN_REMOTE_SERVER)

uinstall:
	sudo rm -f /usr/bin/$(BIN_TESTLAB)
	sudo rm -f /usr/bin/$(BIN_CHECKOUT)
	sudo rm -f /usr/bin/$(BIN_COMPILE)
	sudo rm -f /usr/bin/$(BIN_TEST)
	sudo rm -f /usr/bin/$(BIN_CLEAR)
	sudo rm -f /usr/bin/$(BIN_REMOTE_EXIT)
	sudo rm -f /usr/bin/$(BIN_REMOTE_SERVER)

remote-install: all
	scp $(BIN_TESTLAB) $(SERVER)@$(USER):/opt/testlab/main/$(BIN_TESTLAB)
	scp $(BIN_CHECKOUT) $(SERVER)@$(USER):/opt/testlab/main/$(BIN_CHECKOUT)
	scp $(BIN_COMPILE) $(SERVER)@$(USER):/opt/testlab/main/$(BIN_COMPILE)
	scp $(BIN_TEST) $(SERVER)@$(USER):/opt/testlab/main/$(BIN_TEST)
	scp $(BIN_CLEAR) $(SERVER)@$(USER):/opt/testlab/main/$(BIN_CLEAR)
	scp $(BIN_REMOTE_EXIT) $(SERVER)@$(USER):/opt/testlab/main/$(BIN_REMOTE_EXIT)
	scp $(BIN_REMOTE_SERVER) $(SERVER)@$(USER):/opt/testlab/main/$(BIN_REMOTE_SERVER)
