#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
using namespace std;

extern unsigned int global_seed;

vector<int> choose_percent(int n, double percent);
vector<int> choose_neighbours(int n, int k, vector<int> excluded);
bool check_connected(vector<vector<int>>& al);
int uniform_distribution(int min, int max);
int exponential_distribution(long long mean);

#endif //UTILITY_H
