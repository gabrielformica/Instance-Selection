/**
  * @file
  * @author Gabriel Formica <gabrielformica93@gmail.com>
  * @author Melecio Ponte <pontezambrano@gmail.com>
  *
  * @section Description
  *
  * Metaheuristics' classes
  */

#include "metaheuristic.h"
#include "is.h"

using namespace std;

/**
  * Calculate euclidean distance. Simple call IS::Instance Method
  * @param 'a'  :  Instance
  * @param 'b'  :  Instance
  * @return Euclidean distance between a and b
  */
double Metaheuristic::calc_distance(const IS::Instance &a, 
                                    const IS::Instance &b) const {
    return a.calcDistance(b);
}

/**
  * Find instance nearest to an instance
  * @param 'p'  :  Dataset
  * @param 'a'  :  Instance
  * @return Euclidean distance to the nearest instance in p to a 
  *         (and different to a)
  */
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

/**
  * One-NN method. Just calculate the nearest distances of elements
  * in result to p. Save these distances in category
  * @param 'training'  :  Dataset
  * @param 'result'    :  Dataset 
  * @param 'Category'  :  vector<double> (vector of distances)
  */
void Metaheuristic::oneNN(const IS::Dataset &training, 
                          const IS::Dataset &result, 
                          std::vector<double> &category) const {

    for (int i = 0; i < result.size(); i++)
        category[i] = find_nearest(training, result[i]);
}

/**
  * Fitness function.
  * @param 'T'      :  Dataset
  * @param 'S'      :  Solution 
  * @param 'alpha'  :  double
  * 
  * This function calculates:
  * alpha * (instances_well_clasified) + (1 - alpha) * (percentage_of_reduction)
  *
  * @return Fitness of S. 
  */
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
      cout << "Hubo un total de " << count << " aciertos" << endl;
      cout << "Result es de tamanio: " << result.size() << endl;
      cout << "La relación es: " << clas_rate << endl;
      cout << "El tamaño de T es: " << T.size() << endl;
      cout << "El tamaño de training es: " << training.size() << endl;
      cout << "El porcentaje de reducción es: " << perc_redc << endl;
      cout << "El fitness es : " << fitness << endl << endl;

    assert(fitness <= 1.0);
    return fitness;
}


/**
  * Optimize of HillClimbing
  * @param 'T'      :  Dataset
  * @param 'S'      :  Solution 
  *
  * This function save in S the best found solution 
  */
void HillClimbing::optimize(const IS::Dataset &T, IS::Solution &S) const {
    std::cout << ">>>> Running Hill Climbing" << std::endl;
    std::cout << "quality = " << max_quality << std::endl;
    std::cout << "ite_limit = " << ite_limit << std::endl;
    std::cout << "no_change_best = " << no_change_best << std::endl;
    std::cout << ">>>>>>>>>>>>>>>>>>>>" << std::endl;

    double q_max, q_best = -1;
    int no_change = 0, iter = 0;
    while (1) {
        iter++;
        IS::Solution R(S);
        tweaker->tweak(R); 
        // assert((S.getBits() ^ R.getBits()).count() == 1);

        // Using 0.5 because paper
        double q1 = quality(T, R, 0.5), q2 = quality(T, S, 0.5);
        if (q1 > q2)
            S.setBits(R.getBits());
        
        q_max = std::max(q1, q2);
        if (q_best > q_max) {
            no_change++;  
        } else {
            q_best = q_max;
            no_change = 0;
        }
        // TODO: Pass these values as parameters
        bool stop = q_max > max_quality or no_change == no_change_best;
        stop = stop || iter == ite_limit;
        if (stop) break;
    }
}

std::string HillClimbing::output_params() const {
    return "HillC_";
}

/**
  * Optimize of ILS 
  * @param 'T'      :  Dataset
  * @param 'S'      :  Solution 
  *
  * This function save in S the best found solution 
  */
void ILS::optimize(const IS::Dataset &T, IS::Solution &S) const {
    std::cout << ">>>> Running ILS" << std::endl;
    std::cout << "quality = " << max_quality << std::endl;
    std::cout << "ite_limit = " << ite_limit << std::endl;
    std::cout << "no_change_best = " << no_change_best << std::endl;
    std::cout << ">>>>>>>>>>>>>>>>>>>>" << std::endl;

    IS::Solution best(S);

    double q_max, q_BEST = -1.0;
    int global_iter = 0;

    int max_out_iter = ite_limit;
    int no_change = 0;
    int max_local_iter = getLocalIter(); 

    while(1) {
        global_iter++;
        // Local search
        int local_iter = 0;
        while(1) {
            local_iter++;
            IS::Solution R(S);
            tweaker->tweak(R); 
            double q1 = quality(T, R, 0.5), q2 = quality(T, S, 0.5);
            if (q1 > q2)
                S.setBits(R.getBits());

            q_max = std::max(q1, q2);
            bool stop = q_max > max_quality or local_iter == max_local_iter;
            if (stop) break;
        }

        // Compare and (maybe) perturb
        double qs = quality(T, S, 0.5);
        double qb = quality(T, best, 0.5);

        if (qs > qb) { 
            best.setBits(S.getBits());
            no_change = 0;
        } else {
            no_change++;
        }
        //weightedRandomPlus tweaker = weightedRandomPlus(perc, 50);
        int perc = S.getSize() / getPerturbPerc();
        nRandomFlips tweaker = nRandomFlips(perc);
        tweaker.tweak(S);

        bool stop = q_max > max_quality or global_iter == max_out_iter;
        stop = stop or no_change == no_change_best;
        if (stop) break;
    }
    S.copy(best);
}

std::string ILS::output_params() const {
    std::string params = "ILS_" + std::to_string(ite_limit) + "_";
    params += std::to_string(li) + "_";
    params += std::to_string(pp);
    return params;
}


/**
  * Optimize of ILS 
  * @param 'T'      :  Dataset
  * @param 'S'      :  Solution 
  *
  * This function save in S the best found solution 
  */
void Tabu::optimize(const IS::Dataset &T, IS::Solution &best) const {
    std::cout << ">>>> Running Tabu" << std::endl;
    std::cout << "quality = " << max_quality << std::endl;
    std::cout << "ite_limit = " << ite_limit << std::endl;
    std::cout << "no_change_best = " << no_change_best << std::endl;
    std::cout << "Number of Tweaks = " << number_of_tweaks << std::endl;
    std::cout << "Length of the list = " << length << std::endl;
    std::cout << ">>>>>>>>>>>>>>>>>>>>" << std::endl;

    IS::Solution S(best);
    int max_length = (length * T.size() / 100);

    std::deque<IS::Solution> tl;   // Tabu list
    tl.push_back(S);
    double q_max, q_BEST = -1;
    int no_change = 0, iter = 0;
    while (1) {
        iter++;
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
        // TODO: Tune iter_total and no_change

        if (q_max > q_BEST) {
            q_BEST = q_max;
            no_change = 0;
        } else no_change++;  

        bool stop = q_max > max_quality || no_change == no_change_best;
        stop = stop || iter == ite_limit;
        if (stop) break;
    }
}

std::string Tabu::output_params() const {
    std::string params = "Tabu_" + std::to_string(length) + "_";
    params += std::to_string(number_of_tweaks);
    return params;
}
