#include <iostream>
#include <cstdlib>
#include <string>

int main() {
    string filename = "bell.wav"; // Path to your sound file
    string command = "aplay -q " + filename; // -q makes it run quietly
    // This runs the command in the Linux terminal
    std::system(command.c_str()); 
}
