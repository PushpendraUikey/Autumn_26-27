/*
    Following program is generated using the Gemini AI code generator.
    23B1023
*/

#include <iostream>
#include <string>
#include <unordered_map>

// 1. Polymorphic Base Class
// A virtual destructor is required to make the class polymorphic,
// which is strictly necessary for dynamic_cast to work.
class Object {
public:
    virtual ~Object() = default;
};

// 2. Concrete Child Classes
class DatabaseConnection : public Object {
public:
    void query() { std::cout << "Executing database query...\n"; }
};

class Logger : public Object {
public:
    void log(const std::string& msg) { std::cout << "Log: " << msg << "\n"; }
};

// 3. The Naming Service
class NamingService {
private:
    // Stores the (Name, Object pointer) tuples
    std::unordered_map<std::string, Object*> registry;

public:
    void registerObject(const std::string& name, Object* obj) {
        registry[name] = obj;
        std::cout << "Registered: " << name << "\n";
    }

    Object* getObject(const std::string& name) {
        auto it = registry.find(name);
        if (it != registry.end()) {
            return it->second;
        }
        std::cout << "Error: '" << name << "' not found in registry.\n";
        return nullptr;
    }
};

int main() {
    NamingService service;

    // Registering tuples (Name, Object pointer)
    service.registerObject("MainDB", new DatabaseConnection());
    service.registerObject("AppLogger", new Logger());

    std::cout << "\n--- Extracting and Casting ---\n";

    // Scenario A: Correctly extracting and casting a DatabaseConnection
    Object* dbObj = service.getObject("MainDB");
    if (DatabaseConnection* db = dynamic_cast<DatabaseConnection*>(dbObj)) {
        db->query(); // Use it correctly
    } else {
        std::cout << "Error: 'MainDB' is not a DatabaseConnection. Skipping.\n";
    }

    // Scenario B: Erroneous cast (Flag and skip)
    // Trying to cast the Logger object into a DatabaseConnection
    Object* logObj = service.getObject("AppLogger");
    if (DatabaseConnection* falseDb = dynamic_cast<DatabaseConnection*>(logObj)) {
        falseDb->query();
    } else {
        // dynamic_cast returns nullptr if the object isn't actually a DatabaseConnection
        std::cout << "Error: Type mismatch for 'AppLogger'. It is not a DatabaseConnection. Skipping.\n";
    }

    // Cleanup (in a real app, use std::unique_ptr or similar)
    delete service.getObject("MainDB");
    delete service.getObject("AppLogger");

    return 0;
}