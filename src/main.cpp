#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "../include/circuit.hpp"
#include "../include/parser.hpp"
#include "../include/dc_solver.hpp"

int main(int argc, char *argv[]) {

    if (argc != 2 && argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <inputfile> " 
                << "<outputfile>[optional]" << std::endl; 
        return 1;
    }

    Parser parser(argv[1]);
    Circuit circuit;
    if(!parser.parse(circuit)){
        std::cerr << "Failed to parse the file" << std::endl;
        return 1;
    }

    std::ofstream ofs; 
    std::ostream* os = &std::cout;

    if(argc == 3){
        ofs.open(argv[2], std::ios::out | std::ios::trunc);
        if(!ofs){
            std::cerr << "Fail to open/create file <" << argv[2] << ">" << std::endl;
            return 1;
        }
        os = &ofs;
    }

    circuit.print(*os);

    DcSolver solver;
    if(!solver.solve(circuit, *os)){
        std::cerr << "DcAnalysis Failed!" << std::endl;
        return 1; 
    };
    
    return 0;
}