# Nazwa kompilatora
CC = gcc

# Flagi kompilacji: 
# -Wall: wszystkie ostrzeżenia
# -Wextra: dodatkowe ostrzeżenia
# -g: informacje do debuggera (opcjonalnie)
CFLAGS = -Wall -Wextra -g -D_POSIX_C_SOURCE=199309L

# Pliki wykonywalne
TARGETS = main child

# Reguła domyślna (wywoływana przez samo wpisanie 'make')
all: $(TARGETS)

# Kompilacja programu głównego
main: main.c
	$(CC) $(CFLAGS) main.c -o main

# Kompilacja programu potomnego
child: child.c
	$(CC) $(CFLAGS) child.c -o child

# Czyszczenie plików binarnych i tymczasowych
clean:
	rm -f $(TARGETS) *.o

# Reguła pomocnicza do uruchamiania testów (przykład dla trybu mask)
test_mask: all
	./main mask

# Reguła pomocnicza do uruchamiania testów (przykład dla trybu handle)
test_handle: all
	./main handle

.PHONY: all clean test_mask test_handle