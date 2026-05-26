#pragma once
#include <iostream>
#include <string>

// 解析字符串类型的值
double parse_value(std::string str){
    double value = 0.0;
    double multiplier = 1.0;

    if(str.empty()){
        std::cerr << "Wrong input" << std::endl;
        return 1;
    }

    if(str.back() == 'm'){
        multiplier = 0.001;
        str.pop_back();
    }
    else if(str.back() == 'u'){
        multiplier = 0.000001;
        str.pop_back();
    }
    else if(str.back() == 'n'){
        multiplier = 0.000000001;
        str.pop_back();
    }
    else if(str.back() == 'p'){
        multiplier = 0.000000000001;
        str.pop_back();
    }
    else if(str.back() == 'k'){
        multiplier = 1000;
        str.pop_back();
    }
    else if(str.back() == 'M'){
        multiplier = 1000000;
        str.pop_back();
    }
    else if(str.back() == 'G'){
        multiplier = 1000000000;
        str.pop_back();
    }

    for(char c: str){
        if(c < '0' || c > '9'){
            std::cerr << "Wrong input" << std::endl;
            return -1;
        }
        value = value * 10 + double(c-'0');
    }

    return value * multiplier;
}