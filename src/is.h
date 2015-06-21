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
#define MAX 8000 

using namespace std;

namespace IS {
    class Instance : public std::vector<double> {
      private:
        double category;
      public:
        double getCategory() const { return category; }
        void setCategory(double c) { category = c; }
        double calcDistance(const Instance &a) const;
        std::string toString() const;
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
        void copy(const Solution &a);
        bool operator==(const Solution &b) const {
            return (size == b.size && bits == b.bits);
        }
    };

    class Dataset : public std::vector<Instance> {
      private:
        int attrs;
      public:
        Dataset() { };
        Dataset(const Dataset& a) : vector<Instance>(a.begin(), a.end()), 
                                    attrs(a.getAttrs()) { };
        Dataset(int N, int a) : std::vector<Instance>(N), attrs(a) {};
        void setAttrs(int a) { attrs = a; }
        int getAttrs() const { return attrs; }
        void getDispersions(std::vector<double> &dispersion) const;
        std::string toString() const;
    };
}

#endif
