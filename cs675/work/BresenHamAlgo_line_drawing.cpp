#include <iostream>
#include <cmath>
#include <algorithm>

void drawLine(int x1, int y1, int x2, int y2) {
    int dx = std::abs(x2-x1);
    int dy = std::abs(y2-y1);

    // Determine the step direction for both axes.
    int stepX = (x1 < x2) ? 1 : -1;
    int stepY = (y1 < y2) ? 1 : -1;

    // Checking if the line is steep
    bool isSteep = dy > dx;

    if (isSteep) {
        std::swap(dy, dx);
    }

    int P = 2 * dy - dx; 
    int x = x1;
    int y = y1;

    // We always loop over the longer distance (which is now dx)
    for (int i=0; i <= dx; ++i) {
        // plot (x, y) here
        std::cout << "(" << x << ", " << y << ")\n";

        if (P >= 0) {
            // Move along the steep axis
            if (isSteep) x += stepX;
            else y += stepY;

            P = P - 2 * dx;
        }

        // Move along the driving axis
        if (isSteep) y += stepY;
        else x += stepX;

        P = P + 2 * dy;
    }
}

int main() {
    std::cout << "--- Test 1: Gentle positive slope (Octant 1) ---\n";
    std::cout << "Drawing from (0,0) to (5,2)\n";
    drawLine(0, 0, 5, 2);

    std::cout << "\n--- Test 2: Steep positive slope (Octant 2) ---\n";
    std::cout << "Drawing from (0,0) to (2,5)\n";
    drawLine(0, 0, 2, 5);

    std::cout << "\n--- Test 3: Gentle negative slope (Octant 8) ---\n";
    std::cout << "Drawing from (0,0) to (5,-2)\n";
    drawLine(0, 0, 5, -2);

    std::cout << "\n--- Test 4: Backwards gentle slope (Right-to-Left, Octant 4) ---\n";
    std::cout << "Drawing from (5,2) to (0,0)\n";
    drawLine(5, 2, 0, 0);

    std::cout << "\n--- Test 5: Backwards steep slope (Right-to-Left, Octant 6) ---\n";
    std::cout << "Drawing from (2,5) to (0,0)\n";
    drawLine(2, 5, 0, 0);

    std::cout << "\n--- Test 6: Pure Horizontal Line ---\n";
    std::cout << "Drawing from (0,0) to (4,0)\n";
    drawLine(0, 0, 4, 0);

    std::cout << "\n--- Test 7: Pure Vertical Line ---\n";
    std::cout << "Drawing from (0,0) to (0,4)\n";
    drawLine(0, 0, 0, 4);

    std::cout << "\n--- Test 8: Single Pixel (Start and End are the same) ---\n";
    std::cout << "Drawing from (3,3) to (3,3)\n";
    drawLine(3, 3, 3, 3);

    return 0;
}