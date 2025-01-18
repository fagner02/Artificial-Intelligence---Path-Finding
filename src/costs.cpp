#include <costs.h>


float c3(float t) {
    return 10.0 + fmod(abs(5.0f - t), 6.0f);
}

float c4(float t) {
    return 5.0 + fmod(abs(10.0f - t), 11.0f);
}

// custo 10 para todos os movimentos
float cost_all10(int dir, int step) {
    return 10.0;
}

// custo 10 para movimentos verticais e 15 para movimentos horizontais
float cost_v10h15(int dir, int step) {
    if (dir % 2 == 0) {
        return 10.0;
    } else {
        return 15.0;
    }
}

// custo 10 para movimentos verticais e c3 para movimentos horizontais
float cost_v10hc3(int dir, int step) {
    if (dir % 2 == 0) {
        return 10.0;
    } else {
        return c3(step);
    }
}

// custo 10 para movimentos verticais e c4 para movimentos horizontais
float cost_v10hc4(int dir, int step) {
    if (dir % 2 == 0) {
        return 10.0;
    } else {
        return c4(step);
    }
}
