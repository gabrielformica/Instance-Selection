#include <algorithm>
#include <iostream>
#include <cstdio>
#include "metaheuristic.h"
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
        int insts_size, num_attrs;
        double attr;
        std::string tra_filename = path + "/" + basename + "-10-" + to_string(i) + "tra.dat";
        std::string val_filename = path + "/" + basename + "-10-" + to_string(i) + "tst.dat";

        cout << tra_filename << endl;
        cout << val_filename << endl;

        const char *tra_filename_str = tra_filename.c_str();
        const char *val_filename_str = val_filename.c_str();

        FILE *tra_file = fopen(tra_filename_str, "r");
        FILE *val_file = fopen(val_filename_str, "r");

        if (tra_file) printf("File: %s\n", tra_filename_str);
        else {
            perror("ERROR at data load");
            exit(1);
        }

        if (val_file) printf("File: %s\n", val_filename_str);
        else {
            perror("ERROR at data load");
            exit(1);
        }

        if (fscanf(tra_file, "%d %d", &insts_size, &num_attrs) != 2) {
            printf("ERROR at data load: file malformed\n");
            exit(1);
        }

        IS::Dataset training   = IS::Dataset();
        
        for(int j = 0; j < insts_size; ++j) {
            IS::Instance tra;
            for(int k = 0; k < num_attrs-1; ++k) { 
                fscanf(tra_file, "%lf", &attr);
                tra.push_back(attr);
            }
            fscanf(tra_file, "%lf", &attr);
            tra.setCategory(attr);
            training.push_back(tra);
        }

        IS::Dataset validation = IS::Dataset();

        if (fscanf(val_file, "%d %d", &insts_size, &num_attrs) != 2) {
            printf("ERROR at data load: file malformed\n");
            exit(1);
        }

        for(int j = 0; j < insts_size; ++j) {
            IS::Instance val;
            for(int k = 0; k < num_attrs-1; ++k) { 
                fscanf(tra_file, "%lf", &attr);
                val.push_back(attr);
            }
            fscanf(tra_file, "%lf", &attr);
            val.setCategory(attr);
            validation.push_back(val);
        }

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

Metaheuristic *choose_metaheuristic(std::string metaheuristic_str, 
                                    int (&metaheuristic_optional_arg)[10],
                                    int moa_counter) {
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


void run_tenfold(const vps &datasets, const Metaheuristic *metaheuristic, int runs) {
    results res;
    IS::Dataset training, validation;

    for(int i = 0; i < 10; ++i) {
        training = datasets[i].first;
        validation = datasets[i].second;

        for(int j = 0; j < runs; ++j) {
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

            metaheuristic->oneNN(final, validation, categories);

            int errors = 0;
            for (int k = 0; k < validation.size(); k++)
                if (validation[k].getCategory() != categories[k])
                    errors++;

            double clas_rate = (1.0 * errors) / (1.0 * validation.size());
            double perc_redc = (1.0 * final.size()) / (1.0 * training.size());
            double fitness = 0.5 * clas_rate + (1 - 0.5) * perc_redc;

            res.errors.push_back(errors);
            res.val_errors.push_back((errors * 100.0) / (validation.size() * 1.0));
            res.sizes.push_back((final.size() * 100.0) / (training.size() * 1.0));
        }
    }

    // std::ostream *out;
   // std::ofstream rFile;

   // if(flag_r) {
   //     rFile.open(out_fn, std::ios::out);
   //     out = &rFile;
   // } else {
   //     out = &std::cout;
   // }
    // double esum = std::accumulate(res.errors.begin(), res.errors.end(), 0.0);
    // double emean = esum / res.errors.size();
    // double vsum = std::accumulate(res.sizes.begin(), res.sizes.end(), 0.0);
    // double vmean = vsum / res.sizes.size();
    // double ssum = std::accumulate(res.val_errors.begin(), res.val_errors.end(), 0.0);
    // double smean = ssum / res.val_errors.size();

    //*out << "Val_errors" << std::endl;
    //for (unsigned i = 0; i < res.val_errors.size(); ++i)  {
    //    *out << res.val_errors[i] << " ";
    //}
    //*out << std::endl;
    //*out << "Sizes" << std::endl;
    //for (unsigned i = 0; i < res.sizes.size(); ++i)  {
    //    *out << res.sizes[i] << " ";
    //}
    //*out << std::endl;
    //*out << "Errors" << std::endl;
    //for (unsigned i = 0; i < res.errors.size(); ++i)  {
    //    *out << res.errors[i] << " ";
    //}
    //*out << std::endl;


    // *out << "Errors: " << emean << std::endl;
    // *out << "Size of solution %: " << smean << std::endl;
    // *out << "Validation errors: %: " << vmean << std::endl;

    //rFile.close();

}