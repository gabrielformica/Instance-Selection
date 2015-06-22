#include "metaheuristic.h"
#include "is.h"

using namespace std;

/* Metaheuristics methods */

double Metaheuristic::calc_distance(const IS::Instance &a, 
                                    const IS::Instance &b) const {
    return a.calcDistance(b);
}

double Metaheuristic::find_nearest(const IS::Dataset &p, const IS::Instance &a) const {
    double imin = 10000000;
    double category;
    for (IS::Dataset::const_iterator it = p.begin(); it < p.end(); ++it) {
        double dist = calc_distance(*it, a);
        if (dist < EPSILON) {
            category = (*it).getCategory();
            break;
        }
        if (dist < imin) {
            imin = dist;                  
            category = (*it).getCategory();
        }
    }
    return category;
}

void Metaheuristic::oneNN(const IS::Dataset &training, 
                          const IS::Dataset &result, 
                          std::vector<double> &category) const {

    for (int i = 0; i < result.size(); i++)
        category[i] = find_nearest(training, result[i]);
}

double Metaheuristic::quality(const IS::Dataset &T, 
                              const IS::Solution &S,
                              double alpha) const {

    std::bitset<MAX> bits = S.getBits();
    if (bits.count() == 0) return 0.5;

    IS::Dataset training, result;
    std::vector<double> category;
    int j = 0;
    for (int i = 0; i < T.size(); i++) {
        if (bits.test(i)) training.push_back(T[i]); 
        else result.push_back(T[i]);
    }

    category.assign(result.size(), -1);
    oneNN(training, result, category);

    int count = 0;
    for (int i = 0; i < result.size(); i++)
        if (result[i].getCategory() == category[i])
            count++;

    double clas_rate = (1.0 * count) / (1.0 * result.size());
    double perc_redc = (1.0 * result.size()) / (1.0 * T.size());
    double fitness = alpha * clas_rate + (1 - alpha) * perc_redc;

    // // XXX: Print
      //cout << "Hubo un total de " << count << " aciertos" << endl;
      //cout << "Result es de tamanio: " << result.size() << endl;
      //cout << "La relación es: " << clas_rate << endl;
      //cout << "El tamaño de T es: " << T.size() << endl;
      //cout << "El tamaño de training es: " << training.size() << endl;
      //cout << "El porcentaje de reducción es: " << perc_redc << endl;
      //cout << "El fitness es : " << fitness << endl << endl;

    assert(fitness <= 1.0);
    return fitness;
}


/* Hill Climbing */

void HillClimbing::optimize(const IS::Dataset &T, IS::Solution &S) const {
    std::cout << ">>>> Running Hill Climbing" << std::endl;
    std::cout << ">>>> quality = " << max_quality << std::endl;
    double q_max, old_q = -1;
    int iter_old = 0, iter_total = 0;
    while (1) {
        iter_total++;
        IS::Solution R(S);
        tweaker->tweak(R); 
        // assert((S.getBits() ^ R.getBits()).count() == 1);

        // Using 0.5 because paper
        double q1 = quality(T, R, 0.5), q2 = quality(T, S, 0.5);
        if (q1 > q2)
            S.setBits(R.getBits());
        
        q_max = std::max(q1, q2);
        if (old_q == q_max) {
          iter_old ++;  
        } else {
            iter_old = 0;
        }
        // TODO: Pass these values as parameters
        if (q_max > max_quality or iter_old == 1000) break;
        old_q = q_max;

        if(iter_total % 100 == 0 and iter_total != 0) {
            cout << "Total iterations: " << iter_total << endl;
            cout << "Best fitness so far: " << q_max << endl;
            cout << "Solution size: " << S.getBits().count() << endl << endl;
        }
    }
}

std::string HillClimbing::output_params() const {
    return "HillC_";
}

/* Simulated Annealing  */

void SimulatedAnnealing::optimize(const IS::Dataset &T, IS::Solution &S) const {
    assert(T.size() == S.getSize());
    std::cout << ">>>> Running Simulated Annealing" << std::endl;
    std::cout << ">>>> quality = " << max_quality << std::endl;

    IS::Solution best(S);  // Best solution

    srand(time(NULL));

    int t = temperature;  // Temperature 
    while (1) {
        IS::Solution R(S);
        tweaker->tweak(R); 
        // assert((S.getBits() ^ R.getBits()).count() == 1);

        // Using 0.5 because paper
        double qr = quality(T, R, 0.5), qs = quality(T, S, 0.5);
        double qb = quality(T, best, 0.5);
        // Random number between 0 and 1
        double r = ((double) rand() / (double) (RAND_MAX));
        double prob = -5.0;
        if (t > 0) prob = exp((1.0 * (qr - qs)) / ((1.0) * t));

        if (qr > qs || (prob - r > EPSILON)) {
            S.setBits(R.getBits());
            qs = qr;
        }
        t -= dec_factor;  // Decreasing temperature
        // cout << "temperatura = " << t << endl;
        
        if (qs > qb) {
            best.setBits(S.getBits());
            qb = qs;
        }

        if (qb > max_quality || t <= EPSILON) break;
    }
}

std::string SimulatedAnnealing::output_params() const {
    std::string params = "SimulAnn_" + std::to_string(temperature) + "_";
    params += std::to_string(dec_factor);
    return params;
}

/* ILS */

void ILS::optimize(const IS::Dataset &T, IS::Solution &S) const {
    std::cout << ">>>> Running ILS" << std::endl;
    std::cout << ">>>> quality = " << max_quality << std::endl;

    IS::Solution best(S);
    IS::Solution home_base(S);

    double q_max, q_BEST = -1.0;
    int global_iter = 0;
    
    // TODO: Tune this
    int max_out_iter = getTotalIter();
    int max_iter = getLocalIter(); 
    std::cout << "local iter = " << max_iter << endl;

    while(1) {
        // Local search

        // TODO: Ideally some tuneable number of iterations
        // 500 to 1000 random comes to mind
        cout << ">>> Running local search" << endl;
        int local_iter = 0;
        while(1) {
            IS::Solution R(S);
            tweaker->tweak(R); 
            double q1 = quality(T, R, 0.5), q2 = quality(T, S, 0.5);
            if (q1 > q2)
                S.setBits(R.getBits());

            q_max = std::max(q1, q2);
            if (q_max > q_BEST) {
                q_BEST = q_max;
                local_iter = 0;
            } else local_iter++;

            // std::cout << "ITER ---XXXXXX " << local_iter << std::endl;
            if (q_max > max_quality or local_iter == max_iter) break;
        }

        // Compare and (maybe) perturb
        double qs = quality(T, S, 0.5);
        double qb = quality(T, best, 0.5);

        if (qs > qb) best.setBits(S.getBits());
        // Flipping 25% of bits for perturb
        // TODO: This CAN be an argument
        // cout << ">>> Perturbing <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
        //weightedRandomPlus tweaker = weightedRandomPlus(perc, 50);
        int perc = S.getSize() / getPerturbPerc();
        // cout << "perc ---> " << perc << endl;
        nRandomFlips tweaker = nRandomFlips(perc);
        tweaker.tweak(S);

        global_iter++;
        if (q_max > max_quality or global_iter == max_out_iter) break;
    }
    S.copy(best);
}

std::string ILS::output_params() const {
    std::string params = "ILS_" + std::to_string(ti) + "_";
    params += std::to_string(li) + "_";
    params += std::to_string(pp);
    return params;
}


/* Tabu */

void Tabu::optimize(const IS::Dataset &T, IS::Solution &best) const {
    std::cout << ">>>> Running Tabu" << std::endl;
    std::cout << ">>>> quality = " << max_quality << std::endl;
    IS::Solution S(best);
    int max_length = (length * T.size() / 100);

    std::deque<IS::Solution> tl;   // Tabu list
    tl.push_back(S);
    double q_max, q_BEST = -1;
    int iter_old = 0;
    while (1) {
        while (tl.size() > max_length) tl.pop_front();
        IS::Solution R(S);
        tweaker->tweak(R); 

        for (int i = 0; i < number_of_tweaks - 1; i++) {
            IS::Solution W(S); 
            tweaker->tweak(W);

            // if W is not int tabu list
            if (std::find(tl.begin(), tl.end(), W) == tl.end()) {
                double qw = quality(T, W, 0.5), qr = quality(T, R, 0.5);
                if (qw > qr || std::find(tl.begin(), tl.end(), R)
                               != tl.end()) 
                    R.copy(W);
            }

        }
        // R is not in L and r is better
        bool R_is_better = quality(T, R, 0.5) > quality(T, S, 0.5);
        if (std::find(tl.begin(), tl.end(), R) == tl.end() and R_is_better) {
            S.copy(R);
            tl.push_back(R);
        }

        double qs = quality(T, S, 0.5), qb = quality(T, best, 0.5);
        q_max = std::max(qs, qb);
        if (qs > qb) best.copy(S);
        // TODO: Tune iter_total and iter_old

        if (q_max > q_BEST) {
            q_BEST = q_max;
            iter_old = 0;
        } else iter_old++;  

        if (q_max > max_quality || iter_old == iter_limit) break;
    }
}

std::string Tabu::output_params() const {
    std::string params = "Tabu_" + std::to_string(length) + "_";
    params += std::to_string(number_of_tweaks);
    return params;
}
