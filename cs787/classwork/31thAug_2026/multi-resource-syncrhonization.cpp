#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

// A shared resource structure with its own mutex
struct Account {
    int id;
    double balance;
    std::mutex mtx;

    Account(int i, double b) : id(i), balance(b) {}
};

// --- Multi-Resource Synchronization ---
void transferMoney(Account& from, Account& to, double amount) {
    // Prevent locking the same mutex twice (undefined behavior)
    if (&from == &to) return;

    // std::scoped_lock locks BOTH mutexes safely without risking a deadlock.
    // If it can't get both, it backs off and tries again.
    std::scoped_lock lock(from.mtx, to.mtx);

    // CRITICAL SECTION: We now have exclusive access to both 'from' and 'to'
    if (from.balance >= amount) {
        from.balance -= amount;
        
        // Simulate a tiny delay during the transaction
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); 
        
        to.balance += amount;
        
        std::cout << "Transferred $" << amount << " from Account " << from.id 
                  << " to Account " << to.id << "\n";
    } else {
        std::cout << "Transfer failed: Insufficient funds in Account " << from.id << "\n";
    }
    // Mutexes are automatically released when 'lock' goes out of scope here
}

// Worker function to simulate high-frequency transfers
void transactionWorker(Account& a, Account& b, double amount) {
    for (int i = 0; i < 5; ++i) {
        transferMoney(a, b, amount);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    Account acc1(1, 1000.0);
    Account acc2(2, 500.0);

    // Spawn threads that attempt cross-transfers simultaneously.
    // Without std::scoped_lock, this exact scenario easily causes deadlocks.
    std::thread t1(transactionWorker, std::ref(acc1), std::ref(acc2), 100.0);
    std::thread t2(transactionWorker, std::ref(acc2), std::ref(acc1), 50.0);

    t1.join();
    t2.join();

    std::cout << "\n=== Final Balances ===\n";
    std::cout << "Account 1: $" << acc1.balance << "\n";
    std::cout << "Account 2: $" << acc2.balance << "\n";

    return 0;
}