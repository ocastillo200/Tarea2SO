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

class Monitor
{
private:
    vector<int> queue;
    size_t size;
    size_t capacity;
    mutex mtx;
    condition_variable not_full;
    condition_variable not_empty;
    ofstream log_file;
    atomic<bool> production_done;

    void resizeQueue(size_t new_capacity)
    {
        vector<int> new_queue(new_capacity);
        for (size_t i = 0; i < size; i++)
        {
            new_queue[i] = queue[i];
        }
        queue = move(new_queue);
        capacity = new_capacity;

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

    void produce(int item)
    {
        unique_lock<mutex> lock(mtx);
        not_full.wait(lock, [this]()
                      { return size < capacity; });

        queue[size++] = item;
        log_file << "Produced: " << item << ", Queue size: " << size << "\n";

        if (size == capacity)
        {
            resizeQueue(capacity * 2);
        }

        not_empty.notify_one();
    }

    bool consume(int &item, int wait_time_seconds)
    {
        unique_lock<mutex> lock(mtx);
        if (!not_empty.wait_for(lock, chrono::seconds(wait_time_seconds), [this]()
                                { return size > 0 || production_done; }))
        {
            log_file << "Consumer timeout. Exiting.\n";
            return false;
        }

        if (size == 0 && production_done)
        {
            return false;
        }

        item = queue[--size];
        log_file << "Consumed: " << item << ", Queue size: " << size << "\n";

        if (size > 0 && size <= capacity / 4)
        {
            resizeQueue(capacity / 2);
        }

        not_full.notify_one();
        return true;
    }
    void endProduction()
    {
        unique_lock<mutex> lock(mtx);
        production_done = true;
        not_empty.notify_all();
    }
};

void producer(Monitor &monitor, int id, int item_count)
{
    for (int i = 0; i < item_count; i++)
    {
        monitor.produce(id * 100 + i);
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}

void consumer(Monitor &monitor, int id, int wait_time)
{
    while (true)
    {
        int item;
        if (!monitor.consume(item, wait_time))
        {
            break;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

int main(int argc, char *argv[])
{
    if (argc != 9)
    {
        cerr << "Usage: ./simulapc -p <producers> -c <consumers> -s <queue_size> -t <wait_time>\n";
        return 1;
    }

    int producers = stoi(argv[2]);
    int consumers = stoi(argv[4]);
    size_t queue_size = stoi(argv[6]);
    int wait_time = stoi(argv[8]);

    Monitor monitor(queue_size, "simulapc.log");

    vector<thread> producer_threads;
    vector<thread> consumer_threads;

    for (int i = 0; i < producers; i++)
    {
        producer_threads.emplace_back(producer, ref(monitor), i + 1, 10);
    }
    for (int i = 0; i < consumers; i++)
    {
        consumer_threads.emplace_back(consumer, ref(monitor), i + 1, wait_time);
    }
    for (auto &t : producer_threads)
        t.join();

    monitor.endProduction();

    for (auto &t : consumer_threads)
        t.join();

    cout << "Simulation finished. Check simulapc.log for details.\n";
    return 0;
}
