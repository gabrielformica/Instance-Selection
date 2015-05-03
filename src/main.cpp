#include <cstdlib>
#include <iostream>
#include <string>
#include <cstdio>
#include "is.h"

IS::Problem load_data(char *file_name) {
    FILE *input = fopen(file_name, "r");
    int instances, attrs;

    if(input) 
        printf("File: %s\n", file_name);
    else {
        perror("ERROR at data load");
        exit(1);
    }

    if (fscanf(input, "%d %d", &instances, &attrs) != 2) {
        printf("ERROR at data load: file malformed\n");
        exit(1);
    }
    
    IS::Problem problem = IS::Problem(instances);
    problem.setAttrs(attrs);

    for(unsigned i = 0; i < instances; ++i) {
        // attrs-1 because last element of line is category
        double attr;
        for(unsigned j = 0; j < attrs-1; ++j) { 
            fscanf(input, "%lf", &attr);
            problem[i].push_back(attr);
        }
        fscanf(input, "%lf", &attr);
        problem[i].setCategory(attr);
    }

    return problem;
}


int main(int argc, char *argv[]) {

    IS::Problem problem = load_data(argv[1]);
    IS::Metaheuristic ls;
    IS::Solution sol(problem.size());

    std::cout << ">> Local search: " << std::endl;
    ls.optimize(problem, sol);

    std::bitset<MAX> bits = sol.getBits();
    for(unsigned i = 0; i < sol.getSize(); ++i) {
        if(bits.test(i)) {
            for(unsigned j = 0; j < problem.getAttrs()-1; ++j) {
                cout << problem[i][j] << " ";
            }
            cout << problem[i].getCategory() << endl;
        }
    }


    //IS::Problem training = IS::Problem(training_size);
    //IS::Problem testing = IS::Problem(testing_size);

    /*for(unsigned i = 0; i < training_size; ++i) {*/
        //double attr;
        //for(unsigned j = 0; j < attrs-1; ++j) {
            //cin >> attr;
            //training[i].push_back(attr);
        //}
        //cin >> attr;
        //training[i].setCategory(attr);
    //}

    //for(unsigned i = 0; i < testing_size; ++i) {
        //double attr;
        //for(unsigned j = 0; j < attrs-1; ++j) {
            //cin >> attr;
            //testing[i].push_back(attr);
        //}
        //cin >> attr;
        //testing[i].setCategory(attr);
    /*}*/

    // cout << "TRAINING SET" << endl;
    // for(unsigned i = 0; i < training_size; ++i) {
    //     double attr;
    //     for(unsigned j = 0; j < attrs-1; ++j) {
    //         cout << training[i][j] << " ";
    //     }
    //     cout << training[i].getCategory() << endl;
    // }

    // cout << "TESTING SET" << endl;
    // for(unsigned i = 0; i < testing_size; ++i) {
    //     double attr;
    //     for(unsigned j = 0; j < attrs-1; ++j) {
    //         cout << testing[i][j] << " ";
    //     }
    //     cout << testing[i].getCategory() << endl;
    // }

  /*  IS::Problem result(testing_size);*/

    //for(unsigned i = 0; i < testing_size; ++i) {
        //for(unsigned j = 0; j < attrs-1; ++j) {
            //result[i].push_back(testing[i][j]);
            //result[i].setCategory(-1);
        //}
    //}

    //oneNN(training, &result);

    //int count_error = 0;
    //for(unsigned i = 0; i < testing_size; ++i) {

        //if(testing[i].getCategory() != result[i].getCategory()) {
            //cout << "test: " << testing[i].getCategory() << endl;
            //cout << "result: " << result[i].getCategory() << endl;
            //count_error++;
        //}
    //}
    //cout << "Error in " << count_error << " instances out of ";
    /*cout << testing_size << "." << endl;*/
}
