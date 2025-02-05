#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

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

// Creates a file with name fname and write graphs nodes in it.
void write_network_to_file(vector<vector<int>>& al,const string& fname);

#endif //UTILITY_H
