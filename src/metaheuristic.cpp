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

    cout << "Hubo un total de " << count << " aciertos" << endl;
    cout << "Result es de tamanio: " << result.size() << endl;
    cout << "La relación es: " << clas_rate << endl;

    cout << "El tamaño de T es: " << T.size() << endl;
    cout << "El porcentaje de reducción es: " << perc_redc << endl;

    double fitness = alpha * clas_rate + (1 - alpha) * perc_redc;
    cout << "El fitness es : " << fitness << endl << endl;
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
        tweak(R); 
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

void HillClimbing::tweak(IS::Solution &S) {
    std::bitset<MAX> bits = S.getBits();
    int size = S.getSize();
    srand(time(NULL));
    int n = (rand() % bits.count()) + 1;
    int i, j;
    for (j = 0, i = 0; j < size && i < n; j++) if (bits.test(j)) i++;
    assert(bits.test(j-1));
    bits[j-1] = 0;
    S.setBits(bits);
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
        tweak(R); 
        assert((S.getBits() ^ R.getBits()).count() == 1);

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
        cout << "temperatura = " << t << endl;
        
        if (qs > qb) { 
            best.setBits(S.getBits());
            qb = qs;
        }

        if (qb > 0.95 || t <= epsilon) break;
    }
}

void SimulatedAnnealing::tweak(IS::Solution &S) {
    std::bitset<MAX> bits = S.getBits();
    int size = S.getSize();
    srand(time(NULL));
    int n = (rand() % bits.count()) + 1;
    int i, j;
    for (j = 0, i = 0; j < size && i < n; j++) if (bits.test(j)) i++;
    assert(bits.test(j-1));
    bits[j-1] = 0;
    S.setBits(bits);

    //std::bitset<MAX> bits = S.getBits();
    //srand(time(NULL));
    //int i = (rand() % S.getSize());
    //bits.flip(i);  // Toggle n-th bit
    //S.setBits(bits);
}
