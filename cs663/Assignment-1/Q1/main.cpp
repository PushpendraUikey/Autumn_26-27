#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <sstream> 

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
// MATH & INTERPOLATION HELPERS
// ==========================================

// Clamps to edge (Used for Scaling)
float getPixel(const std::vector<float>& img, int r, int c, int M, int N, int ch, int channels) {
    r = std::max(0, std::min(r, M - 1));
    c = std::max(0, std::min(c, N - 1));
    return img[(r * N + c) * channels + ch];
}

// Pads with black (Used for Rotation)
float getRotationPixelSafe(const std::vector<float>& img, float origX, float origY, int W, int H, int c, int channels) {
    int x1 = std::floor(origX);
    int y1 = std::floor(origY);
    int x2 = std::min(x1 + 1, W - 1);
    int y2 = std::min(y1 + 1, H - 1);

    if (x1 < 0 || x1 >= W || y1 < 0 || y1 >= H) return 0.0f;

    float dx = origX - x1;
    float dy = origY - y1;

    float p00 = img[(y1 * W + x1) * channels + c];
    float p10 = img[(y1 * W + x2) * channels + c];
    float p01 = img[(y2 * W + x1) * channels + c];
    float p11 = img[(y2 * W + x2) * channels + c];

    float top = p00 * (1.0f - dx) + p10 * dx;
    float bottom = p01 * (1.0f - dx) + p11 * dx;

    return top * (1.0f - dy) + bottom * dy;
}

float mapCoordinate(int x, int oldMax, int newMax) {
    if (newMax == 0) return 0.0f;
    return static_cast<float>(x) * oldMax / newMax;
}

float cubicWeight(float x) {
    float absx = std::abs(x);
    float absx2 = absx * absx;
    float absx3 = absx2 * absx;
    if (absx <= 1.0f) return 1.5f * absx3 - 2.5f * absx2 + 1.0f;
    if (absx <= 2.0f) return -0.5f * absx3 + 2.5f * absx2 - 4.0f * absx + 2.0f;
    return 0.0f;
}

// ==========================================
// GENERIC MULTI-CHANNEL SCALING ENGINES
// ==========================================

std::vector<float> enlargeNearestNeighbor(const std::vector<float>& img, int m, int n, int M, int N, int channels) {
    std::vector<float> enlarged(M * N * channels);
    for (int r = 0; r < M; ++r) {
        for (int c = 0; c < N; ++c) {
            int origR = std::round(mapCoordinate(r, m - 1, M - 1));
            int origC = std::round(mapCoordinate(c, n - 1, N - 1));
            for (int ch = 0; ch < channels; ++ch) {
                enlarged[(r * N + c) * channels + ch] = getPixel(img, origR, origC, m, n, ch, channels);
            }
        }
    }
    return enlarged;
}

std::vector<float> enlargeBilinear(const std::vector<float>& img, int m, int n, int M, int N, int channels) {
    std::vector<float> enlarged(M * N * channels);
    for (int r = 0; r < M; ++r) {
        for (int c = 0; c < N; ++c) {
            float origR = mapCoordinate(r, m - 1, M - 1);
            float origC = mapCoordinate(c, n - 1, N - 1);
            int r1 = std::floor(origR);
            int c1 = std::floor(origC);
            float dr = origR - r1;
            float dc = origC - c1;
            for (int ch = 0; ch < channels; ++ch) {
                float p00 = getPixel(img, r1, c1, m, n, ch, channels);
                float p01 = getPixel(img, r1, c1 + 1, m, n, ch, channels);
                float p10 = getPixel(img, r1 + 1, c1, m, n, ch, channels);
                float p11 = getPixel(img, r1 + 1, c1 + 1, m, n, ch, channels);
                float top = p00 * (1.0f - dc) + p01 * dc;
                float bottom = p10 * (1.0f - dc) + p11 * dc;
                enlarged[(r * N + c) * channels + ch] = top * (1.0f - dr) + bottom * dr;
            }
        }
    }
    return enlarged;
}

std::vector<float> enlargeBicubic(const std::vector<float>& img, int m, int n, int M, int N, int channels) {
    std::vector<float> enlarged(M * N * channels);
    for (int r = 0; r < M; ++r) {
        for (int c = 0; c < N; ++c) {
            float origR = mapCoordinate(r, m - 1, M - 1);
            float origC = mapCoordinate(c, n - 1, N - 1);
            int r0 = std::floor(origR);
            int c0 = std::floor(origC);
            float dr = origR - r0;
            float dc = origC - c0;
            for (int ch = 0; ch < channels; ++ch) {
                float pixelSum = 0.0f;
                for (int i = -1; i <= 2; ++i) {
                    for (int j = -1; j <= 2; ++j) {
                        float p = getPixel(img, r0 + i, c0 + j, m, n, ch, channels);
                        pixelSum += p * cubicWeight(dr - i) * cubicWeight(dc - j);
                    }
                }
                enlarged[(r * N + c) * channels + ch] = pixelSum;
            }
        }
    }
    return enlarged;
}

// ==========================================
// ASSIGNMENT REQUIRED SIGNATURES (WRAPPERS)
// ==========================================

std::vector<float> myImageShrink(const std::vector<float>& image, int width, int height, int channels, int d, int& newWidth, int& newHeight) {
    newWidth = width / d;
    newHeight = height / d; 
    std::vector<float> newImage(newWidth * newHeight * channels);
    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            for (int c = 0; c < channels; ++c) {
                int origX = x * d;
                int origY = y * d;
                newImage[(y * newWidth + x) * channels + c] = image[(origY * width + origX) * channels + c]; 
            }
        }
    }
    return newImage;
}

std::vector<float> myNearestNeighborInterpolation(const std::vector<float>& image, int M, int N, int& outM, int& outN) {
    outM = 300 * (M - 1) + 1;
    outN = 300 * (N - 1) + 1;
    return enlargeNearestNeighbor(image, M, N, outM, outN, 1); 
}

std::vector<float> myBilinearInterpolation(const std::vector<float>& image, int M, int N, int& outM, int& outN) {
    outM = 300 * (M - 1) + 1;
    outN = 300 * (N - 1) + 1;
    return enlargeBilinear(image, M, N, outM, outN, 1);
}

std::vector<float> myBicubicInterpolation(const std::vector<float>& image, int M, int N, int& outM, int& outN) {
    outM = 300 * (M - 1) + 1;
    outN = 300 * (N - 1) + 1;
    return enlargeBicubic(image, M, N, outM, outN, 1);
}

// ==========================================
// ROTATION IMPLEMENTATIONS
// ==========================================

std::vector<float> myImageRotationUsingBilinearInterp(const std::vector<float>& image, int W, int H, int channels, float angleDegrees) {
    std::vector<float> rotated(W * H * channels, 0.0f); 
    float theta = angleDegrees * M_PI / 180.0f;
    float cosT = std::cos(theta);
    float sinT = std::sin(theta);
    float cx = W / 2.0f, cy = H / 2.0f;

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            float dx = x - cx, dy = y - cy;
            float origX = dx * cosT + dy * sinT + cx;
            float origY = -dx * sinT + dy * cosT + cy;
            for (int c = 0; c < channels; ++c) {
                rotated[(y * W + x) * channels + c] = getRotationPixelSafe(image, origX, origY, W, H, c, channels);
            }
        }
    }
    return rotated;
}

std::vector<float> myImageRotationUsingNearestNeighborInterp(const std::vector<float>& image, int W, int H, int channels, float angleDegrees) {
    std::vector<float> rotated(W * H * channels, 0.0f); 
    float theta = angleDegrees * M_PI / 180.0f;
    float cosT = std::cos(theta), sinT = std::sin(theta);
    float cx = W / 2.0f, cy = H / 2.0f;

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            float dx = x - cx, dy = y - cy;
            int nx = std::round(dx * cosT + dy * sinT + cx);
            int ny = std::round(-dx * sinT + dy * cosT + cy);
            if (nx >= 0 && nx < W && ny >= 0 && ny < H) {
                for (int c = 0; c < channels; ++c) {
                    rotated[(y * W + x) * channels + c] = image[(ny * W + nx) * channels + c];
                }
            }
        }
    }
    return rotated;
}

// ==========================================
// MAIN EXECUTION
// ==========================================

int main() {
    int W, H, ch; // Reusable dimension variables

    // ------------------------------------------
    // PART 1(a): Subsampling (suit.png)
    // ------------------------------------------
    std::vector<float> suitImg = readImageToFloat("./data/suit.png", W, H, ch, 3);
    if (!suitImg.empty()) {
        int w2, h2, w3, h3;
        
        std::vector<float> shrinkD2 = myImageShrink(suitImg, W, H, 3, 2, w2, h2);
        exportImageToCSV(shrinkD2, w2, h2, 3, "temp/1_a_shrunken_d2.csv");
        
        std::vector<float> shrinkD3 = myImageShrink(suitImg, W, H, 3, 3, w3, h3);
        exportImageToCSV(shrinkD3, w3, h3, 3, "temp/1_a_shrunken_d3.csv");
        
        std::cout << "[Part 1a] Subsampling complete.\n";
    }

    // ------------------------------------------
    // PART 1(b-d): Interpolation (random.png)
    // ------------------------------------------
    std::vector<float> randomImg = readImageToFloat("./data/random.png", W, H, ch, 1);
    if (!randomImg.empty()) {
        int outW, outH;
        
        std::vector<float> nnImg = myNearestNeighborInterpolation(randomImg, H, W, outH, outW);
        exportImageToCSV(nnImg, outW, outH, 1, "temp/1_b_resized_nn.csv");
        
        std::vector<float> biImg = myBilinearInterpolation(randomImg, H, W, outH, outW);
        exportImageToCSV(biImg, outW, outH, 1, "temp/1_c_resized_bilinear.csv");
        
        std::vector<float> bicImg = myBicubicInterpolation(randomImg, H, W, outH, outW);
        exportImageToCSV(bicImg, outW, outH, 1, "temp/1_d_resized_bicubic.csv");
        
        std::cout << "[Part 1b-d] Enlargements complete.\n";
    }

    // ------------------------------------------
    // PART 1(e): Image Rotation (main.png)
    // ------------------------------------------
    std::vector<float> mainImg = readImageToFloat("./data/main.png", W, H, ch, 3);
    if (!mainImg.empty()) {
        float rotAngle = 5.8f; 
        exportImageToCSV(myImageRotationUsingNearestNeighborInterp(mainImg, W, H, 3, rotAngle), W, H, 3, "temp/1_e_rotated_nn.csv");
        exportImageToCSV(myImageRotationUsingBilinearInterp(mainImg, W, H, 3, rotAngle), W, H, 3, "temp/1_e_rotated_bilinear.csv");
        std::cout << "[Part 1e] Rotations complete.\n";
    }

    // ------------------------------------------
    // PART 1(f): Generic Upsampling (ct.mat CSVs)
    // ------------------------------------------
    int subW, subH;
    std::vector<float> ctSub = readCSVToFloat("temp/1_f_ct_subsampled.csv", subW, subH);
    std::vector<float> ctOrig = readCSVToFloat("temp/1_f_ct_original.csv", W, H);
    if (!ctSub.empty() && !ctOrig.empty()) {
        exportImageToCSV(enlargeNearestNeighbor(ctSub, subH, subW, H, W, 1), W, H, 1, "temp/1_f_enlarged_nn.csv");
        exportImageToCSV(enlargeBilinear(ctSub, subH, subW, H, W, 1), W, H, 1, "temp/1_f_enlarged_bi.csv");
        exportImageToCSV(enlargeBicubic(ctSub, subH, subW, H, W, 1), W, H, 1, "temp/1_f_enlarged_bic.csv");
        std::cout << "[Part 1f] CT Scans upsampled to " << W << "x" << H << ".\n";
    }

    std::cout << "All C++ Engine processing finished successfully!\n";
    return 0;
}