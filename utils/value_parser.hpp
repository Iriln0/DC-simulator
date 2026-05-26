#pragma once
#include <cctype>
#include <cmath>
#include <iostream>
#include <string>

#include "string_utils.hpp"

// 解析字符串类型的值，失败返回 -1
inline double parse_value(std::string str) {
    if (str.empty()) {
        std::cerr << "Wrong input" << std::endl;
        return -1;
    }

    double multiplier = 1.0;

    if (str.size() >= 3) {
        const std::string suffix3 = str.substr(str.size() - 3);
        if (equals_ignore_case(suffix3, "meg")) {
            multiplier = 1e6;
            str.resize(str.size() - 3);
        }
    }

    if (!str.empty()) {
        const char last = str.back();
        if (last == 'f') {
            multiplier = 1e-15;
            str.pop_back();
        } else if (last == 'p') {
            multiplier = 1e-12;
            str.pop_back();
        } else if (last == 'n') {
            multiplier = 1e-9;
            str.pop_back();
        } else if (last == 'u') {
            multiplier = 1e-6;
            str.pop_back();
        } else if (last == 'm') {
            multiplier = 1e-3;
            str.pop_back();
        } else if (last == 'k' || last == 'K') {
            multiplier = 1e3;
            str.pop_back();
        } else if (last == 'M') {
            multiplier = 1e6;
            str.pop_back();
        } else if (last == 'G') {
            multiplier = 1e9;
            str.pop_back();
        } else if (last == 'T' || last == 't') {
            multiplier = 1e12;
            str.pop_back();
        } else if (!std::isdigit(static_cast<unsigned char>(last)) && last != '.') {
            std::cerr << "Wrong input" << std::endl;
            return -1;
        }
    }

    if (str.empty()) {
        std::cerr << "Wrong input" << std::endl;
        return -1;
    }

    try {
        size_t pos = 0;
        const double value = std::stod(str, &pos);
        if (pos != str.size() || !std::isfinite(value)) {
            std::cerr << "Wrong input" << std::endl;
            return -1;
        }
        const double result = value * multiplier;
        return result;
    } catch (const std::exception&) {
        std::cerr << "Wrong input" << std::endl;
        return -1;
    }
}
