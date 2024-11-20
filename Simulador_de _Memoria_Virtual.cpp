#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <list>
#include <queue>
#include <algorithm>
#include <string>
#include <sstream>
#include <climits> // Para INT_MAX

using namespace std;

// Clase para la simulación de memoria virtual
class VirtualMemorySimulator {
private:
    int frames;
    string replacement_algo;
    vector<int> page_references;
    unordered_map<int, int> page_table; // Cambiado para almacenar índices directamente
    list<int> lru_cache;
    queue<int> fifo_queue;
    vector<pair<int, bool>> clock_vector;
    size_t clock_hand; // Puntero al índice del vector
    int page_faults;

    void loadReferences(const string& file_name) {
        ifstream file(file_name);
        if (!file) {
            cerr << "Error: Unable to open file " << file_name << endl;
            exit(1);
        }
        int page;
        while (file >> page) {
            page_references.push_back(page);
        }
        file.close();
    }

    void replaceFIFO(int page) {
        if (fifo_queue.size() == frames) {
            int victim = fifo_queue.front();
            fifo_queue.pop();
            page_table.erase(victim);
        }
        fifo_queue.push(page);
        page_table[page] = page; // Almacena el valor directamente
    }

    void replaceLRU(int page) {
        if (page_table.size() == frames) {
            int victim = lru_cache.back();
            lru_cache.pop_back();
            page_table.erase(victim);
        }
        lru_cache.push_front(page);
        page_table[page] = page; // Almacena el valor directamente
    }

    void replaceClock(int page) {
        while (true) {
            auto& [stored_page, use_bit] = clock_vector[clock_hand];
            if (!use_bit) {
                page_table.erase(stored_page);
                stored_page = page;
                use_bit = true;
                page_table[page] = clock_hand;
                break;
            }
            use_bit = false;
            clock_hand = (clock_hand + 1) % frames;
        }
    }

    void replaceOptimal(int page, size_t current_index) {
        if (page_table.size() == frames) {
            int farthest = -1;
            int victim_index = -1;
            for (const auto& [stored_page, _] : page_table) {
                size_t next_use = INT_MAX;
                for (size_t i = current_index + 1; i < page_references.size(); i++) {
                    if (page_references[i] == stored_page) {
                        next_use = i;
                        break;
                    }
                }
                if (next_use > farthest) {
                    farthest = next_use;
                    victim_index = stored_page;
                }
            }
            page_table.erase(victim_index);
        }
        page_table[page] = page; // Almacena el valor directamente
    }

public:
    VirtualMemorySimulator(int num_frames, const string& algo, const string& file_name)
        : frames(num_frames), replacement_algo(algo), page_faults(0), clock_hand(0) {
        loadReferences(file_name);
        clock_vector.resize(frames, {-1, false});
    }

    void simulate() {
        for (size_t i = 0; i < page_references.size(); i++) {
            int page = page_references[i];
            if (page_table.find(page) == page_table.end()) {
                page_faults++;
                if (replacement_algo == "FIFO") {
                    replaceFIFO(page);
                } else if (replacement_algo == "LRU") {
                    replaceLRU(page);
                } else if (replacement_algo == "Clock") {
                    replaceClock(page);
                } else if (replacement_algo == "Optimal") {
                    replaceOptimal(page, i);
                }
            } else if (replacement_algo == "Clock") {
                clock_vector[page_table[page]].second = true;
            }
        }
    }

    void printResults() const {
        cout << "Simulation completed.\n";
        cout << "Total page faults: " << page_faults << endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 7) {
        cerr << "Usage: ./mvirtual -m <frames> -a <algorithm> -f <file>\n";
        return 1;
    }

    int frames = stoi(argv[2]);
    string algorithm = argv[4];
    string file_name = argv[6];

    VirtualMemorySimulator simulator(frames, algorithm, file_name);
    simulator.simulate();
    simulator.printResults();

    return 0;
}
