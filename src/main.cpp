#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <getopt.h>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <string>
#include <cstdio>
#include <ctime>
#include "metaheuristic.h"
#include "tweaker.h"
#include "utils.h"
#include "is.h"

int metaheuristic_optional_arg[10];
int moa_counter = 0;

int tweaker_optional_arg[10];
int toa_counter = 0;

int main(int argc, char *argv[]) {

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"file", required_argument, 0, 'f'},
        {"disp", no_argument, 0, 'd'},
        {"metaheuristic", required_argument, 0, 'g'},
        {"meta-optional-arg", required_argument, 0, 'm'},
        {"tweaker-optional-arg", required_argument, 0, 'o'},
        {"tweaker", required_argument, 0, 'x'},
        {"results", required_argument, 0, 'r'},
        {0, 0, 0, 0}
    };

    // Parser of CLI
    int opt = 0, option_index, runs;
    bool flag_h = false; 
    bool flag_f = false; 
    bool flag_d = false; 
    bool flag_g = false; 
    bool flag_m = false; 
    bool flag_o = false; 
    bool flag_n = false;
    bool flag_x = false;
    bool flag_r = false;
    bool flag_t = false;
    memset(metaheuristic_optional_arg, 0, sizeof(metaheuristic_optional_arg));
    memset(tweaker_optional_arg, 0, sizeof(tweaker_optional_arg));

    // We need these three things
    std::string file_name, metaheuristic_str, tweaker_str;
    std::string out_fn;

    while (1) {
        opt = getopt_long(argc, argv, "dhf:g:m:o:x:n:r:t:", long_options, &option_index);
        if (opt == -1) break;
        opt = char(opt);

        switch (opt) {
            case 0  : break;
            case 'h': flag_h = true; print_help(); break;
            case 'f': flag_f = true; file_name = optarg; break;
            case 'g': flag_g = true; metaheuristic_str = optarg; break; 
            case 'm': metaheuristic_optional_arg[moa_counter++] = atoi(optarg); 
                      break;
            case 'o': tweaker_optional_arg[toa_counter++] = atoi(optarg); 
                      break;
            case 'x': flag_x = true; tweaker_str = optarg; break;
            case 'n': flag_n = true; runs = atoi(optarg); break;
            case 'r': flag_r = true; out_fn = optarg; break;
            case 'd': flag_d = true; break;
            case 't': flag_t = true; file_name = optarg; break;
            case '?': break;
        }
    }

    if (flag_h) return 0; // Print help and exit

    // Checking and exit in case of error
    if (! flag_g and ! flag_d) error_("You have to specify a metaheuristic"); 
    if (  flag_f and   flag_t) error_("You cannot use -f and -t together");
    if (! flag_x and ! flag_d) error_("You have to specify a tweaker");
    if ((! flag_f and ! flag_d) and (! flag_t)) error_("You have to specify a file");  
    if (! flag_n and ! flag_d) runs = 1;

    // TODO: Make this a parameter
    int part = 10;

    Metaheuristic *metaheuristic = choose_metaheuristic(metaheuristic_str, 
                                    metaheuristic_optional_arg, 
                                    moa_counter);
    if (!metaheuristic) {
        std::cerr << "Error: No metaheuristic by that name." << std::endl;
        exit(1);
    }
    Tweaker *tweaker = choose_tweaker(tweaker_str,
                                      tweaker_optional_arg,
                                      toa_counter);
    if (!tweaker) {
        std::cerr << "Error: No tweaker by that name." << std::endl;
        exit(1);
    }

    metaheuristic->setTweaker(tweaker); // Setting tweaker

    if (flag_f) {
        IS::Dataset dataset = load_data_basic(file_name);
        IS::Solution solution(dataset.size());
        //std::cout << dataset.toString() << std::endl; 
        metaheuristic->optimize(dataset, solution);
    } else if (flag_t) {
        vps datasets;
        load_data_tenfold(file_name, datasets);
        run_tenfold(datasets, metaheuristic, runs);
    }

    if (flag_d) {
        IS::Dataset dataset = load_data_basic(file_name);
        print_dispersions(dataset);
        return 0;
    }


   
    return 0;
}
