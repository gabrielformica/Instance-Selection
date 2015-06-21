#include "is.h"
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

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

results run_tenfold(const vps &datasets, 
                 const Metaheuristic *metaheuristic, 
                 int runs);

IS::Dataset load_dataset(const char *filename);

void output_results(const results &res, 
                    const Metaheuristic *metaheuristic,
                    bool flag_r,
                    std::string out_fn);

void debug_print(const IS::Dataset &dataset, const IS::Solution &solution);
