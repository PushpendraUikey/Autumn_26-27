#include <iostream>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <string>
#include <vector>

// Define possible thread states
enum class ThreadState { STARTING, RUNNING, WAITING, FINISHED };

// Central registry to track thread metadata
class ThreadMonitor {
private:
    std::unordered_map<std::thread::id, std::pair<std::string, ThreadState>> registry;
    std::mutex mtx;

public:
    // Update or register a thread's state
    void updateState(std::thread::id id, const std::string& name, ThreadState state) {
        std::lock_guard<std::mutex> lock(mtx);
        registry[id] = {name, state};
    }

    // Print the current status of all tracked threads
    void displayStatus() {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "=== Thread Monitor Status ===\n";
        std::cout << "Tracked threads: " << registry.size() << "\n";
        
        for (const auto& entry : registry) {
            std::cout << "ID: " << entry.first 
                      << " | Name: " << entry.second.first 
                      << " | State: ";
                      
            switch (entry.second.second) {
                case ThreadState::STARTING: std::cout << "STARTING\n"; break;
                case ThreadState::RUNNING:  std::cout << "RUNNING\n"; break;
                case ThreadState::WAITING:  std::cout << "WAITING\n"; break;
                case ThreadState::FINISHED: std::cout << "FINISHED\n"; break;
            }
        }
        std::cout << "=============================\n\n";
    }
};

// Global monitor instance
ThreadMonitor monitor;

// Worker function that reports its own state
void workerTask(const std::string& name) {
    auto id = std::this_thread::get_id();
    
    monitor.updateState(id, name, ThreadState::STARTING);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate startup
    
    monitor.updateState(id, name, ThreadState::RUNNING);
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate work
    
    monitor.updateState(id, name, ThreadState::WAITING);
    std::this_thread::sleep_for(std::chrono::milliseconds(300)); // Simulate waiting on I/O
    
    monitor.updateState(id, name, ThreadState::FINISHED);
}

int main() {
    std::vector<std::thread> workers;

    // Spawn threads
    for (int i = 1; i <= 3; ++i) {
        workers.emplace_back(workerTask, "Worker-" + std::to_string(i));
    }

    // Monitor them from the main thread while they run
    for (int i = 0; i < 4; ++i) {
        monitor.displayStatus();
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    // Clean up
    for (auto& t : workers) {
        if (t.joinable()) {
            t.join();
        }
    }
    
    // Final status after all threads finish
    monitor.displayStatus();

    return 0;
}

/*
In this example, as soon as a worker thread starts, it registers itself with the ThreadMonitor and updates its state
at various points in its execution. The main thread periodically queries the monitor to display the current state of
all the tracked threads. This allows for real-time monitoring of thread activity, which can be invaluable for debugging
and performance analysis in multi-threaded applications.
*/