#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <sstream> 
#include <numeric>

using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ==========================================
// FILE I/O HELPERS
// ==========================================

std::vector<float> readImageToFloat(const std::string& filepath, int& width, int& height, int& channels, int force_channels = 0) {
    unsigned char* raw_data = stbi_load(filepath.c_str(), &width, &height, &channels, force_channels);
    if (raw_data == nullptr) {
        std::cerr << "Failed to load image: " << filepath << "\n";
        return {}; 
    }
    int active_channels = (force_channels == 0) ? channels : force_channels;
    int total_elements = width * height * active_channels;
    std::vector<float> float_image(total_elements);
    for (int i = 0; i < total_elements; ++i) {
        float_image[i] = static_cast<float>(raw_data[i]); 
    }
    stbi_image_free(raw_data);
    return float_image;
}

std::vector<float> readCSVToFloat(const std::string& filepath, int& width, int& height) {
    std::vector<float> data;
    std::ifstream file(filepath);
    std::string line;
    height = 0; width = 0;
    
    if (!file.is_open()) {
        std::cerr << "Failed to open CSV: " << filepath << "\n";
        return data;
    }
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        height++;
        int current_width = 0;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            data.push_back(std::stof(cell));
            current_width++;
        }
        if (width == 0) width = current_width; 
    }
    return data;
}

void exportImageToCSV(const std::vector<float>& image, int width, int height, int channels, const std::string& filepath) {
    std::ofstream outFile(filepath);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width * channels; ++x) {
            outFile << image[y * (width * channels) + x];
            if (x < (width * channels) - 1) outFile << ",";
        }
        outFile << "\n";
    }
    outFile.close();
}

// ==========================================
// Q2 ALGORITHMS WILL GO HERE
// ==========================================

// ==========================================
// 2(a): Manual Thresholding
// ==========================================
std::vector<float> myManualThresholding(const std::vector<float>& image, int W, int H, float threshold, bool invert) {
    std::vector<float> binarized(W * H);
    for (int i = 0; i < W * H; ++i) {
        bool isText = (image[i] < threshold); // Assuming dark text on light bg by default
        if (invert) isText = (image[i] > threshold); // Flip logic for light text on dark bg (e.g., blackboard)
        
        binarized[i] = isText ? 0.0f : 255.0f; // Text = Black (0), BG = White (255)
    }
    return binarized;
}

// ==========================================
// 2(b): Otsu Thresholding
// ==========================================
std::vector<float> myOtsuThresholding(const std::vector<float>& image, int W, int H, bool invert) {
    // 1. Calculate Histogram
    std::vector<int> hist(256, 0);
    for (float val : image) {
        int v = std::max(0, std::min(255, static_cast<int>(std::round(val))));
        hist[v]++;
    }

    int totalPixels = W * H;
    float sumTotal = 0;
    for (int i = 0; i < 256; ++i) sumTotal += i * hist[i];

    float sumB = 0;
    int wB = 0, wF = 0;
    float varMax = 0;
    int optimalThreshold = 0;

    // 2. Exhaustive search for maximum between-class variance
    for (int t = 0; t < 256; ++t) {
        wB += hist[t];               // Weight Background
        if (wB == 0) continue;
        wF = totalPixels - wB;       // Weight Foreground[cite: 2]
        if (wF == 0) break;

        sumB += static_cast<float>(t * hist[t]);
        
        float mB = sumB / wB;            // Mean Background[cite: 2]
        float mF = (sumTotal - sumB) / wF; // Mean Foreground[cite: 2]

        // Between-class variance formula[cite: 2]
        float varBetween = static_cast<float>(wB) * static_cast<float>(wF) * (mB - mF) * (mB - mF);

        if (varBetween > varMax) {
            varMax = varBetween;
            optimalThreshold = t;
        }
    }

    std::cout << "Computed Otsu Threshold: " << optimalThreshold << "\n";
    return myManualThresholding(image, W, H, optimalThreshold, invert);
}

// ==========================================
// 2(c): Local / Adaptive Thresholding (Niblack)
// ==========================================
// The assignment requires displaying the threshold map[cite: 3].
// We will return a pair: {Binarized_Image, Threshold_Map}
// std::pair<std::vector<float>, std::vector<float>> myAdaptiveThresholding(
//     const std::vector<float>& image, int W, int H, int windowSize, float k, bool invert) {
    
//     std::vector<float> binarized(W * H);
//     std::vector<float> thresholdMap(W * H);
//     int offset = windowSize / 2;

//     for (int y = 0; y < H; ++y) {
//         for (int x = 0; x < W; ++x) {
//             float sum = 0.0f, sqSum = 0.0f;
//             int count = 0;

//             // Extract local neighborhood statistics
//             for (int wy = -offset; wy <= offset; ++wy) {
//                 for (int wx = -offset; wx <= offset; ++wx) {
//                     int ny = std::max(0, std::min(H - 1, y + wy));
//                     int nx = std::max(0, std::min(W - 1, x + wx));
//                     float val = image[ny * W + nx];
//                     sum += val;
//                     sqSum += val * val;
//                     count++;
//                 }
//             }

//             float mean = sum / count;
//             float variance = (sqSum / count) - (mean * mean);
//             float stddev = std::sqrt(std::max(0.0f, variance));

//             // Niblack's threshold formula: T = mean + k * stddev[cite: 2]
//             float T = mean + k * stddev;
//             thresholdMap[y * W + x] = T;

//             bool isText = (image[y * W + x] < T);
//             if (invert) isText = (image[y * W + x] > T);

//             binarized[y * W + x] = isText ? 0.0f : 255.0f; // Text=Black, BG=White[cite: 3]
//         }
//     }
//     return {binarized, thresholdMap};
// }

// ==========================================
// 2(c): Local / Adaptive Thresholding (Optimized with Integral Images)
// ==========================================
std::pair<std::vector<float>, std::vector<float>> myAdaptiveThresholding(
    const std::vector<float>& image, int W, int H, int windowSize, float k, bool invert) {
    
    std::vector<float> binarized(W * H);
    std::vector<float> thresholdMap(W * H);
    int offset = windowSize / 2;

    // 1. Pre-compute Integral Images (Sum and Squared Sum)[cite: 2]
    // Using double to prevent precision loss and overflow on large images
    std::vector<double> intSum(W * H, 0.0);
    std::vector<double> intSqSum(W * H, 0.0);

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            double val = image[y * W + x];
            double sqVal = val * val;

            // Fetch previous integral values[cite: 2]
            double A = (x > 0 && y > 0) ? intSum[(y - 1) * W + (x - 1)] : 0.0;
            double B = (y > 0) ? intSum[(y - 1) * W + x] : 0.0;
            double C = (x > 0) ? intSum[y * W + (x - 1)] : 0.0;
            intSum[y * W + x] = val + B + C - A; // I(x,y) = i(x,y) + I(x,y-1) + I(x-1,y) - I(x-1,y-1)[cite: 2]

            double sqA = (x > 0 && y > 0) ? intSqSum[(y - 1) * W + (x - 1)] : 0.0;
            double sqB = (y > 0) ? intSqSum[(y - 1) * W + x] : 0.0;
            double sqC = (x > 0) ? intSqSum[y * W + (x - 1)] : 0.0;
            intSqSum[y * W + x] = sqVal + sqB + sqC - sqA;
        }
    }

    // 2. Compute local thresholds in O(1) time per pixel
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            // Determine the boundaries of the local window
            int y_min = std::max(0, y - offset);
            int y_max = std::min(H - 1, y + offset);
            int x_min = std::max(0, x - offset);
            int x_max = std::min(W - 1, x + offset);

            // Compute area of this specific window (edges have smaller areas)
            double count = (x_max - x_min + 1) * (y_max - y_min + 1);

            // Fetch sums from Integral Images: Sum = D - B - C + A[cite: 2]
            double D = intSum[y_max * W + x_max];
            double B = (y_min > 0) ? intSum[(y_min - 1) * W + x_max] : 0.0;
            double C = (x_min > 0) ? intSum[y_max * W + (x_min - 1)] : 0.0;
            double A = (x_min > 0 && y_min > 0) ? intSum[(y_min - 1) * W + (x_min - 1)] : 0.0;
            double sum = D - B - C + A;

            // Fetch squared sums
            double sqD = intSqSum[y_max * W + x_max];
            double sqB = (y_min > 0) ? intSqSum[(y_min - 1) * W + x_max] : 0.0;
            double sqC = (x_min > 0) ? intSqSum[y_max * W + (x_min - 1)] : 0.0;
            double sqA = (x_min > 0 && y_min > 0) ? intSqSum[(y_min - 1) * W + (x_min - 1)] : 0.0;
            double sqSum = sqD - sqB - sqC + sqA;

            // Compute Statistics
            double mean = sum / count;
            double variance = (sqSum / count) - (mean * mean); // Var(X) = E[X^2] - (E[X])^2[cite: 2]
            double stddev = std::sqrt(std::max(0.0, variance)); // Max safeguards against floating point errors

            // Niblack's threshold formula
            // float T = static_cast<float>(mean + k * stddev);
            // Sauvola's threshold formula: T = mean + mean * k * ((stddev / R) - 1)
            // R is the dynamic range of standard deviation (typically 128 for 8-bit images)
            float R = 128.0f;
            float T = static_cast<float>(mean + mean * k * ((stddev / R) - 1.0f));
            thresholdMap[y * W + x] = T;

            // Apply binarization logic
            bool isText = (image[y * W + x] < T);
            if (invert) isText = (image[y * W + x] > T);

            binarized[y * W + x] = isText ? 0.0f : 255.0f;
        }
    }
    
    return {binarized, thresholdMap};
}


// ==========================================
// MAIN EXECUTION
// ==========================================
// Add this struct definition right above int main()
struct ThresholdConfig {
    std::string filename;
    bool invert;
    float manualThresh;
    int adaptWindowSize;
    float adaptK;
};

int main() {
    // Define the processing configurations for all 4 images
    // Note: You will need to manually tune manualThresh, adaptWindowSize, and adaptK for your final report!
    std::vector<ThresholdConfig> configs = {
        {"receipt.png", false, 100.0f, 31, -0.2f},
        {"blackboard.png", true,  100.0f, 31, -0.2f}, // Invert is true because text is lighter than background[cite: 3]
        {"lilavati.png", false, 100.0f, 31, -0.2f},
        {"qr.png", false, 100.0f, 31, -0.2f}
    };

    for (const auto& config : configs) {
        int W, H, ch;
        std::string filepath = "./data/" + config.filename;
        
        // Extract base name (e.g., "receipt" from "receipt.png")
        size_t dotPos = config.filename.find_last_of('.');
        std::string baseName = config.filename.substr(0, dotPos);

        std::vector<float> img = readImageToFloat(filepath, W, H, ch, 1);
        if (img.empty()) continue;

        std::cout << "Processing " << config.filename << "...\n";

        // 2(a) Manual
        std::vector<float> imgMan = myManualThresholding(img, W, H, config.manualThresh, config.invert);
        exportImageToCSV(imgMan, W, H, 1, "temp/" + baseName + "_manual.csv");

        // 2(b) Otsu
        std::vector<float> imgOtsu = myOtsuThresholding(img, W, H, config.invert);
        exportImageToCSV(imgOtsu, W, H, 1, "temp/" + baseName + "_otsu.csv");

        // 2(c) Adaptive
        auto adaptiveResult = myAdaptiveThresholding(img, W, H, config.adaptWindowSize, config.adaptK, config.invert);
        exportImageToCSV(adaptiveResult.first, W, H, 1, "temp/" + baseName + "_adapt_bin.csv");
        exportImageToCSV(adaptiveResult.second, W, H, 1, "temp/" + baseName + "_adapt_map.csv");
    }

    std::cout << "Q2 C++ Engine processing finished successfully!\n";
    return 0;
}