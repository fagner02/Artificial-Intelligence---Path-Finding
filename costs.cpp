#include <math.h>
#include <bits/stdc++.h>

typedef std::function<float(int, int)> cost_fn;

float c3(float t) {
    return 10.0 + fmod(abs(5.0f - t), 6.0f);
}

float c4(float t) {
    return 5.0 + fmod(abs(10.0f - t), 11.0f);
}

float cost_all10(int dir, int step) {
    return 10.0;
}

float cost_v10h15(int dir, int step) {
    if (dir % 2 == 0) {
        return 10.0;
    } else {
        return 15.0;
    }
}

float cost_v10hc3(int dir, int step) {
    if (dir % 2 == 0) {
        return 10.0;
    } else {
        return c3(step);
    }
}

float cost_v10hc4(int dir, int step) {
    if (dir % 2 == 0) {
        return 10.0;
    } else {
        return c4(step);
    }
}

const cost_fn costs[] = {
    cost_all10,
    cost_v10h15,
    cost_v10hc3,
    cost_v10hc4
};