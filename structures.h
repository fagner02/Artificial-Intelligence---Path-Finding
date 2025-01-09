#pragma once

struct point {
    int x, y;
};

struct visited_info {
    float cost;
    int step;
};

bool operator==(const point& lhs, const point& rhs);