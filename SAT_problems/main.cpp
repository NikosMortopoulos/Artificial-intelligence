#include <iostream>
#include <vector>
#include "sat_generator.h"
#include "solvers_help_functions.h"  // Includes helper functions

using namespace std;

int main() {
    int variables_num, clauses_num, problem_num, max_flips, max_tries;
    double probability_negative, propability_gsat_rw, propability_walksat, propability_semi_greedy, semi_greedy_a;

    // Getting input from the user
    cout << "Enter the number of variables: ";
    cin >> variables_num;
    cout << "Enter the number of clauses: ";
    cin >> clauses_num;
    cout << "Enter the probability of negative literals (0.0 to 1.0): ";
    cin >> probability_negative;
    cout << "Enter the number of problems to generate: ";
    cin >> problem_num;
    cout << "Enter the maximum number of flips per attempt: ";
    cin >> max_flips;
    cout << "Enter the maximum number of tries to find a solution: ";
    cin >> max_tries;
    cout << "Enter the probability for random walk in GSAT-RW (0.0 to 1.0): ";
    cin >> propability_gsat_rw;
    cout << "Enter the probability for random walk in WalkSAT (0.0 to 1.0): ";
    cin >> propability_walksat;
    cout << "Enter the probability for random walk in WalkSAT (Semi-Greedy) (0.0 to 1.0): ";
    cin >> propability_semi_greedy;
    cout << "Enter the semi-greedy parameter 'a' for WalkSAT (0.0 to 1.0): ";
    cin >> semi_greedy_a;

    // Generate SAT problems
    vector<vector<vector<int>>> problems = satgenerator(variables_num, clauses_num, probability_negative, problem_num);

    // Print generated problems
    cout << "Generated SAT Problems:" << endl;
    printSATProblems(problems);

    // Apply algorithms to each problem
    for (size_t i = 0; i < problems.size(); i++) {
        cout << "Solving Problem " << i + 1 << " using GSAT..." << endl;
        vector<int> gsat_solution = gsat(problems[i], max_flips, max_tries, variables_num);
        if (!gsat_solution.empty()) {
            cout << "GSAT Solution found: ";
            for (int val : gsat_solution) {
                cout << val << " ";
            }
            cout << endl;
            cout << "GSAT Cost: " << clauses_num - number_of_clauses_satisfied_by_assignment(problems[i], gsat_solution) << endl;
        } else {
            cout << "GSAT: No solution found after " << max_tries << " tries." << endl;
        }

        cout << "Solving Problem " << i + 1 << " using GSAT-RW..." << endl;
        vector<int> gsat_rw_solution = gsat_rw(problems[i], max_flips, max_tries, propability_gsat_rw, variables_num);
        if (!gsat_rw_solution.empty()) {
            cout << "GSAT-RW Solution found: ";
            for (int val : gsat_rw_solution) {
                cout << val << " ";
            }
            cout << endl;
            cout << "GSAT-RW Cost: " << clauses_num - number_of_clauses_satisfied_by_assignment(problems[i], gsat_rw_solution) << endl;
        } else {
            cout << "GSAT-RW: No solution found after " << max_tries << " tries." << endl;
        }

        cout << "Solving Problem " << i + 1 << " using WalkSAT..." << endl;
        vector<int> walksat_solution = walksat(problems[i], max_flips, max_tries, propability_walksat, variables_num);
        if (!walksat_solution.empty()) {
            cout << "WalkSAT Solution found: ";
            for (int val : walksat_solution) {
                cout << val << " ";
            }
            cout << endl;
            cout << "WalkSAT Cost: " << clauses_num - number_of_clauses_satisfied_by_assignment(problems[i], walksat_solution) << endl;
        } else {
            cout << "WalkSAT: No solution found after " << max_tries << " tries." << endl;
        }

        cout << "Solving Problem " << i + 1 << " using WalkSAT with Semi-Greedy Initial Assignment..." << endl;
        vector<int> walksat_semi_greedy_solution = walksat_semi_greedy(problems[i], max_flips, max_tries, propability_semi_greedy, variables_num, semi_greedy_a);
        if (!walksat_semi_greedy_solution.empty()) {
            cout << "WalkSAT (Semi-Greedy) Solution found: ";
            for (int val : walksat_semi_greedy_solution) {
                cout << val << " ";
            }
            cout << endl;
            cout << "WalkSAT (Semi-Greedy) Cost: " << clauses_num - number_of_clauses_satisfied_by_assignment(problems[i], walksat_semi_greedy_solution) << endl;
        } else {
            cout << "WalkSAT (Semi-Greedy): No solution found after " << max_tries << " tries." << endl;
        }

        cout << "----------------------------------------" << endl;
    }

    return 0;
}

