#!/bin/bash

# Compilar sala.c y crear la biblioteca estática
gcc -c sala.c -o sala.o
ar rcs libsala.a sala.o

# Compilar minishell.c enlazándolo con la biblioteca estática
gcc minishell.c -o minishell -L. -lsala

# Hacer ejecutable el archivo de salida final
chmod +x minishell

# Compilar sucursal.c
gcc sucursal.c -o sucursal

# Ejecutar sucursal
./sucursal

