#include <algorithm>
#include <random>
#include <numeric>

class PerlinNoise {
public:
    PerlinNoise(unsigned int seed = 0) {
        // Initialize permutation table
        std::iota(std::begin(p), std::end(p), 0);
        std::shuffle(std::begin(p), std::end(p), std::default_random_engine{ seed });
        for (int i = 0; i < 256; ++i) {
            p[i + 256] = p[i];
        }
    }

    float noise(float x, float y) {
        int xi = (int)x & 255;
        int yi = (int)y & 255;
        int aa = p[p[xi] + yi];
        int ab = p[p[xi + 1] + yi];
        int ba = p[p[xi] + yi + 1];
        int bb = p[p[xi + 1] + yi + 1];
        float xf = x - (int)x;
        float yf = y - (int)y;
        float u = fade(xf);
        float v = fade(yf);
        float aaa = grad(aa, xf, yf);
        float aba = grad(ab, xf - 1.0f, yf);
        float baa = grad(ba, xf, yf - 1.0f);
        float bba = grad(bb, xf - 1.0f, yf - 1.0f);
        float x1 = lerp(u, aaa, aba);
        float x2 = lerp(u, baa, bba);
        return lerp(v, x1, x2);
    }

private:
    int p[512];

    float fade(float t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    float lerp(float t, float a, float b) {
        return a + t * (b - a);
    }

    float grad(int hash, float x, float y) {
        int h = hash & 7;
        float u = h < 4 ? x : y;
        float v = h < 4 ? y : x;
        return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
    }
};