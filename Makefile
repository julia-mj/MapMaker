# Mój plik makefile - wpisz 'make all' aby skompilować cały program
# (właściwie wystarczy wpisać 'make' - all jest domyślny jako pierwszy cel)
compile = cc `pkg-config --cflags gtk+-3.0 gdal`
#obrazek.c -o obrazek `pkg-config --libs gtk+-3.0`

all: interfejs.o paleta.o obrazek.o
	$(compile) interfejs.o paleta.o obrazek.o -o caly_program `pkg-config --libs gtk+-3.0 gdal`

interfejs.o: interfejs.c paletalib.h mapa.h
	$(compile) interfejs.c -c -o interfejs.o #`pkg-config --libs gtk+-3.0`

paleta.o: paleta.c paletalib.h
	$(compile) paleta.c -c -o paleta.o #`pkg-config --libs gtk+-3.0`
	
obrazek.o: obrazek.c paletalib.h mapa.h
	$(compile) obrazek.c -c -o obrazek.o
