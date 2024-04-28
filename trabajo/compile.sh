#!/bin/bash

# Compilar sala.c y crear la biblioteca estática
gcc -c ./src/sala.c -o ./outputs/sala.o
ar rcs libsala.a ./outputs/sala.o

# Compilar minishell.c enlazándolo con la biblioteca estática
gcc ./src/minishell.c -o ./outputs/minishell -L. -lsala

# Hacer ejecutable el archivo de salida final
chmod +x ./outputs/minishell

# Compilar sucursal.c
gcc ./src/sucursal.c -o ./outputs/sucursal

# Ejecutar sucursal
./outputs/sucursal

