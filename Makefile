all:
	make -C lib all || exit 1
	make -C main all || exit 1
	make -C api all || exit 1

clean:
	make -C lib clean
	make -C main clean
	make -C api clean

install: all
	make -C lib install
	make -C main install
	make -C api install

uinstall:
	make -C lib uinstall
	make -C main uinstall
	make -C api uinstall

remote-install: all
	make -C lib remote-install
	make -C main remote-install
	make -C api remote-install
