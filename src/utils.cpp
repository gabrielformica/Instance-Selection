/**
  * @file
  * @author Gabriel Formica <gabrielformica93@gmail.com>
  * @author Melecio Ponte <pontezambrano@gmail.com>
  *
  * @section Description
  *
  * Utils classes
  */

#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>
#include "metaheuristic.h"
#include "populationbased.h"
#include "utils.h"

IS::Dataset load_data_basic(std::string file_name_str) {
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
    
    IS::Dataset dataset(insts_size, num_attrs);
    double x;
    for (int i = 0; i < insts_size; i++) {
        IS::Instance ins;
        for (int j = 0; j < num_attrs -1; j++) {
            fscanf(input, "%lf", &x);
            ins.push_back(x);
        }
        fscanf(input, "%lf", &x); // reading class
        ins.setCategory(x);
        dataset[i] = ins;
    }
    fclose(input);
    return dataset;
}

void load_data_tenfold(std::string path, vps &datasets) {
    if (path[path.size()-1] == '/') path.erase(path.end()-1);
    size_t basename_pos = path.rfind("/");
    string basename = path.substr(basename_pos+1, path.size());

    for(int i = 1; i <= 10; ++i) {
        std::string tra_filename = path + "/" + basename + "-10-" + to_string(i) + "tra.dat";
        std::string val_filename = path + "/" + basename + "-10-" + to_string(i) + "tst.dat";
        const char *tra_filename_str = tra_filename.c_str();
        const char *val_filename_str = val_filename.c_str();

        IS::Dataset training   = load_dataset(tra_filename_str);
        IS::Dataset validation = load_dataset(val_filename_str);
        datasets.push_back(make_pair(training, validation));
    }
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
    std::cout << "-a, --debugging\t\t\t\tPrint for debugging" << std::endl;  

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

    // Population
    std::cout << "-P, --population-based [VALUE]\tSpecify population based algorithm"; 
    std::cout << std::endl << std::endl;

    std::cout << "-p [VALUE]\tValue for population based"; 
    std::cout << std::endl << std::endl;

    // Example
    std::cout << "Running an simple example: " << std::endl;
    std::cout << "   ./instance_selection -g \"Tabu\" -m 30 -m 10 -m 1000 -m 100";
    std::cout << " -x \"weightedRandomPlus\" -o 10 -o 60 -f file_path";
    std::cout << std::endl << std::endl;
    std::cout << "Wich will use Tabu with a tabu list of 30 elements, ";
    std::cout << std::endl <<  "10  number of tweaks equals to 30"; 
    std::cout << std::endl <<  "1000 as a limit of global iterations and 100 ";
    std::cout << std::endl <<  "as a limit of iterations without change"; 
    std::cout << std::endl << std::endl;
    std::cout << "Running a 10Fold validation:" << std::endl;
    std::cout << "   ./instance_selection -g \"Tabu\" -m 30 -m 10 -m 1000 -m 100";
    std::cout << " -x \"weightedRandomPlus\" -o 10 -o 60 -t file_DIR -n 5"; 
    std::cout << std::endl << std::endl;
    std::cout << "Same as above, but you have to specify the dir. -n 5 runs 5 times every validation";
    std::cout << std::endl;

}

void error_(std::string msg) {
    const char *error_msg = ("ERROR: " + msg).c_str();
    perror(error_msg);
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

Metaheuristic *choose_population(std::string population_str, 
                                   Metaheuristic *local_search,
                                   int (&population_optional_arg)[10],
                                   int poa_counter) {

   PopulationBased *pb; 
   if (population_str == "Hybrid") {
      if (poa_counter != 1) error_("Hybrid needs Population size as argument");
      return new Hybrid(population_optional_arg[0], local_search);
   } else if (population_str == "SGA") {
      if (poa_counter != 1) error_("SGA needs Population size as argument");
      return new SGA(population_optional_arg[0], local_search);
   }
   error_("The Population based algorithm you are providing doesn't exist");
   return NULL;   // Shut up warning
}

Metaheuristic *choose_metaheuristic(std::string metaheuristic_str, 
                                    int (&metaheuristic_optional_arg)[10],
                                    int moa_counter) {
    Metaheuristic *metaheuristic;
    if (metaheuristic_str == "HillClimbing") {
        if (moa_counter != 2) error_("HillClimbing needs two arguments");
        int i = metaheuristic_optional_arg[0];
        int nc = metaheuristic_optional_arg[1];
        return new HillClimbing(i, nc);
    } else if (metaheuristic_str == "Tabu") {
        // Print error and exit
        if (moa_counter != 4) error_("Tabu needs four arguments");
        int l = metaheuristic_optional_arg[0]; 
        int n = metaheuristic_optional_arg[1];
        int i = metaheuristic_optional_arg[2];
        int nc = metaheuristic_optional_arg[3];
        return new Tabu(l, n, i, nc);
    } else if (metaheuristic_str == "ILS") {
        // Print error and exit
        if (moa_counter != 4) error_("ILS needs four arguments");
        int l = metaheuristic_optional_arg[0]; 
        int t = metaheuristic_optional_arg[1];
        int i = metaheuristic_optional_arg[2];
        int nc = metaheuristic_optional_arg[3];
        return new ILS(l, t, i, nc);
    }
    error_("The metaheuristic you are providing doesn't exist");
    return NULL;   // Shut up warning
}

Tweaker *choose_tweaker(std::string tweaker_str,
                        int (&tweaker_optional_arg)[10],
                        int toa_counter) {
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
    } else if (tweaker_str == "weightedRandom") {
        // Print error and exit
        if (toa_counter != 2) error_("weightedRandom needs two argument");
        return new weightedRandom(tweaker_optional_arg[0], tweaker_optional_arg[1]);
    } else if (tweaker_str == "weightedRandomPlus") {
        // Print error and exit
        if (toa_counter != 2) error_("weightedRandomPlus needs two argument");
        return new weightedRandomPlus(tweaker_optional_arg[0], tweaker_optional_arg[1]);
    }

    return NULL;
}


results run_tenfold(const vps &datasets, const Metaheuristic *metaheuristic, int runs) {
    results res;
    IS::Dataset training, validation;

    cout << "Datasets: " << datasets.size() << endl;
    cout << "Runs: " << runs << endl;

    for(int i = 0; i < 10; ++i) {
        cout << BOLDGREEN << "Running fold" << RESET << endl;
        training = datasets[i].first;
        validation = datasets[i].second;

        for(int j = 0; j < runs; ++j) {
            clock_t start = clock();
            double duration;
            IS::Solution solution(training.size());
            metaheuristic->optimize(training, solution);

            // Checking quality of final solution
            std::vector<double> categories;
            categories.assign(validation.size(), -1);

            std::bitset<MAX> bits = solution.getBits();
            IS::Dataset final;
            for(int k = 0; k < training.size(); ++k) {
                if(bits[k]) final.push_back(training[k]);
            }

            duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

            metaheuristic->oneNN(final, validation, categories);

            int errors = 0;
            for (int k = 0; k < validation.size(); k++)
                if (validation[k].getCategory() != categories[k])
                    errors++;

            // double clas_rate = (1.0 * errors) / (1.0 * validation.size());
            // double perc_redc = (1.0 * final.size()) / (1.0 * training.size());
            // double fitness = 0.5 * clas_rate + (1 - 0.5) * perc_redc;

            res.errors.push_back(errors);
            res.val_errors.push_back((errors * 100.0) / (validation.size() * 1.0));
            res.sizes.push_back((final.size() * 100.0) / (training.size() * 1.0));
            res.times.push_back(duration);
        }
    }

    return res;
}

IS::Dataset load_dataset(const char *filename) {
    int insts, attrs;
    double attr;

    FILE *file = fopen(filename, "r");
    if (!file) error_("ERROR at dataload");

    if (fscanf(file, "%d %d", &insts, &attrs) != 2)
        error_("ERROR at data load: file malformed\n");

    IS::Dataset dataset(insts, attrs);

    for(int i = 0; i < insts; ++i) {
        IS::Instance instance;
        for(int j = 0; j < attrs-1; ++j) {
            fscanf(file, "%lf", &attr);
            instance.push_back(attr);
        }
        fscanf(file, "%lf", &attr);
        instance.setCategory(attr);
        dataset[i] = instance;
    }

    return dataset;
}

void output_results(const results &res, 
                    const Metaheuristic *metaheuristic,
                    bool flag_r,
                    std::string out_fn) {

    std::ostream *out;
    std::ofstream out_file;
    
    if(flag_r) {
        out_fn += "_" + metaheuristic->output_params() + ".result";
        out_file.open(out_fn, std::ios::out);
        out = &out_file;
    } else {
       out = &std::cout;
    }

    *out << "Val_errors" << std::endl;
    for (unsigned i = 0; i < res.val_errors.size(); ++i)  {
       *out << res.val_errors[i] << " ";
    }
    *out << std::endl;
    *out << "Sizes" << std::endl;
    for (unsigned i = 0; i < res.sizes.size(); ++i)  {
       *out << res.sizes[i] << " ";
    }
    *out << std::endl;
    *out << "Errors" << std::endl;
    for (unsigned i = 0; i < res.errors.size(); ++i)  {
       *out << res.errors[i] << " ";
    }
    *out << std::endl;
    *out << "Times" << std::endl;
    for (unsigned i = 0; i < res.times.size(); ++i)  {
       *out << res.times[i] << " ";
    }
    *out << std::endl;

}

void debug_print(const IS::Dataset &dataset, const IS::Solution &solution) {
   HillClimbing meta = HillClimbing(0,0);
   std::cout << "----------------------------------------- " << std::endl;
   meta.quality(dataset, solution, 0.5);
   std::cout << "----------------------------------------- " << std::endl;
   std::cout << "Imprimiendo en archivos respectivos" << std::endl;
   assert(dataset.size() == solution.getSize());
   std::string sol_name = "scripts/solution.txt", comp_name = "scripts/complement.txt";

   std::ofstream sol_out, comp_out; 

   // Opening files
   sol_out.open(sol_name, std::ios::out);
   comp_out.open(comp_name, std::ios::out);

   // Printing to files
   std::bitset<MAX> bits = solution.getBits();
   for (int i = 0; i < dataset.size(); i++) {
      if (bits.test(i)) sol_out << dataset[i].toString() << std::endl;
      else comp_out << dataset[i].toString() << std::endl;
   }
   std::cout << "Se imprimieron " << to_string(bits.count()) << " lineas en ";
   std::cout << "solution.txt" << std::endl;

   std::cout << "Se imprimieron " << to_string(dataset.size() - bits.count()); 
   std::cout << " lineas en complement.txt" << std::endl;;
   sol_out.close();
   comp_out.close();
}
