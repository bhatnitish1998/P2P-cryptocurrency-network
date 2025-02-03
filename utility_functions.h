#ifndef UTILITY_H
#define UTILITY_H

#include <vector>

using namespace std;

extern unsigned int global_seed;

// min and max are inclusive
int uniform_distribution(int min, int max);

// returns discrete values (milliseconds)
int exponential_distribution(double mean);

// returns percentage of nodes from given 0 to n-1 nodes
vector<int> choose_percent(int n, double percent);

// returns k nodes apart from those in <excluded> from 0 to n-1 nodes.
vector<int> choose_neighbours(int n, int k, vector<int> excluded);

// checks if the given graph is connected using dfs
bool check_connected(vector<vector<int>>& al);

#endif //UTILITY_H
