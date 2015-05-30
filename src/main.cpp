#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <getopt.h>
#include "is.h"
#include "metaheuristic.h"

IS::Problem load_data(char *file_name) {
    FILE *input = fopen(file_name, "r");
    int instances, attrs;

    if (input) 
        printf("File: %s\n", file_name);
    else {
        perror("ERROR at data load");
        exit(1);
    }

    if (fscanf(input, "%d %d", &instances, &attrs) != 2) {
        printf("ERROR at data load: file malformed\n");
        exit(1);
    }
    
    IS::Problem problem = IS::Problem(instances);
    problem.setAttrs(attrs);

    for(unsigned i = 0; i < instances; ++i) {
        // attrs-1 because last element of line is category
        double attr;
        for(unsigned j = 0; j < attrs-1; ++j) { 
            fscanf(input, "%lf", &attr);
            problem[i].push_back(attr);
        }
        fscanf(input, "%lf", &attr);
        problem[i].setCategory(attr);
    }

    return problem;
}

void print_help() {
    std::cout << "-h, --help\t\t\tPrint this help" << std::endl;
    std::cout << "-f, --file\t\t\tExpect a file name" << std::endl;
    std::cout << "-d, --disp\t\t\tPrint dispersion vector" << std::endl;

    std::cout << "-c, --hill-climing\t\tRun hill climbing metaheuristic"; 
    std::cout << std::endl;

    std::cout << "-s, --simulated-annealing\tRun simulated annealing metaheuristic"; 
    std::cout << std::endl;

    std::cout << "-t, --tabu\t\t\tRun tabu metaheuristic"; 
    std::cout << std::endl;
}

void print_solution(const IS::Problem &problem, const IS::Solution &sol) {
    int count = 0;
    std::bitset<MAX> bits = sol.getBits();
    for (unsigned i = 0; i < sol.getSize(); ++i) {
        if(bits.test(i)) {
            count++;
            for (unsigned j = 0; j < problem.getAttrs() - 1; ++j) {
                std::cout << problem[i][j] << " ";
            }
            std::cout << problem[i].getCategory() << std::endl;
        }
    }
    std::cout << "Solution of size: " << count << std::endl;
}

void print_dispersions(const IS::Problem &problem) {
    std::vector<double> dispersion;
    problem.getDispersions(dispersion);
    std::cout << ">>>>>> Dispersions: " << std::endl;
    for (int i = 0; i < dispersion.size(); i++)
        std::cout << dispersion[i];
    std::cout << endl;
}

void hill_climbing(const IS::Problem &problem) {
    std::cout << ">>>> Running hill climbing" << std::endl << std::endl;
    IS::Solution sol(problem.size());
    HillClimbing hc;
    hc.optimize(problem, sol);
    print_solution(problem, sol);
}

void simulated_annealing(const IS::Problem &problem) {
    std::cout << ">>>> Running simulated annealing" << std::endl << std::endl;
    IS::Solution sol(problem.size());
    SimulatedAnnealing sa(10000, 10);
    sa.optimize(problem, sol);
    print_solution(problem, sol);
}


void tabu() {

}

void missing_file() {
    std::cout << "You need to specify a file path" << std::endl;
    exit(1);
}

int main(int argc, char *argv[]) {

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"file", required_argument, 0, 'f'},
        {"disp", no_argument, 0, 'd'},
        {"hill-climbing", no_argument, 0, 'c'},
        {"simulated-annealing", no_argument, 0, 's'},
        {"tabu", no_argument, 0, 't'},
        {0, 0, 0, 0}
    };

    // Parser of CLI
    int opt, option_index;
    bool flag_dispersions, flag_help, flag_d, flag_f, flag_c, flag_s, flag_t;
    char file_name[100];
    while (1) {
        opt = getopt_long(argc, argv, "hf:dcst", long_options, &option_index);
        if (opt == -1) break;

        switch (opt) {
            case 0: break;
            case 'h': flag_help = true; print_help(); break;
            case 'f': flag_f = true; strcpy(file_name, optarg); break;
            case 'd': flag_d = true; break;
            case 'c': flag_c = true; break;
            case 's': flag_s = true; break;
            case 't': flag_t = true; break;
        }
    }
    if (flag_help) return 0; // Print help and exit

    if (! flag_f) missing_file();  // Print error and exit

    IS::Problem problem = load_data(file_name);
    if (flag_d) print_dispersions(problem);
    if (flag_c) hill_climbing(problem);
    if (flag_s) simulated_annealing(problem);

    return 0;
}
