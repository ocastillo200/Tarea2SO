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
2. Para compilar el simulador de memoria virtual:
```bash
g++ Simulador_de_Memoria_Virtual.cpp -o mvirtual
```
Y para ejecutar el programa:
```bash
./mvirtual -m <frames> -a <algorithm> -f <file>
```
3. Para compilar el simulador de Productor-Consumidor:
```bash
g++ Simulador_Productores_Consumidores.cpp -o simulapc 
```
Y para ejecutar el programa:
```bash
./simulapc -p <producers> -c <consumers> -s <queue_size> -t <wait_time>
```

