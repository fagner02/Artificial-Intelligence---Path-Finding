#include <fstream>
#include <iostream>
#include <structures.h>
#include <path_finding.h>

#pragma once

void experiment1();
void experiment2();
void experiment3();
void experiment4();
void experiment5();

void experiment1(
    std::vector<point> start_points,
    std::vector<point> target_points,
    std::function<void(std::string)> toastText
);
void experiment2(
    std::vector<point> start_points,
    std::vector<point> target_points,
    std::function<void(std::string)> toastText
);
void experiment3(
    std::vector<point> start_points,
    std::vector<point> target_points,
    std::function<void(std::string)> toastText
);
void experiment4(
    std::vector<point> start_points,
    std::vector<point> target_points,
    std::vector<std::vector<int>> orders,
    std::function<void(std::string)> toastText
);
void experiment5(
    std::vector<point> start_points,
    std::vector<point> target_points,
    std::vector<std::vector<int>> orders,
    std::vector<std::set<point>> constraints,
    std::function<void(std::string)> toastText
);
