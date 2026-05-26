#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "../include/parser.hpp"

int main(int argc, char *argv[]) {

    if (argc != 2 && argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <inputfile> " 
                << "<outputfile>[optional]" << std::endl; 
        return 1;
    }

    Parser parser(argv[1]);
    Circuit circuit;
    if(!parser.parse(&circuit)){
        std::cerr << "Failed to parse the file" << std::endl;
        return 1;
    }

    return 0;
}