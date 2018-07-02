CFLAGS=-Wall -Wextra -pedantic -O2
FICHEIROS=cgi.h estado.c estado.h DragonSlayer.c Makefile Images/*

install: DragonSlayer
	sudo cp -r Images /var/www/html
	sudo cp DragonSlayer /usr/lib/cgi-bin
	sudo touch /var/www/html/estado
	sudo chmod 666 /var/www/html/estado
	touch install

DragonSlayer: DragonSlayer.o estado.o
	cc -o DragonSlayer DragonSlayer.o estado.o

DragonSlayer.zip: $(FICHEIROS)
	zip -9 DragonSlayer.zip $(FICHEIROS)

doc:
	doxygen -g
	doxygen

clean:
	rm -rf *.o DragonSlayer DragonSlayer.zip Doxyfile Doxyfile.bak latex html install

DragonSlayer.o: DragonSlayer.c cgi.h estado.h

estado.o: estado.c estado.h
