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
#define MAX 20000

using namespace std;

namespace IS {
    class Instance : public std::vector<double> {
      private:
        double category;
      public:
        double getCategory() const { return category; }
        void setCategory(double c) { category = c; }
        double calcDistance(const Instance &a) const;
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
        void setBits(const std::bitset<MAX> &a) { bits = a; }
        void generateRandom(); 
        std::string to_str() {
            std::string str = "";
            for (int i = 0; i < size; i++)
                str = (bits.test(i) ? "1" : "0") + str; 
            return str;
        }
    };

    class Problem : public std::vector<Instance> {
      private:
        int attrs;
      public:
        Problem() { };
        Problem(const Problem& a) : vector<Instance>(a.begin(), a.end()) { };
        Problem(int N) : std::vector<Instance>(N) {};
        void setAttrs(int a) { attrs = a; }
        int getAttrs() { return attrs; }
        void getDispersions(std::vector<double> &dispersion);
    };
}

#endif
