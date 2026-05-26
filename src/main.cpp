#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {

    if (argc != 2 && argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <inputfile> " 
                << "<outputfile>[optional]" << std::endl; 
        return 1;
    }

    Parser parser(argv[1]);
    parser.parse();

    return 0;
}