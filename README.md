# Proyecto Sistemas Operativos: Mecanismos de Sincronización y Memoria Virtual
Este proyecto consiste en la implementación de dos simuladores, el primero es una cola circular de tamaño dinámico multi-hebras para atacar el problema productor-consumidor, y el segundo es un simulador de memoria virtual con paginación y reemplazo de páginas.

## Integrantes:
- Oscar Castillo
- Gaspar Jimenez
- Matias Gayoso
- Pedro Muñoz

## Instrucciones de uso:
1. Clonar repositorio
```bash
git clone https://github.com/ocastillo200/Tarea2SO.git
```
2. Actualizar paquetes
```bash
sudo apt update
sudo apt install build-essential
```
3. Compilar el código (verificar que se está situado en la carpeta del repositorio)
```bash
g++ main.cpp circularQ.cpp vmSimulator.cpp
```
4. Ejecutar el programa
```bash
./a.out
```
5. Opcional (en caso de errores de ejecución):
Si el programa no realiza de manera correcta los comandos de creación de archivos como favs --crear, quizás sea necesario ejecutarlo con permisos adicionales de la siguiente forma:
```bash
sudo ./a.out
```

