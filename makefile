#==============================================================
# UNIFAL - UNIVERSIDADE FEDERAL DE ALFENAS
# BACHARELADO EM CIÊNCIA DA COMPUTAÇÃO
# Trabalho: Decodificador do formato PGH
# Professor: Luiz Eduardo da Silva
# Aluno: [SEU NOME AQUI]
# Data: [DATA AQUI]
#==============================================================

# Compilador e flags
CC = gcc
CFLAGS = -Wall -O2

# Arquivos objeto e executável
OBJS = decode.o imagelib.o huf.o
TARGET = decode

# Regra principal
all: $(TARGET)

# Linkagem do executável
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compilar os arquivos individualmente
decode.o: decode.c imagelib.h
	$(CC) $(CFLAGS) -c decode.c

imagelib.o: imagelib.c imagelib.h
	$(CC) $(CFLAGS) -c imagelib.c

huf.o: huf.c
	$(CC) $(CFLAGS) -c huf.c

# Limpar tudo
clean:
	rm -f *.o $(TARGET)
