#include "metaheuristic.h"
#include "is.h"

using namespace std;

/* Metaheuristics methods */

double Metaheuristic::calc_distance(const IS::Instance &a, const IS::Instance &b) {
    return a.calcDistance(b);
    //double dist = 0.0;
    //assert(a.size() == b.size());
    //for (int i = 0; i < a.size(); ++i)
    //    dist += (a[i] - b[i]) * (a[i] - b[i]);
    //return sqrt(dist);
}

double Metaheuristic::find_nearest(const IS::Problem &p, const IS::Instance &a) {
    double imin = 1.0 * MAX;
    double category;
    for (IS::Problem::const_iterator it = p.begin(); it < p.end(); ++it) {
        double dist = calc_distance(*it, a);
        if (dist < epsilon) {
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

void Metaheuristic::oneNN(const IS::Problem &training, 
                          const IS::Problem &result, 
                          std::vector<double> &category) {

    for (int i = 0; i < result.size(); i++)
        category[i] = find_nearest(training, result[i]);
}

double Metaheuristic::quality(const IS::Problem &T, 
                              const IS::Solution &S,
                              double alpha) {

    std::bitset<MAX> bits = S.getBits();
    IS::Problem training, result;
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

    // cout << "Hubo un total de " << count << " aciertos" << endl;
    // cout << "Result es de tamanio: " << result.size() << endl;
    // cout << "La relación es: " << clas_rate << endl;

    // cout << "El tamaño de T es: " << T.size() << endl;
    // cout << "El porcentaje de reducción es: " << perc_redc << endl;

    double fitness = alpha * clas_rate + (1 - alpha) * perc_redc;
    // cout << "El fitness es : " << fitness << endl << endl;
    assert(fitness <= 1.0);
    return fitness;
}


/* Hill Climbing */

void HillClimbing::optimize(const IS::Problem &T, IS::Solution &S) {
    S.setSize(T.size());
    S.generateRandom();
    double q_max, old_q = -1;
    int iter_old = 0;
    while (1) {
        IS::Solution R(S);
        getTweaker()->tweak(R); 
        assert((S.getBits() ^ R.getBits()).count() == 1);

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
        if (q_max > 0.95 or iter_old == 1000) break;
        old_q = q_max;
    }
    cout << "---> " << q_max << endl;
}

/* Simulated Annealing  */

void SimulatedAnnealing::optimize(const IS::Problem &T, IS::Solution &S) {
    S.setSize(T.size());
    S.generateRandom();   // Initial solution

    IS::Solution best(S);  // Best solution

    srand(time(NULL));

    int t = temperature;  // Temperature 
    while (1) {
        IS::Solution R(S);
        getTweaker()->tweak(R); 
        // assert((S.getBits() ^ R.getBits()).count() == 1);

        // Using 0.5 because paper
        double qr = quality(T, R, 0.5), qs = quality(T, S, 0.5);
        double qb = quality(T, best, 0.5);
        // Random number between 0 and 1
        double r = ((double) rand() / (RAND_MAX));
        double prob = -1.0;
        if (t > epsilon) prob = exp((qr - qs) / t);

        if (qr > qs || (prob - r > epsilon)) {
            S.setBits(R.getBits());
            qs = qr;
        }
        t -= dec_factor;  // Decreasing temperature
        // cout << "temperatura = " << t << endl;
        
        if (qs > qb) { 
            best.setBits(S.getBits());
            qb = qs;
        }

        if (qb > 0.95 || t <= epsilon) break;
    }
}

/* ILS */

void ILS::optimize(const IS::Problem &T, IS::Solution &S) {
    S.setSize(T.size());
    S.generateRandom();

    IS::Solution best(S);
    IS::Solution home_base(S);

    double q_max, old_q = -1;
    int iter_old = 0, iter = 0;
    
    // TODO: Tune this
    int max_out_iter = 1000;

    while(1) {
        // Local search

        // TODO: Ideally some tuneable number of iterations
        // 500 to 1000 random comes to mind
        int max_iter = 1000; 
        while(1) {
            IS::Solution R(S);
            getTweaker()->tweak(R); 
            double q1 = quality(T, R, 0.5), q2 = quality(T, S, 0.5);
            if (q1 > q2)
                S.setBits(R.getBits());

            q_max = std::max(q1, q2);
            if (old_q == q_max) {
              iter_old ++;  
            } else {
                iter_old = 0;
            }
            if (q_max > 0.95 or iter_old == max_iter) break;
            old_q = q_max;
        }

        // Compare and (maybe) perturb
        double qs = quality(T, S, 0.5);
        double qb = quality(T, best, 0.5);

        if (qs > qb) best.setBits(S.getBits());
        // Flipping 25% of bits for perturb
        // TODO: This CAN be an argument
        nRandomFlips tweaker = nRandomFlips(S.getSize() / 4);
        tweaker.tweak(S);

        iter++;
        if (q_max > 0.95 or iter == max_out_iter) break;

    }
}

/* Tabu */

void Tabu::optimize(const IS::Problem &T, IS::Solution &best) {
    IS::Solution S(T.size());
    S.generateRandom();  // First random solution

    std::deque<IS::Solution> tabu_list;   // Tabu list
    tabu_list.push_back(S);
    best.copy(S);
    while (1) {
        if (tabu_list.size() > length) tabu_list.pop_front();
        IS::Solution R(S);
        getTweaker()->tweak(R); 

        for (int i = 0; i < number_of_tweaks - 1; i++) {
            IS::Solution W(S); 
            getTweaker()->tweak(W);

            // Checking if W is in tabu list
            std::deque<IS::Solution>::iterator it;
            it = std::find(tabu_list.begin(), tabu_list.end(), W);
            bool is_in = it != tabu_list.end();
            if (! is_in) {
                double qw = quality(T, W, 0.5), qr = quality(T, R, 0.5);
                if (qw > qr || std::find(tabu_list.begin(), tabu_list.end(), R)
                               != tabu_list.end())
                R.copy(W);
            }

            // R is not in L
            if (std::find(tabu_list.begin(), tabu_list.end(), R) == tabu_list.end()) {
                S.copy(R);
                tabu_list.push_back(R);
            }

            double qs = quality(T, S, 0.5), qb = quality(T, best, 0.5);
            double q_max = std::max(qs, qb);
            if (qs > qb) best.copy(S);
            if (q_max > 0.95) break;
        }
    }
}
