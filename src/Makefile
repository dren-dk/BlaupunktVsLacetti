
remote-test: remote.c remote-test.c remote.h blaupunkt-2011.h 
	gcc -std=c99 -Wall -g3 -pedantic-errors -Werror -o $@ remote.c remote-test.c

blaupunkt-2011.h: blaupunkt-2011.lirc
	./lirc2c $< $@
