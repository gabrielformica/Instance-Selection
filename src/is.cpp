#include <cstdio>
#include <string>
#include <cassert>
#include <cmath>
#include <vector>
#include <bitset>
#include <iostream>
#include "is.h"

const double epsilon = 0.00001;

double IS::Metaheuristic::find_nearest(const Problem &p, const Instance &a) {
    double imin = 1.0 * MAX;
    double category;
    for (Problem::const_iterator it = p.begin(); it < p.end(); ++it) {
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

void IS::Metaheuristic::oneNN(const Problem &training, 
                              const Problem &result, 
                              std::vector<double> &category) {

    for (int i = 0; i < result.size(); i++)
        category[i] = find_nearest(training, result[i]);
}

double IS::Metaheuristic::calc_distance(const Instance &a, const Instance &b) {
    double dist = 0.0;
    assert(a.size() == b.size());
    for (int i = 0; i < a.size(); ++i)
        dist += (a[i] - b[i]) * (a[i] - b[i]);
    return sqrt(dist);
}

void IS::Solution::generateRandom() {
    srand(time(NULL));
    for (int i = 0; i < size; i++) {
        int p = rand() % 100; 
        this->bits[i] = (p > 10) ? 1 : 0;
    }
}

void IS::Solution::tweak() {
    srand(time(NULL));
    int n = (rand() % bits.count()) + 1;
    int i, j;
    for (j = 0, i = 0; j < size && i < n; j++) if (bits.test(j)) i++;
    assert(bits.test(j-1));
    this->bits[j-1] = 0;
}

void IS::Metaheuristic::optimize(const IS::Problem &T, IS::Solution &S) {
    S.setSize(T.size());
    S.generateRandom();
    double q_max;
    while (1) {
        IS::Solution R(S);
        R.tweak(); 
        assert((S.getBits() ^ R.getBits()).count() == 1);

        // Using 0.5 because paper
        double q1 = quality(T, R, 0.5), q2 = quality(T, S, 0.5);
        if (q1 > q2)
            S.setBits(R.getBits());
        
        q_max = std::max(q1, q2);
        if (q_max > 0.95) break;
    }
    cout << "---> " << q_max << endl;
}

double IS::Metaheuristic::quality(const IS::Problem &T, 
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

    //cout << "Hubo un total de " << count << " aciertos" << endl;
    //cout << "Result es de tamanio: " << result.size() << endl;
    //cout << "La relación es: " << clas_rate << endl;

    //cout << "El tamaño de T es: " << T.size() << endl;
    //cout << "El porcentaje de reducción es: " << perc_redc << endl;

    double fitness = alpha * clas_rate + (1 - alpha) * perc_redc;
    cout << "El fitness es : " << fitness << std::endl;
    assert(fitness <= 1.0);
    return fitness;
}
