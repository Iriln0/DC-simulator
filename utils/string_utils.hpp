#pragma once
#include <string>

inline std::string trim(const std::string& str){
    auto i = str.find_first_not_of(" \t\r\n");
    if(i == std::string::npos){
        return "";
    }
    
    auto j = str.find_last_not_of(" \t\r\n");
    if(j == std::string::npos){
        return "";
    }

    return str.substr(i, j-i+1);
}

inline bool equals_ignore_case(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}