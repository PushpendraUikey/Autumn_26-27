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


std::vector<float> readImageToFloat(const std::string& filepath, int& width, int& height, int& channels, int force_channels = 0) {
    // force_channels can be set to 1 to force grayscale, or 0 to keep original channels
    unsigned char* raw_data = stbi_load(filepath.c_str(), &width, &height, &channels, force_channels);
    
    if (raw_data == nullptr) {
        std::cerr << "Failed to load image: " << filepath << "\n";
        return {}; // Return empty vector if file not found
    }

    int active_channels = (force_channels == 0) ? channels : force_channels;
    int total_elements = width * height * active_channels;

    std::vector<float> float_image(total_elements);
    for (int i = 0; i < total_elements; ++i) {
        // Note: You can also divide by 255.0f here if you prefer working in a normalized [0, 1] range.
        float_image[i] = static_cast<float>(raw_data[i]); 
    }

    // 3. Free the C-style memory allocated by stb_image to prevent memory leaks
    stbi_image_free(raw_data);

    return float_image;
}

// Helper to read the Python-exported CSV matrices back into 1D C++ vectors
std::vector<float> readCSVToFloat(const std::string& filepath, int& width, int& height) {
    std::vector<float> data;
    std::ifstream file(filepath);
    std::string line;
    height = 0;
    width = 0;
    
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
        if (width == 0) width = current_width; // Set width based on first row
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


std::vector<float> myImageShrink(const std::vector<float>& image, int width, int height, int channels, int d, int& newWidth, int& newHeight) {
    newWidth = width / d;
    newHeight = height / d; 
    
    // Allocate memory for width * height * 3 (RGB channels)
    std::vector<float> newImage(newWidth * newHeight * channels);

    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            for (int c = 0; c < channels; ++c) {
                // Calculate the corresponding original pixel coordinates
                int origX = x * d;
                int origY = y * d;
                
                // Address formula: (Row * Width + Column) * Channels + ChannelOffset
                int origIndex = (origY * width + origX) * channels + c;
                int newIndex = (y * newWidth + x) * channels + c;
                
                newImage[newIndex] = image[origIndex]; 
            }
        }
    }
    return newImage;
}


// Note: M represents rows (height), N represents columns (width)
std::vector<float> myNearestNeighborInterpolation(
    const std::vector<float>& image, int M, int N, int& outM, int& outN) {
    
    // Calculate new dimensions exactly as requested
    outM = 300 * (M - 1) + 1;
    outN = 300 * (N - 1) + 1;
    
    std::vector<float> resizedImage(outM * outN);
    
    for (int r = 0; r < outM; ++r) {
        for (int c = 0; c < outN; ++c) {
            
            // Map back to the original grid using the 1/300 scaling factor
            int origR = std::round(r / 300.0f);
            int origC = std::round(c / 300.0f);
            
            // Safety clamp to ensure edge pixels don't cause an out-of-bounds error
            origR = std::max(0, std::min(origR, M - 1));
            origC = std::max(0, std::min(origC, N - 1));
            
            // Map the 1D indices
            int outIndex = r * outN + c;
            int origIndex = origR * N + origC;
            
            resizedImage[outIndex] = image[origIndex];
        }
    }
    
    return resizedImage;
}

// Helper to safely fetch pixels and handle edge clamp boundaries
float getPixel(const std::vector<float>& img, int r, int c, int M, int N) {
    r = std::max(0, std::min(r, M - 1));
    c = std::max(0, std::min(c, N - 1));
    return img[r * N + c];
}

// 1(c): Bilinear Interpolation
std::vector<float> myBilinearInterpolation(const std::vector<float>& image, int M, int N, int& outM, int& outN) {
    outM = 300 * (M - 1) + 1;
    outN = 300 * (N - 1) + 1;
    std::vector<float> resized(outM * outN);
    
    for (int r = 0; r < outM; ++r) {
        for (int c = 0; c < outN; ++c) {
            float origR = r / 300.0f;
            float origC = c / 300.0f;
            
            int r1 = std::floor(origR);
            int c1 = std::floor(origC);
            
            float dr = origR - r1;
            float dc = origC - c1;
            
            float p00 = getPixel(image, r1, c1, M, N);
            float p01 = getPixel(image, r1, c1 + 1, M, N);
            float p10 = getPixel(image, r1 + 1, c1, M, N);
            float p11 = getPixel(image, r1 + 1, c1 + 1, M, N);
            
            // Interpolate horizontally, then vertically
            float top = p00 * (1.0f - dc) + p01 * dc;
            float bottom = p10 * (1.0f - dc) + p11 * dc;
            resized[r * outN + c] = top * (1.0f - dr) + bottom * dr;
        }
    }
    return resized;
}

// Bicubic Weighting Function (assuming a = -0.5)
float cubicWeight(float x) {
    float absx = std::abs(x);
    float absx2 = absx * absx;
    float absx3 = absx2 * absx;
    
    if (absx <= 1.0f) {
        return 1.5f * absx3 - 2.5f * absx2 + 1.0f;
    } else if (absx <= 2.0f) {
        return -0.5f * absx3 + 2.5f * absx2 - 4.0f * absx + 2.0f;
    }
    return 0.0f;
}

// 1(d): Bicubic Interpolation
std::vector<float> myBicubicInterpolation(const std::vector<float>& image, int M, int N, int& outM, int& outN) {
    outM = 300 * (M - 1) + 1;
    outN = 300 * (N - 1) + 1;
    std::vector<float> resized(outM * outN);
    
    for (int r = 0; r < outM; ++r) {
        for (int c = 0; c < outN; ++c) {
            float origR = r / 300.0f;
            float origC = c / 300.0f;
            
            int r0 = std::floor(origR);
            int c0 = std::floor(origC);
            
            float dr = origR - r0;
            float dc = origC - c0;
            
            float pixelSum = 0.0f;
            
            // 4x4 Neighborhood matrix traversal
            for (int i = -1; i <= 2; ++i) {
                for (int j = -1; j <= 2; ++j) {
                    float p = getPixel(image, r0 + i, c0 + j, M, N);
                    float weightR = cubicWeight(dr - i);
                    float weightC = cubicWeight(dc - j);
                    pixelSum += p * weightR * weightC;
                }
            }
            resized[r * outN + c] = pixelSum;
        }
    }
    return resized;
}

// 1(e) Rotation

// Helper function to safely fetch a pixel with Bilinear Interpolation
float getBilinearPixel(const std::vector<float>& img, float origX, float origY, int W, int H, int c, int channels) {
    int x1 = std::floor(origX);
    int y1 = std::floor(origY);
    int x2 = std::min(x1 + 1, W - 1);
    int y2 = std::min(y1 + 1, H - 1);

    // Boundary check - if the inverse maps outside the original image, return black
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

// 1(e) Rotation with Bilinear Interpolation
std::vector<float> myImageRotationUsingBilinearInterp(const std::vector<float>& image, int W, int H, int channels, float angleDegrees) {
    std::vector<float> rotated(W * H * channels, 0.0f); // Initialize with black
    
    // Convert angle to radians
    float theta = angleDegrees * M_PI / 180.0f;
    float cosT = std::cos(theta);
    float sinT = std::sin(theta);
    
    float cx = W / 2.0f;
    float cy = H / 2.0f;

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            // 1. Shift to origin
            float dx = x - cx;
            float dy = y - cy;

            // 2. Inverse Rotation
            float origX = dx * cosT + dy * sinT + cx;
            float origY = -dx * sinT + dy * cosT + cy;

            // 3. Interpolate for each color channel
            for (int c = 0; c < channels; ++c) {
                rotated[(y * W + x) * channels + c] = getBilinearPixel(image, origX, origY, W, H, c, channels);
            }
        }
    }
    return rotated;
}

// 1(e) Rotation with Nearest-Neighbor Interpolation
std::vector<float> myImageRotationUsingNearestNeighborInterp(const std::vector<float>& image, int W, int H, int channels, float angleDegrees) {
    std::vector<float> rotated(W * H * channels, 0.0f); 
    
    float theta = angleDegrees * M_PI / 180.0f;
    float cosT = std::cos(theta);
    float sinT = std::sin(theta);
    
    float cx = W / 2.0f;
    float cy = H / 2.0f;

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            float dx = x - cx;
            float dy = y - cy;

            float origX = dx * cosT + dy * sinT + cx;
            float origY = -dx * sinT + dy * cosT + cy;

            // Nearest Neighbor rounding
            int nx = std::round(origX);
            int ny = std::round(origY);

            // Boundary check
            if (nx >= 0 && nx < W && ny >= 0 && ny < H) {
                for (int c = 0; c < channels; ++c) {
                    rotated[(y * W + x) * channels + c] = image[(ny * W + nx) * channels + c];
                }
            }
        }
    }
    return rotated;
}

// Generic Boundary Mapping Helper[cite: 1]
float mapCoordinate(int x, int oldMax, int newMax) {
    if (newMax == 0) return 0.0f;
    return static_cast<float>(x) * oldMax / newMax;
}

// 1(f) Generic Nearest-Neighbor
std::vector<float> enlargeNearestNeighbor(const std::vector<float>& img, int m, int n, int M, int N) {
    std::vector<float> enlarged(M * N);
    for (int r = 0; r < M; ++r) {
        for (int c = 0; c < N; ++c) {
            int origR = std::round(mapCoordinate(r, m - 1, M - 1));
            int origC = std::round(mapCoordinate(c, n - 1, N - 1));
            
            origR = std::max(0, std::min(origR, m - 1));
            origC = std::max(0, std::min(origC, n - 1));
            
            enlarged[r * N + c] = img[origR * n + origC];
        }
    }
    return enlarged;
}

// 1(f) Generic Bilinear
std::vector<float> enlargeBilinear(const std::vector<float>& img, int m, int n, int M, int N) {
    std::vector<float> enlarged(M * N);
    for (int r = 0; r < M; ++r) {
        for (int c = 0; c < N; ++c) {
            float origR = mapCoordinate(r, m - 1, M - 1);
            float origC = mapCoordinate(c, n - 1, N - 1);
            
            int r1 = std::floor(origR);
            int c1 = std::floor(origC);
            float dr = origR - r1;
            float dc = origC - c1;
            
            float p00 = getPixel(img, r1, c1, m, n);
            float p01 = getPixel(img, r1, c1 + 1, m, n);
            float p10 = getPixel(img, r1 + 1, c1, m, n);
            float p11 = getPixel(img, r1 + 1, c1 + 1, m, n);
            
            float top = p00 * (1.0f - dc) + p01 * dc;
            float bottom = p10 * (1.0f - dc) + p11 * dc;
            enlarged[r * N + c] = top * (1.0f - dr) + bottom * dr;
        }
    }
    return enlarged;
}

// 1(f) Generic Bicubic
std::vector<float> enlargeBicubic(const std::vector<float>& img, int m, int n, int M, int N) {
    std::vector<float> enlarged(M * N);
    for (int r = 0; r < M; ++r) {
        for (int c = 0; c < N; ++c) {
            float origR = mapCoordinate(r, m - 1, M - 1);
            float origC = mapCoordinate(c, n - 1, N - 1);
            
            int r0 = std::floor(origR);
            int c0 = std::floor(origC);
            float dr = origR - r0;
            float dc = origC - c0;
            
            float pixelSum = 0.0f;
            for (int i = -1; i <= 2; ++i) {
                for (int j = -1; j <= 2; ++j) {
                    float p = getPixel(img, r0 + i, c0 + j, m, n);
                    float weightR = cubicWeight(dr - i);
                    float weightC = cubicWeight(dc - j);
                    pixelSum += p * weightR * weightC;
                }
            }
            enlarged[r * N + c] = pixelSum;
        }
    }
    return enlarged;
}


int main() {

    // ==========================================
    // PART 1(a): Image Shrinking - Subsampling
    // ==========================================
    int width, height, channels;
    std::string filepath = "./data/suit.png"; 

    // 1. Load the original image forcing 3 channels for RGB
    std::vector<float> originalImage = readImageToFloat(filepath, width, height, channels, 3);
    
    if (originalImage.empty()) {
        std::cerr << "Execution aborted: Could not load the image.\n";
        return -1;
    }
    
    std::cout << "Original Image Loaded: " << width << "x" << height << "\n";

    // 2. Process and Export for d = 2
    int newWidthD2, newHeightD2;
    int d2 = 2;
    std::vector<float> shrinkD2 = myImageShrink(originalImage, width, height, 3, d2, newWidthD2, newHeightD2);
    exportImageToCSV(shrinkD2, newWidthD2, newHeightD2, 3, "temp/shrunken_d2.csv");
    std::cout << "Exported d=2 image (" << newWidthD2 << "x" << newHeightD2 << ")\n";

    // 3. Process and Export for d = 3
    int newWidthD3, newHeightD3;
    int d3 = 3;
    std::vector<float> shrinkD3 = myImageShrink(originalImage, width, height, 3, d3, newWidthD3, newHeightD3);
    exportImageToCSV(shrinkD3, newWidthD3, newHeightD3, 3, "temp/shrunken_d3.csv");
    std::cout << "Exported d=3 image (" << newWidthD3 << "x" << newHeightD3 << ")\n";
    

    // ==========================================
    // PART 1(b): Nearest-Neighbor Interpolation
    // ==========================================
    std::string randomFilepath = "./data/random.png";
    int M, N, randomChannels;
    // Load as 1-channel grayscale so we can apply the 'jet' colormap later
    std::vector<float> randomImage = readImageToFloat(randomFilepath, N, M, randomChannels, 1);
    
    if (randomImage.empty()) {
        std::cerr << "Execution aborted: Could not load random.png.\n";
        return -1;
    }
    std::cout << "Original Random Image Loaded: " << M << " rows, " << N << " columns.\n";

    int outM, outN;
    std::vector<float> resizedNN = myNearestNeighborInterpolation(randomImage, M, N, outM, outN);
    exportImageToCSV(resizedNN, outN, outM, 1, "temp/resized_nn.csv");
    std::cout << "Exported Nearest-Neighbor image (" << outM << "x" << outN << ")\n";

    // 1(c): Bilinear
    std::vector<float> resizedBilinear = myBilinearInterpolation(randomImage, M, N, outM, outN);
    exportImageToCSV(resizedBilinear, outN, outM, 1, "temp/resized_bilinear.csv");
    std::cout << "Exported Bilinear image.\n";

    // 1(d): Bicubic
    std::vector<float> resizedBicubic = myBicubicInterpolation(randomImage, M, N, outM, outN);
    exportImageToCSV(resizedBicubic, outN, outM, 1, "temp/resized_bicubic.csv");
    std::cout << "Exported Bicubic image.\n";

    // ==========================================
    // PART 1(e): Image Rotation
    // ==========================================
    // The assignment specifies main.png, adjust to main.jpg if necessary
    std::string mainFilepath = "./data/main.png"; 
    int W, H, mainChannels;
    
    // Load as a 3-channel RGB image
    std::vector<float> mainImage = readImageToFloat(mainFilepath, W, H, mainChannels, 3);
    
    if (mainImage.empty()) {
        std::cerr << "Execution aborted: Could not load main image.\n";
        return -1;
    }
    
    std::cout << "Original Main Image Loaded: " << W << "x" << H << "\n";

    // Positive values rotate clockwise.
    float rotationAngle = 5.8f; 

    // Rotate using Nearest-Neighbor[cite: 1]
    std::vector<float> rotatedNN = myImageRotationUsingNearestNeighborInterp(mainImage, W, H, 3, rotationAngle);
    exportImageToCSV(rotatedNN, W, H, 3, "temp/rotated_nn.csv");
    std::cout << "Exported Nearest-Neighbor rotated image.\n";

    // Rotate using Bilinear[cite: 1]
    std::vector<float> rotatedBilinear = myImageRotationUsingBilinearInterp(mainImage, W, H, 3, rotationAngle);
    exportImageToCSV(rotatedBilinear, W, H, 3, "temp/rotated_bilinear.csv");
    std::cout << "Exported Bilinear rotated image.\n";
    

    // ==========================================
    // PART 1(f): Generic Image Upsampling (CT Scans)
    // ==========================================
    int m, n; // Subsampled dimensions
    int M, N; // Original (Target) dimensions

    // 1. Load the matrices extracted by Python
    std::vector<float> ctSubsampled = readCSVToFloat("temp/ct_subsampled.csv", n, m);
    std::vector<float> ctOriginal = readCSVToFloat("temp/ct_original.csv", N, M);

    if (ctSubsampled.empty() || ctOriginal.empty()) {
        std::cerr << "Error: CT scan CSVs not found. Run extract_mat.py first!\n";
        return -1;
    }

    std::cout << "CT Original Loaded: " << M << "x" << N << "\n";
    std::cout << "CT Subsampled Loaded: " << m << "x" << n << "\n";

    // 2. Enlarge using the 3 methods to exactly match the Original's dimensions[cite: 1]
    std::vector<float> ctEnlargedNN = enlargeNearestNeighbor(ctSubsampled, m, n, M, N);
    exportImageToCSV(ctEnlargedNN, N, M, 1, "temp/enlarged_nn.csv");
    std::cout << "Exported Generic Nearest-Neighbor.\n";

    std::vector<float> ctEnlargedBi = enlargeBilinear(ctSubsampled, m, n, M, N);
    exportImageToCSV(ctEnlargedBi, N, M, 1, "temp/enlarged_bi.csv");
    std::cout << "Exported Generic Bilinear.\n";

    std::vector<float> ctEnlargedBic = enlargeBicubic(ctSubsampled, m, n, M, N);
    exportImageToCSV(ctEnlargedBic, N, M, 1, "temp/enlarged_bic.csv");
    std::cout << "Exported Generic Bicubic.\n";
    return 0;
}