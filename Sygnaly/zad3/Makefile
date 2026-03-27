CC = gcc
CFLAGS = -Wall -Wextra -g -D_POSIX_C_SOURCE=199309L
LIB_NAME = signals
SRCS = sig_default.c sig_mask.c sig_ignore.c sig_handle.c
OBJS = $(SRCS:.c=.o)

STATIC_LIB = lib$(LIB_NAME).a
SHARED_LIB = lib$(LIB_NAME).so

.PHONY: all clean static shared dynamic

# Dodajemy 'main' do listy celów
all: static shared dynamic main child

# Tworzymy 'main' jako kopię lub link do wybranej wersji (np. shared)
main: shared
	cp main_shared main

# Zmień tę linię w Makefile:
child: child.c $(STATIC_LIB)
	$(CC) $(CFLAGS) child.c ./$(STATIC_LIB) -o child

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

# --- BIBLIOTEKA STATYCZNA ---
static: $(OBJS)
	ar rcs $(STATIC_LIB) $(OBJS)
	$(CC) $(CFLAGS) main.c -L. -l$(LIB_NAME) -o main_static

# --- BIBLIOTEKA WSPÓŁDZIELONA ---
shared: $(OBJS)
	$(CC) -shared $(OBJS) -o $(SHARED_LIB)
	$(CC) $(CFLAGS) main.c -L. -l$(LIB_NAME) -Wl,-rpath,. -o main_shared

# --- ŁADOWANIE DYNAMICZNE ---
dynamic: $(OBJS)
	$(CC) -shared $(OBJS) -o $(SHARED_LIB)
	$(CC) $(CFLAGS) -D DYNAMIC_LOAD main.c -o main_dynamic -ldl

clean:
	rm -f *.o *.a *.so main_static main_shared main_dynamic main child