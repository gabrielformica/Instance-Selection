#ifndef IS_H
#define IS_H

#include <cstdio>
#include <string>
#include <cassert>
#include <cmath>
#include <vector>
#include <bitset>
#include <cstdlib>
#include <ctime>
#define MAX 10000

using namespace std;

namespace IS {
    class Instance : public std::vector<double> {
      private:
        double category;
      public:
        double getCategory() const { return category; }
        void setCategory(double c) { category = c; }
    }; 

    class Solution {
      private:
        std::bitset<MAX> bits;
        int size;
      public:
        Solution(int N) : size(N) { };
        Solution(std::bitset<MAX> a, int N) : bits(a), size(N) { };
        Solution(const Solution &a) : bits(a.getBits()), size(a.getSize()) { };
        int getSize() const { return size; };
        void setSize(int n) { size = n; };
        std::bitset<MAX> getBits() const { return bits; };
        void setBits(std::bitset<MAX> a) { bits = a; }
        void generateRandom(); 
        void tweak();
        std::string to_str() {
            std::string str = "";
            for (int i = 0; i < size; i++)
                str = (bits.test(i) ? "1" : "0") + str; 
            return str;
        }
    };

    class Problem : public std::vector<Instance> {
      public:
        Problem() { };
        Problem(const Problem& a) : vector<Instance>(a.begin(), a.end()) { };
        Problem(int N) : std::vector<Instance>(N) {};
    };


    class Metaheuristic {
      public:
        void optimize(const Problem &T, Solution &R);
        double quality(const IS::Problem &T, const IS::Solution &S);
        IS::Instance find_nearest(const Problem &p, const Instance &a);
        void oneNN(const Problem &training, Problem *result);
        double calc_distance(const Instance &a, const Instance &b); 
        void tweak(Solution S);
    };
}

#endif
