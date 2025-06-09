#==============================================================
# UNIFAL - UNIVERSIDADE FEDERAL DE ALFENAS
# BACHARELADO EM CIÊNCIA DA COMPUTAÇÃO
# Trabalho: Decodificador do formato PGH
# Professor: Luiz Eduardo da Silva
# Aluno: Otávio Augusto Souza Martins
#==============================================================

# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Arquivos objeto e executável
OBJS = decode.o imagelib.o huf.o
TARGET = decode

# Regra principal
all: $(TARGET)

# Linkagem do executável
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lm

# Compilar os arquivos individualmente
decode.o: decode.c imagelib.h
	$(CC) $(CFLAGS) -c decode.c

imagelib.o: imagelib.c imagelib.h
	$(CC) $(CFLAGS) -c imagelib.c

huf.o: huf.c huf.h
	$(CC) $(CFLAGS) -c huf.c

# Limpar tudo
clean:
	rm -f *.o $(TARGET)

.PHONY: all clean
