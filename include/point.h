#pragma once

struct point {
    int x, y;
};

point operator+(const point& lhs, const point& rhs);

point operator-(const point& lhs, const point& rhs);

bool operator==(const point& lhs, const point& rhs);

bool operator!=(const point& lhs, const point& rhs);

bool operator<(const point& lhs, const point& rhs);