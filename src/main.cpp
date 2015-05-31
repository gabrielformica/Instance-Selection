#include <iostream>
#include <getopt.h>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <string>
#include <cstdio>
#include <ctime>
#include "metaheuristic.h"
#include "tweaker.h"
#include "is.h"

typedef IS::Dataset ds;

int metaheuristic_optional_arg[10];
int moa_counter = 0;

int tweaker_optional_arg[10];
int toa_counter = 0;

typedef struct result {
    int errors;
    float size;
    float val_error;
} result;


std::pair<ds, ds> load_data(std::string file_name_str, int partition) {
    const char *file_name = file_name_str.c_str();
    FILE *input = fopen(file_name, "r");
    int insts_size, num_attrs;

    if (input) 
        printf("File: %s\n", file_name);
    else {
        perror("ERROR at data load");
        exit(1);
    }

    if (fscanf(input, "%d %d", &insts_size, &num_attrs) != 2) {
        printf("ERROR at data load: file malformed\n");
        exit(1);
    }
    
    int val_size = insts_size / 10;
    int tra_size = insts_size - val_size;

    // Two datasets are built: One for validation and one for training
    IS::Dataset validation = IS::Dataset();
    IS::Dataset training   = IS::Dataset();
    training.setAttrs(num_attrs);
    validation.setAttrs(num_attrs);

    std::vector<int> val_pos;
    
    srand(time(NULL));
    for(unsigned i = 0; i < val_size; ++i) {
        int j = rand() % tra_size;
        val_pos.push_back(j);
    }
    assert(val_pos.size() == val_size);

    for(unsigned i = 0; i < insts_size; ++i) {
        if(std::find(val_pos.begin(), val_pos.end(), i) != val_pos.end()) {
            /* validation instance */
            double attr;
            IS::Instance val;
            for(unsigned j = 0; j < num_attrs-1; ++j) { 
                fscanf(input, "%lf", &attr);
                val.push_back(attr);
            }
            fscanf(input, "%lf", &attr);
            val.setCategory(attr);
            validation.push_back(val);
        } else {
            /* not a validation instance */
            double attr;
            IS::Instance tra;
            for(unsigned j = 0; j < num_attrs-1; ++j) { 
                fscanf(input, "%lf", &attr);
                tra.push_back(attr);
            }
            fscanf(input, "%lf", &attr);
            tra.setCategory(attr);
            training.push_back(tra);
        }
    }

    return make_pair(training, validation);
}

void print_help() {
    std::cout << "Usage: ./instance_selection [OPTIONS]" << std::endl;
    std::cout << "Run a metaheuristic with an tweaker over a problem.";
    std::cout << std::endl << std::endl;
    std::cout << "Flags with -Needed- are mandatory." << std::endl << std::endl;

    std::cout << "Flags of the form '--FOO-optional-arg'";
    std::cout << ", recieve parameters that has " << std::endl;
    std::cout << "to be used by 'FOO', ";
    std::cout << "and they follow a possitional order that is" << std::endl;
    std::cout << "meaningful to 'FOO'." << std::endl << std::endl;

    std::cout << "-h, --help\t\t\t\tPrint this help" << std::endl;  
    std::cout << "-f, --file\t-Needed-\t\tExpect a file name" << std::endl; 
    std::cout << "-d, --disp\t\t\t\tPrint dispersion vector" << std::endl; 

    // Metaheuristic
    std::cout << "-g, --metaheuristic [NAME]     -Needed-\tSpecify metaheuristic"; 
    std::cout << std::endl;

    std::cout << "-m, --meta-optional-arg [VALUE]\t\tValue for metaheuristic"; 
    std::cout << std::endl;

    // Tweaker
    std::cout << "-x, --tweaker \t-Needed-\t\tSpecify tweaker"; 
    std::cout << std::endl;

    std::cout << "-o, --tweaker-optional-arg [VALUE]\tValue for tweaker"; 
    std::cout << std::endl << std::endl;

    // Example
    std::cout << "Example" << std::endl;
    std::cout << "   ./instance_selection -g \"Tabu\" -m 100 -m 30 ";
    std::cout << "-x \"nRandomFlips\" -o 15" << std::endl << std::endl;
    std::cout << "   Wich will use Tabu with a tabu list of 100 elements and";
    std::cout << std::endl <<  "   number of tweaks equals to 30" << std::endl;

}

void error_(std::string msg) {

    std::cout << "ERROR: " <<  msg << std::endl;
    exit(1);
}

void print_solution(const IS::Dataset &problem, const IS::Solution &sol) {
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

void print_dispersions(const IS::Dataset &dataset) {
    std::vector<double> dispersion;
    dataset.getDispersions(dispersion);

    std::sort(dispersion.begin(), dispersion.end());

    std::cout << ">>>>>> Dispersions: " << std::endl;
    for (int i = 0; i < dispersion.size(); i++)
        if(i % 10 == 0 and i != 0)  {
            std::cout << dispersion[i] << " " << std::endl;
        } else {
            std::cout << dispersion[i] << " ";
        }
    std::cout << endl;
}

Metaheuristic *choose_metaheuristic(std::string metaheuristic_str) {
    Metaheuristic *metaheuristic;
    if (metaheuristic_str == "HillClimbing") {
        return new HillClimbing();
    } else if (metaheuristic_str == "SimulatedAnnealing") {
        // Print error and exit
        if (moa_counter != 2) error_("SimulatedAnnealing needs two arguments");
        int t = metaheuristic_optional_arg[0]; 
        int d = metaheuristic_optional_arg[1];
        return new SimulatedAnnealing(t, d);
    } else if (metaheuristic_str == "Tabu") {
        // Print error and exit
        if (moa_counter != 2) error_("Tabu needs two arguments");
        int l = metaheuristic_optional_arg[0]; 
        int n = metaheuristic_optional_arg[1];
        return new Tabu(l, n);
    } else if (metaheuristic_str == "ILS") {
        
        // Print error and exit
        if (moa_counter != 3) error_("ILS needs three arguments");
        int l = metaheuristic_optional_arg[0]; 
        int t = metaheuristic_optional_arg[1];
        int p = metaheuristic_optional_arg[2];
        return new ILS(l, t, p);
    }
    error_("The metaheuristic you are providing doesn't exist");
    return NULL;   // Shut up warning
}

Tweaker *choose_tweaker(std::string tweaker_str) {
    Tweaker *tweaker;
    if (tweaker_str == "oneRandomUnset") return new oneRandomUnset();
    else if (tweaker_str == "upToNRandomFlips") {
        // Print error and exit
        if (toa_counter != 1) error_("upToNRandomFlips needs one argument");
        return new upToNRandomFlips(tweaker_optional_arg[0]);
    } else if (tweaker_str == "nRandomFlips") {
        // Print error and exit
        if (toa_counter != 1) error_("nRandomFlips needs one argument");
        return new nRandomFlips(tweaker_optional_arg[0]);
    } else if (tweaker_str == "percRandomFlips") {
        // Print error and exit
        if (toa_counter != 1) error_("percRandomFlips needs one argument");
        return new percRandomFlips(tweaker_optional_arg[0]);
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"file", required_argument, 0, 'f'},
        {"disp", no_argument, 0, 'd'},
        {"metaheuristic", required_argument, 0, 'g'},
        {"meta-optional-arg", required_argument, 0, 'm'},
        {"tweaker-optional-arg", required_argument, 0, 'o'},
        {"tweaker", required_argument, 0, 'x'},
        {0, 0, 0, 0}
    };

    // Parser of CLI
    int opt, option_index, runs;
    bool flag_h, flag_f, flag_d, flag_g, flag_m, flag_o, flag_n;
    bool flag_x;
    memset(metaheuristic_optional_arg, 0, sizeof(metaheuristic_optional_arg));
    memset(tweaker_optional_arg, 0, sizeof(tweaker_optional_arg));

    // We need these three things
    std::string file_name, metaheuristic_str, tweaker_str;

    while (1) {
        opt = getopt_long(argc, argv, "hf:dg:m:o:x:n:", long_options, &option_index);
        if (opt == -1) break;

        switch (opt) {
            case 0: break;
            case 'h': flag_h = true; print_help(); break;
            case 'f': flag_f = true; file_name = optarg; break;
            case 'd': flag_d = true; break;
            case 'g': flag_g = true; metaheuristic_str = optarg; break; 
            case 'm': metaheuristic_optional_arg[moa_counter++] = atoi(optarg); 
                      break;
            case 'o': tweaker_optional_arg[toa_counter++] = atoi(optarg); 
                      break;
            case 'x': flag_x = true; tweaker_str = optarg; break;
            case 'n': flag_n = true; runs = atoi(optarg); break;
        }
    }

    if (flag_h) return 0; // Print help and exit

    // Checking and exit in case of error
    if (! flag_f and ! flag_d) error_("You have to specify a file");  
    if (! flag_g and ! flag_d) error_("You have to specify a metaheuristic"); 
    if (! flag_x and ! flag_d) error_("You have to specify a tweaker");
    if (! flag_n and ! flag_d) {
        runs = 1;
    }


    // TODO: Make this a parameter
    int part = 10;

    std::pair<ds, ds> dataset = load_data(file_name, part);
    IS::Dataset training   = dataset.first;
    IS::Dataset validation = dataset.second;

    if (flag_d) {
        print_dispersions(training);
        return 0;
    }

    Metaheuristic *metaheuristic = choose_metaheuristic(metaheuristic_str);
    Tweaker *tweaker = choose_tweaker(tweaker_str);

    metaheuristic->setTweaker(tweaker);
    vector<result> results;

    for(unsigned i = 0; i < runs; ++i) {
        /* code */
        IS::Solution solution(training.size());
        metaheuristic->optimize(training, solution);

        // Checking quality of final solution
        std::vector<double> categories;
        categories.assign(validation.size(), -1);

        std::bitset<MAX> bits = solution.getBits();
        IS::Dataset final;
        for(unsigned i = 0; i < training.size(); ++i) {
            if(bits[i]) final.push_back(training[i]);
        }

        metaheuristic->oneNN(final, validation, categories);

        int errors = 0;
        for (int i = 0; i < validation.size(); i++)
            if (validation[i].getCategory() != categories[i])
                errors++;

        double clas_rate = (1.0 * errors) / (1.0 * validation.size());
        double perc_redc = (1.0 * final.size()) / (1.0 * training.size());
        double fitness = 0.5 * clas_rate + (1 - 0.5) * perc_redc;

        // XXX: Print
        result r;
        r.errors = errors;
        r.val_error = (errors * 100.0) / (validation.size() * 1.0);
        r.size = (final.size() * 100.0) / (training.size() * 1.0);
        results.push_back(r);

    }

    for(unsigned i = 0; i < results.size(); ++i) {
        result r = results[i];
        cout << "Total errors: " << r.errors << endl;
        cout << "Validation error %: " << r.val_error << endl;
        cout << "Size %: " << r.size << endl;
    }

    return 0;
}
