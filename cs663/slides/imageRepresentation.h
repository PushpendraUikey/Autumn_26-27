#include <vector>
#inlcude <cmath>
#include <algorithm>

struct Image {
    int rows, cols, channels;
    std::vector<unsigned char> data;
    unsigned char& at(int r, int c, int ch=0) {
        return data[(r * cols + c) * channels + ch];
    }
};

// bilinear interpolation function
float bilinear(const Image& src, float x, float y) {
    int x0 = (int)std::floor(x), y0 = (int)std::floor(y);
    int x1 = std::min(x0 + 1, src.cols - 1), 
        y1 = std::min(y0 + 1, src.rows - 1);

    float a = x - x0,
          b = y - y0;

    float top = (1 - a) * src.at(y0, x0) + a * src.at(y0, x1);
    float bottom = (1 - a) * src.at(y1, x0) + a * src.at(y1, x1);
    return (1 - b) * top + b * bottom;
}

// histogram equalizer function
std::vector<unsigned char> histogramEqualize(const Image& src) {
    int N = src.rows * src.cols;
    std::vector<int> histogram(256, 0);
    for (unsigned char p : src.data) histogram[p]++;

    std::vector<unsigned char> lut(256); // lookup table
    int cumulative = 0;
    for (int k = 0; k < 256; ++k) {
        cumulative += histogram[k];
        lut[k] = (unsigned char)(255.0 * cumulative / N);
    }
    return lut; // apply: dst[i] = lut[src.data[i]];
    // We're looking up the new adjusted brightness value in the lookup table of original intensity
}

Image convolve(const Image& src, const std::vector<std::vector<float>>& kernel) {
    int kh = kernel.size(), kw = kernel[0].size();
    int cy = kh/2, 
        cx = kw/2;

    Image out(src.rows, src.cols);

    for( int y=0; y < src.rows; y++) {
        for( int x=0; x < src.cols; x++) {
            float sum = 0.f;
            for(int j=0; j<kh; j++) {
                for(int i=0; i<kw; i++) {
                    int sy = clamp(y + (j-cy), 0, src.rows - 1); // border handling
                    int sx = clamp(x + (i-cx), 0, src.cols - 1);
                    sum += src.at(sy, sx) * kernel[j][i];
                }
            }
            out.at(y, x) = clampByte(sum);
        }
    }
    return out;
}