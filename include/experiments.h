#include <fstream>
#include <iostream>
#include <structures.h>
#include <path_finding.h>

#pragma once

void experiment1(std::string output_file);
void experiment2(std::string output_file);
void experiment3(std::string output_file);
void experiment4(std::string output_file);
void experiment5(std::string output_file);

void experiment1(
    std::vector<point> start_points,
    std::vector<point> target_points,
    std::function<void(std::wstring)> toastText,
    std::string output_file
);
void experiment2(
    std::vector<point> start_points,
    std::vector<point> target_points,
    std::function<void(std::wstring)> toastText,
    std::string output_file
);
void experiment3(
    std::vector<point> start_points,
    std::vector<point> target_points,
    std::function<void(std::wstring)> toastText,
    std::string output_file
);
void experiment4(
    std::vector<point> start_points,
    std::vector<point> target_points,
    std::vector<std::vector<int>> orders,
    std::function<void(std::wstring)> toastText,
    std::string output_file
);
void experiment5(
    std::vector<point> start_points,
    std::vector<point> target_points,
    std::vector<std::vector<int>> orders,
    std::vector<std::set<point>> constraints,
    std::function<void(std::wstring)> toastText,
    std::string output_file
);
