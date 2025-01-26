#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
using namespace std;

// TODO 2: from [0,n) choose percent numbers and return its vector
vector<int> choose_percent(int n, double percent);
vector<int> choose_neighbours (int n, int k, vector<int> excluded);
bool check_connected (vector<vector<int>> & al);
int uniform_distribution(int min, int max);

#endif //UTILITY_H
