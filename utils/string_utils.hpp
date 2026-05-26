#include <string>

std::string trim(const std::string& str){
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