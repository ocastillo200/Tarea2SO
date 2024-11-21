#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <chrono>
#include <atomic>

using namespace std;

// Clase Monitor para manejar la cola circular y la sincronización
class Monitor
{
private:
    vector<int> queue;            // Cola dinámica
    size_t size;                  // Tamaño actual de la cola
    size_t capacity;              // Capacidad máxima de la cola
    mutex mtx;                    // Mutex para sincronización
    condition_variable not_full;  // Condición para cuando la cola no está llena
    condition_variable not_empty; // Condición para cuando la cola no está vacía
    ofstream log_file;            // Archivo para registrar eventos
    atomic<bool> production_done; // Indica si los productores han terminado

    // Función privada para redimensionar la cola
    void resizeQueue(size_t new_capacity)
    {
        vector<int> new_queue(new_capacity);
        for (size_t i = 0; i < size; i++)
        {
            new_queue[i] = queue[i];
        }
        queue = move(new_queue);
        capacity = new_capacity;

        // Registrar el cambio de tamaño en el log
        log_file << "Resized queue to: " << capacity << " elements.\n";
    }

public:
    Monitor(size_t initial_capacity, const string &log_file_name)
        : queue(initial_capacity), size(0), capacity(initial_capacity), production_done(false)
    {
        log_file.open(log_file_name, ios::out);
        if (!log_file)
        {
            cerr << "Error opening log file.\n";
            exit(1);
        }
    }

    ~Monitor()
    {
        log_file.close();
    }

    // Método para agregar elementos a la cola
    void produce(int item)
    {
        unique_lock<mutex> lock(mtx);
        // Esperar si la cola está llena
        not_full.wait(lock, [this]()
                      { return size < capacity; });

        // Agregar elemento a la cola
        queue[size++] = item;
        log_file << "Produced: " << item << ", Queue size: " << size << "\n";

        // Si la cola está llena, duplicar capacidad
        if (size == capacity)
        {
            resizeQueue(capacity * 2);
        }

        not_empty.notify_one(); // Notificar a los consumidores
    }

    // Método para consumir elementos de la cola
    // Método para consumir elementos de la cola
    bool consume(int &item, int wait_time_seconds)
    {
        unique_lock<mutex> lock(mtx);
        // Esperar con un tiempo límite si la cola está vacía y la producción no ha terminado
        if (!not_empty.wait_for(lock, chrono::seconds(wait_time_seconds), [this]()
                                { return size > 0 || production_done; }))
        {
            // Si el tiempo expiró y no hay producción pendiente, finalizar
            log_file << "Consumer timeout. Exiting.\n";
            return false;
        }

        if (size == 0 && production_done)
        {
            // Si la producción terminó y no hay elementos, finalizar
            return false;
        }

        // Extraer elemento de la cola
        item = queue[--size];
        log_file << "Consumed: " << item << ", Queue size: " << size << "\n";

        // Si el tamaño de uso cae al 25%, reducir capacidad
        if (size > 0 && size <= capacity / 4)
        {
            resizeQueue(capacity / 2);
        }

        not_full.notify_one(); // Notificar a los productores
        return true;
    }

    // Método para marcar el fin de la producción
    void endProduction()
    {
        unique_lock<mutex> lock(mtx);
        production_done = true;
        not_empty.notify_all(); // Despertar a todos los consumidores
    }
};

// Función de productor
void producer(Monitor &monitor, int id, int item_count)
{
    for (int i = 0; i < item_count; i++)
    {
        monitor.produce(id * 100 + i);                    // Generar un identificador único
        this_thread::sleep_for(chrono::milliseconds(50)); // Simular producción
    }
}

// Función de consumidor
void consumer(Monitor &monitor, int id, int wait_time)
{
    while (true)
    {
        int item;
        if (!monitor.consume(item, wait_time))
        {
            break; // Terminar si no hay más elementos y la producción ha finalizado
        }
        this_thread::sleep_for(chrono::milliseconds(100)); // Simular procesamiento
    }
}

int main(int argc, char *argv[])
{
    // Validar parámetros de entrada
    if (argc != 9)
    {
        cerr << "Usage: ./simulapc -p <producers> -c <consumers> -s <queue_size> -t <wait_time>\n";
        return 1;
    }

    // Leer argumentos
    int producers = stoi(argv[2]);
    int consumers = stoi(argv[4]);
    size_t queue_size = stoi(argv[6]);
    int wait_time = stoi(argv[8]);

    // Crear monitor con cola inicial y archivo de log
    Monitor monitor(queue_size, "simulapc.log");

    // Crear hebras de productores y consumidores
    vector<thread> producer_threads;
    vector<thread> consumer_threads;

    for (int i = 0; i < producers; i++)
    {
        producer_threads.emplace_back(producer, ref(monitor), i + 1, 10); // Cada productor produce 10 elementos
    }
    for (int i = 0; i < consumers; i++)
    {
        consumer_threads.emplace_back(consumer, ref(monitor), i + 1, wait_time);
    }

    // Esperar a que los productores terminen
    for (auto &t : producer_threads)
        t.join();

    // Indicar fin de la producción
    monitor.endProduction();

    // Esperar a que los consumidores terminen
    for (auto &t : consumer_threads)
        t.join();

    cout << "Simulation finished. Check simulapc.log for details.\n";
    return 0;
}
