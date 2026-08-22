#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

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

void convertRGBtoYCbCrInPlace(std::vector<float>& img) {
    for (size_t i = 0; i < img.size(); i += 3) {
        float R = img[i], G = img[i+1], B = img[i+2];

        // Matrix multiplication from the lecture slides[cite: 2]
        img[i]   =  0.299f * R + 0.587f * G + 0.114f * B;
        img[i+1] = -0.169f * R - 0.331f * G + 0.500f * B;
        img[i+2] =  0.500f * R - 0.419f * G - 0.081f * B;
    }
}

void convertYCbCrtoRGBInPlace(std::vector<float>& img) {
    for (size_t i = 0; i < img.size(); i += 3) {
        float Y = img[i], Cb = img[i+1], Cr = img[i+2];

        float R = Y + 1.402f * Cr;
        float G = Y - 0.344f * Cb - 0.714f * Cr;
        float B = Y + 1.772f * Cb;

        // Clamp outputs to valid ranges
        img[i]   = std::max(0.0f, std::min(255.0f, R));
        img[i+1] = std::max(0.0f, std::min(255.0f, G));
        img[i+2] = std::max(0.0f, std::min(255.0f, B));
    }
}

// ==========================================
// COLOR SPACE CONVERSION STUBS
// ==========================================
// We will implement RGB <-> HSL or YCbCr here to isolate the luminance component.

// ==========================================
// PART 3 ALGORITHM STUBS
// ==========================================

// ==========================================
// 3(a): Linear Contrast Stretching
// ==========================================
std::vector<float> myLinearContrastStretch(const std::vector<float>& rgbImage, int W, int H) {
    // 1. Convert RGB to YCbCr
    std::vector<float> ycbcr = rgbImage;
    convertRGBtoYCbCrInPlace(ycbcr);
    
    // 2. Find f_min and f_max of the Luminance (Y) channel[cite: 2]
    float f_min = 255.0f;
    float f_max = 0.0f;
    
    for (size_t i = 0; i < ycbcr.size(); i += 3) {
        float Y = ycbcr[i];
        if (Y < f_min) f_min = Y;
        if (Y > f_max) f_max = Y;
    }
    
    // 3. Apply the linear stretching formula exclusively to the Y channel[cite: 2, 3]
    if (f_max > f_min) { // Prevent division by zero
        for (size_t i = 0; i < ycbcr.size(); i += 3) {
            float Y = ycbcr[i];
            ycbcr[i] = ((Y - f_min) / (f_max - f_min)) * 255.0f;
        }
    }
    
    // 4. Convert back to RGB and return
    convertYCbCrtoRGBInPlace(ycbcr);
    return ycbcr;
}

// ==========================================
// 3(b): Histogram Equalization
// ==========================================
std::vector<float> myHistEqualize(const std::vector<float>& rgbImage, int W, int H) {
    // 1. Convert RGB to YCbCr
    std::vector<float> ycbcr = rgbImage;
    convertRGBtoYCbCrInPlace(ycbcr);
    int totalPixels = W * H;

    // 2. Compute Histogram of the Luminance (Y) channel
    std::vector<int> hist(256, 0);
    for (size_t i = 0; i < ycbcr.size(); i += 3) {
        int y_val = std::max(0, std::min(255, static_cast<int>(std::round(ycbcr[i]))));
        hist[y_val]++;
    }

    // 3. Compute Cumulative Distribution Function (CDF)[cite: 2]
    std::vector<int> cdf(256, 0);
    cdf[0] = hist[0];
    for (int i = 1; i < 256; ++i) {
        cdf[i] = cdf[i - 1] + hist[i];
    }

    // 4. Find the minimum non-zero value in the CDF 
    int cdf_min = 0;
    for (int i = 0; i < 256; ++i) {
        if (cdf[i] > 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    // 5. Map the Y channel values using the normalized CDF[cite: 2]
    float denominator = static_cast<float>(totalPixels - cdf_min);
    if (denominator > 0.0f) { // Prevent division by zero
        for (size_t i = 0; i < ycbcr.size(); i += 3) {
            int y_val = std::max(0, std::min(255, static_cast<int>(std::round(ycbcr[i]))));
            float new_Y = (static_cast<float>(cdf[y_val] - cdf_min) / denominator) * 255.0f;
            ycbcr[i] = std::max(0.0f, std::min(255.0f, new_Y));
        }
    }

    // 6. Convert back to RGB and return
    convertYCbCrtoRGBInPlace(ycbcr);
    return ycbcr;
}

// ==========================================
// 3(c): Contrast-Limited Adaptive Histogram Equalization (CLAHE)
// ==========================================
std::vector<float> myCLAHE(std::vector<float> img, int W, int H, int numBins, int winSize, float clipThreshold) {
    convertRGBtoYCbCrInPlace(img);
    
    std::vector<float> origY(W * H);
    std::vector<int> binImage(W * H); // NEW: Fast lookup table for histogram bins
    
    // PRE-COMPUTE: Calculate the bin for every pixel exactly once!
    float scale = (numBins - 1) / 255.0f;
    for (int i = 0; i < W * H; ++i) {
        origY[i] = img[i * 3];
        binImage[i] = std::max(0, std::min(numBins - 1, static_cast<int>(std::round(origY[i] * scale))));
    }
    
    int offset = winSize / 2;
    
    // Sliding Window per pixel
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            int y_min = std::max(0, y - offset);
            int y_max = std::min(H - 1, y + offset);
            int x_min = std::max(0, x - offset);
            int x_max = std::min(W - 1, x + offset);
            
            int windowArea = (y_max - y_min + 1) * (x_max - x_min + 1);
            
            std::vector<float> hist(numBins, 0.0f);
            
            // OPTIMIZED INNER LOOP: Just array lookups, no math!
            for (int wy = y_min; wy <= y_max; ++wy) {
                for (int wx = x_min; wx <= x_max; ++wx) {
                    hist[binImage[wy * W + wx]] += 1.0f;
                }
            }
            
            // Limit Contrast
            float clipLimit = std::max(1.0f, clipThreshold * windowArea);
            float excess = 0.0f;
            for (int b = 0; b < numBins; ++b) {
                if (hist[b] > clipLimit) {
                    excess += (hist[b] - clipLimit);
                    hist[b] = clipLimit;
                }
            }
            
            // Redistribute excess
            float redistribute = excess / numBins;
            for (int b = 0; b < numBins; ++b) {
                hist[b] += redistribute;
            }
            
            // Compute CDF using the pre-computed bin
            int currentBin = binImage[y * W + x]; 
            float cdf = 0.0f;
            for (int b = 0; b <= currentBin; ++b) {
                cdf += hist[b];
            }
            
            // Map the new Y value
            float newY = (cdf / windowArea) * 255.0f;
            img[(y * W + x) * 3] = std::max(0.0f, std::min(255.0f, newY));
        }
    }
    
    convertYCbCrtoRGBInPlace(img);
    return img;
}


// ==========================================
// 3(d): Histogram Matching
// ==========================================
std::vector<float> myHistMatch(std::vector<float> source, std::vector<float> reference, 
                               int W, int H, int refW, int refH, int numBins) {
    convertRGBtoYCbCrInPlace(source);
    convertRGBtoYCbCrInPlace(reference);

    // Process Y, Cb, and Cr channels independently
    for (int ch = 0; ch < 3; ++ch) {
        std::vector<float> histSrc(numBins, 0.0f);
        std::vector<float> histRef(numBins, 0.0f);
        int srcFgCount = 0, refFgCount = 0;

        float scale = (numBins - 1) / 255.0f;

        // 1. Build Source Histogram (Ignore black background)
        for (int i = 0; i < W * H; ++i) {
            if (source[i*3] == 0.0f && source[i*3+1] == 0.0f && source[i*3+2] == 0.0f) continue;
            int bin = std::max(0, std::min(numBins - 1, static_cast<int>(std::round(source[i*3 + ch] * scale))));
            histSrc[bin]++;
            srcFgCount++;
        }

        // 2. Build Reference Histogram (Ignore black background)
        for (int i = 0; i < refW * refH; ++i) {
            if (reference[i*3] == 0.0f && reference[i*3+1] == 0.0f && reference[i*3+2] == 0.0f) continue;
            int bin = std::max(0, std::min(numBins - 1, static_cast<int>(std::round(reference[i*3 + ch] * scale))));
            histRef[bin]++;
            refFgCount++;
        }

        // 3. Compute Normalized CDFs
        std::vector<float> cdfSrc(numBins, 0.0f), cdfRef(numBins, 0.0f);
        cdfSrc[0] = histSrc[0] / srcFgCount;
        cdfRef[0] = histRef[0] / refFgCount;
        for (int i = 1; i < numBins; ++i) {
            cdfSrc[i] = cdfSrc[i - 1] + (histSrc[i] / srcFgCount);
            cdfRef[i] = cdfRef[i - 1] + (histRef[i] / refFgCount);
        }

        // 4. Create Mapping Table
        std::vector<int> mapping(numBins, 0);
        for (int i = 0; i < numBins; ++i) {
            float minDiff = 1e9f;
            int bestMatch = 0;
            for (int j = 0; j < numBins; ++j) {
                float diff = std::abs(cdfSrc[i] - cdfRef[j]);
                if (diff < minDiff) {
                    minDiff = diff;
                    bestMatch = j;
                }
            }
            mapping[i] = bestMatch;
        }

        // 5. Apply Mapping to Source Foreground
        for (int i = 0; i < W * H; ++i) {
            if (source[i*3] == 0.0f && source[i*3+1] == 0.0f && source[i*3+2] == 0.0f) continue;
            int bin = std::max(0, std::min(numBins - 1, static_cast<int>(std::round(source[i*3 + ch] * scale))));
            source[i*3 + ch] = (mapping[bin] / static_cast<float>(numBins - 1)) * 255.0f;
        }
    }

    convertYCbCrtoRGBInPlace(source);
    return source;
}

// ==========================================
// MAIN EXECUTION
// ==========================================

int main() {
    int W, H, ch;

    // ------------------------------------------
    // PART 3(a): Linear Contrast Stretch
    // ------------------------------------------
    // std::vector<float> lehImg = readImageToFloat("./data/leh.png", W, H, ch, 3);
    // if (!lehImg.empty()) {
    //     std::cout << "Processing leh.png for Linear Contrast Stretching...\n";
        
    //     std::vector<float> lehStretched = myLinearContrastStretch(lehImg, W, H);
    //     exportImageToCSV(lehStretched, W, H, 3, "temp/3_a_leh_linear.csv");
        
    //     std::cout << "[Part 3a] Exported Linear Contrast Stretched image.\n";
    // }

    // ------------------------------------------
    // PART 3(b): Histogram Equalization
    // ------------------------------------------
    // if (!lehImg.empty()) {
    //     std::cout << "Processing leh.png for Histogram Equalization...\n";
        
    //     std::vector<float> lehHE = myHistEqualize(lehImg, W, H);
    //     exportImageToCSV(lehHE, W, H, 3, "temp/3_b_leh_he.csv");
        
    //     std::cout << "[Part 3b] Exported Histogram Equalized image.\n";
    // }


    // ------------------------------------------
    // PART 3(c): CLAHE (canyon.png and retina.png)
    // ------------------------------------------
    
    // Process canyon.png
    // {
    //     std::vector<float> canyonImg = readImageToFloat("./data/canyon.png", W, H, ch, 3);
    //     if (!canyonImg.empty()) {
    //         std::cout << "Processing canyon.png for CLAHE...\n";
            
    //         // Base Tuned Version (Tune these!)
    //         int bins = 256;
    //         int winSizeTuned = 65; 
    //         float threshTuned = 0.05f; 
            
    //         std::vector<float> claheTuned = myCLAHE(canyonImg, W, H, bins, winSizeTuned, threshTuned);
    //         exportImageToCSV(claheTuned, W, H, 3, "temp/3_c_canyon_clahe_tuned.csv");
            
    //         std::vector<float> claheLargeWin = myCLAHE(canyonImg, W, H, bins, winSizeTuned * 4, threshTuned);
    //         exportImageToCSV(claheLargeWin, W, H, 3, "temp/3_c_canyon_clahe_large_win.csv");
            
    //         std::vector<float> claheSmallWin = myCLAHE(canyonImg, W, H, bins, std::max(3, winSizeTuned / 4), threshTuned);
    //         exportImageToCSV(claheSmallWin, W, H, 3, "temp/3_c_canyon_clahe_small_win.csv");
            
    //         std::vector<float> claheHalfThresh = myCLAHE(canyonImg, W, H, bins, winSizeTuned, threshTuned / 2.0f);
    //         exportImageToCSV(claheHalfThresh, W, H, 3, "temp/3_c_canyon_clahe_half_thresh.csv");
            
    //         std::cout << "[Part 3c] Exported all CLAHE variations for canyon.png.\n";
    //     }
    // }

    // Process retina.png
    // {
    //     std::vector<float> retinaImg = readImageToFloat("./data/retina.png", W, H, ch, 3);
    //     if (!retinaImg.empty()) {
    //         std::cout << "Processing retina.png for CLAHE...\n";
            
    //         // Base Tuned Version (Tune these specifically for the retina image!)
    //         int bins = 256;
    //         int winSizeTuned = 101; 
    //         float threshTuned = 0.02f; 
            
    //         std::vector<float> claheTuned = myCLAHE(retinaImg, W, H, bins, winSizeTuned, threshTuned);
    //         exportImageToCSV(claheTuned, W, H, 3, "temp/3_c_retina_clahe_tuned.csv");
            
    //         std::vector<float> claheLargeWin = myCLAHE(retinaImg, W, H, bins, winSizeTuned * 4, threshTuned);
    //         exportImageToCSV(claheLargeWin, W, H, 3, "temp/3_c_retina_clahe_large_win.csv");
            
    //         std::vector<float> claheSmallWin = myCLAHE(retinaImg, W, H, bins, std::max(3, winSizeTuned / 4), threshTuned);
    //         exportImageToCSV(claheSmallWin, W, H, 3, "temp/3_c_retina_clahe_small_win.csv");
            
    //         std::vector<float> claheHalfThresh = myCLAHE(retinaImg, W, H, bins, winSizeTuned, threshTuned / 2.0f);
    //         exportImageToCSV(claheHalfThresh, W, H, 3, "temp/3_c_retina_clahe_half_thresh.csv");
            
    //         std::cout << "[Part 3c] Exported all CLAHE variations for retina.png.\n";
    //     }
    // }

    // ------------------------------------------
    // PART 3(d): Histogram Matching
    // ------------------------------------------
    {
        int W, H, ch, refW, refH, refCh;
        std::vector<float> retinaImg = readImageToFloat("./data/retina.png", W, H, ch, 3);
        std::vector<float> refImg = readImageToFloat("./data/retinaRef.png", refW, refH, refCh, 3);
        
        if (!retinaImg.empty() && !refImg.empty()) {
            std::cout << "Processing Histogram Matching...\n";
            
            // Generate standard 256-bin match
            std::vector<float> matched256 = myHistMatch(retinaImg, refImg, W, H, refW, refH, 256);
            exportImageToCSV(matched256, W, H, 3, "temp/3_d_retina_matched_256.csv");

            // Generate extreme bin cases for the report
            std::vector<float> matched16 = myHistMatch(retinaImg, refImg, W, H, refW, refH, 16);
            exportImageToCSV(matched16, W, H, 3, "temp/3_d_retina_matched_16.csv");

            std::cout << "[Part 3d] Exported Histogram Matching variations.\n";
        }
    }
}