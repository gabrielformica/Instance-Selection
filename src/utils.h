#include "is.h"

typedef IS::Dataset ds;
typedef std::pair<ds, ds> pds;
typedef std::vector<pds> vps;

typedef struct results {
    std::vector<int> errors;
    std::vector<float> sizes;
    std::vector<float> val_errors;
} result;


IS::Dataset load_data_basic(std::string file_name_str);

void load_data_tenfold(std::string path, vps &datasets);

void print_help();

void error_(std::string msg);

void print_solution(const IS::Dataset &problem, const IS::Solution &sol);

void print_dispersions(const IS::Dataset &dataset);

Metaheuristic *choose_metaheuristic(std::string metaheuristic_str, 
                                    int (&metaheuristic_optional_arg)[10],
                                    int moa_counter);

Tweaker *choose_tweaker(std::string tweaker_str,
                        int (&tweaker_optional_arg)[10],
                        int toa_counter);

void run_tenfold(const vps &datasets, 
                 const Metaheuristic *metaheuristic, 
                 int runs);

IS::Dataset load_dataset(const char *filename);
