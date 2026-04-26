#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <cmath>
#include <string>
#include <limits>
#include <ctime>
using namespace std;
#define WIDTH 50
#define HEIGHT 50
#define PPM_SCALER 25
#define SAMPLE_SIZE 500
#define BIAS 10.0f
#define TRAIN_PASSES 80
typedef float Layer[HEIGHT][WIDTH];

static inline int clampi(int x, int low, int high)
{
    if (x < low)
        x = low;
    if (x > high)
        x = high;
    return x;
}
void layer_fill_rect(Layer layer, int x, int y, int w, int h, float value)
{
    assert(w > 0);
    assert(h > 0);
    int x0 = clampi(x, 0, WIDTH - 1);
    int y0 = clampi(y, 0, HEIGHT - 1);
    int x1 = clampi(x0 + w - 1, 0, WIDTH - 1);
    int y1 = clampi(y0 + h - 1, 0, HEIGHT - 1);
    for (int y = y0; y <= y1; ++y)
    {
        for (int x = x0; x <= x1; ++x)
        {
            layer[y][x] = value;
        }
    }
}
void layer_fill_circle(Layer layer, int cx, int cy, int r, float value)
{
    assert(r > 0);
    int x0 = clampi(cx - r, 0, WIDTH - 1);
    int y0 = clampi(cy - r, 0, HEIGHT - 1);
    int x1 = clampi(cx + r, 0, WIDTH - 1);
    int y1 = clampi(cy + r, 0, HEIGHT - 1);
    for (int y = y0; y <= y1; ++y)
    {
        for (int x = x0; x <= x1; ++x)
        {
            int dx = x - cx;
            int dy = y - cy;
            if (dx * dx + dy * dy <= r * r)
            {
                layer[y][x] = value;
            }
        }
    }
}

void layer_save_as_ppm(Layer layer, const filesystem::path &filep)
{

    float min = numeric_limits<float>::max();
    float max = numeric_limits<float>::min();

    for (int y = 0; y < HEIGHT - 1; ++y)
    {
        for (int x = 0; x < WIDTH - 1; ++x)
        {
            if (layer[y][x] < min)
                min = layer[y][x];
            if (layer[y][x] > max)
                max = layer[y][x];
        }
    }
    ofstream f(filep, std::ios::binary);
    if (!f)
    {
        std::cerr << "not open it\n";
        exit(1);
    }

    f << "P6\n"
      << WIDTH * PPM_SCALER << " " << HEIGHT * PPM_SCALER << " 255\n";

    for (int y = 0; y < HEIGHT * PPM_SCALER; y++)
    {
        for (int x = 0; x < WIDTH * PPM_SCALER; ++x)
        {
            float s = (layer[y / PPM_SCALER][x / PPM_SCALER] - min) / (max - min);

            char pixel[3] = {
                static_cast<char>(std::floor(255.0f * (1.0f - s))),
                static_cast<char>(std::floor(255.0f * s)),
                0};
            f.write(pixel, sizeof(pixel));
        }
    }
}

void layer_save_as_bin(Layer layer, const filesystem::path &file)
{
    ofstream f(file, std::ios::binary);
    if (!f)
    {
        std::cerr << "not open it\n";
        exit(1);
    }
    f.write(reinterpret_cast<const char *>(layer), sizeof(Layer));
}
void layer_load_as_bin(Layer layer, const filesystem::path &file)
{
    assert(0 && "TODO: layer_save_as_bin is not implemented");
}
float feed_forward(Layer inputs, Layer weights)
{
    float output = 0.0f;
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            output += inputs[i][j] * weights[i][j];
        }
    }
    return output;
}

void add_inputs_to_weights(Layer inputs, Layer weights)
{
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            weights[i][j] += inputs[i][j];
        }
    }
}

void sub_inputs_from_weights(Layer inputs, Layer weights)
{
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            weights[i][j] -= inputs[i][j];
        }
    }
}
static Layer inputs;
static Layer weights;

int rand_range(int low, int high)
{
    assert(low < high);
    return rand() % (high - low) + low;
}
void layer_random_rect(Layer layer)
{
    layer_fill_rect(layer, 0, 0, WIDTH, HEIGHT, 0.0f);

    int x = rand_range(0, WIDTH);
    int y = rand_range(0, HEIGHT);
    int w = WIDTH - x;
    if (w < 2)
        w = 2;
    w = rand_range(1, w);
    int h = HEIGHT - y;
    if (h < 2)
        h = 2;
    h = rand_range(1, h);

    layer_fill_rect(inputs, x, y, w, h, 1.0f);
}
void layer_random_circle(Layer layer)
{
    layer_fill_rect(layer, 0, 0, WIDTH, HEIGHT, 0.0f);

    int cx = rand_range(0, WIDTH);
    int cy = rand_range(0, HEIGHT);
    // int r = rand_range(1, WIDTH);
    int r = INT_MAX;
    if (r > cx)
        r = cx;
    if (r > cy)
        r = cy;
    if (r > WIDTH - cx)
        r = WIDTH - cx;
    if (r > HEIGHT - cy)
        r = HEIGHT - cy;
    if (r < 2)
        r = 2;
    r = rand_range(1, r);
    layer_fill_circle(inputs, cx, cy, r, 1.0f);
}

int train_pass(Layer inputs, Layer weights)
{
    int adjust = 0;

    for (int i = 0; i < SAMPLE_SIZE; ++i)
    {
        layer_random_rect(inputs);

        if (feed_forward(inputs, weights) > BIAS)
        {
            sub_inputs_from_weights(inputs, weights);
            adjust++;
        }

        layer_random_circle(inputs);
        if (feed_forward(inputs, weights) < BIAS)
        {
            add_inputs_to_weights(inputs, weights);
            adjust++;
        }
    }
    return adjust;
}
int check_pass(Layer inputs, Layer weights)
{
    int adjust = 0;
    for (int i = 0; i < SAMPLE_SIZE; ++i)
    {
        layer_random_rect(inputs);

        if (feed_forward(inputs, weights) > BIAS)
        {

            adjust++;
        }

        layer_random_circle(inputs);
        if (feed_forward(inputs, weights) < BIAS)
        {
            adjust++;
        }
    }
    return adjust;
}
int main()
{
    for (int i = 0; i < TRAIN_PASSES; ++i)
    {
        srand(69);
        int done = train_pass(inputs, weights);
        cout << done << endl;
    }

    layer_save_as_ppm(weights, "weight.ppm");

    for (int i = 0; i < TRAIN_PASSES; ++i)
    {
        srand(420);
        int done = check_pass(inputs, weights);
        cout << done << endl;
    }
    // layer_fill_circle(inputs, WIDTH / 2, HEIGHT / 2, WIDTH / 2, 1.0f);
    // layer_save_as_bin(inputs, "inputs.bin");
    // float output = feed_forward(inputs, weights);
    // printf("output is %f\n", output);
    return 0;
}