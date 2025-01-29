#include "utility_functions.h"
#include <vector>
#include <ctime>
#include <algorithm>
#include <set>
#include <stack>
#include <iostream>
#include <random>

unsigned int global_seed;

int uniform_distribution(int min, int max)
{
    // return random number between min and max
    // return min + rand() % (max - min + 1);
    static std::mt19937 generator(global_seed); // Mersenne Twister generator
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

// TODO 6: return next arrival time using exponential distribution
int exponential_distribution(double mean)
{
    return 5;
}

vector<int> choose_percent(int n, double percent)
{
    // return n * percent node ids

    int num_to_select = static_cast<int>(n * percent);
    vector<int> selected_nodes;
    selected_nodes.reserve(num_to_select); // Reserve memory for num_to_select elements

    while (selected_nodes.size() < num_to_select)
    {
        int candidate = uniform_distribution(0, n - 1);
        if (find(selected_nodes.begin(), selected_nodes.end(), candidate) == selected_nodes.end())
        {
            selected_nodes.push_back(candidate);
        }
    }
    return selected_nodes;
}

vector<int> choose_neighbours(int n, int k, vector<int> excluded)
{
    // return k node ids as vector randomly from |Total nodes| - |excluded nodes| 

    vector<int> selected_nodes;
    selected_nodes.reserve(k);

    while (selected_nodes.size() < k)
    {
        int candidate = uniform_distribution(0, n - 1);
        if (find(excluded.begin(), excluded.end(), candidate) == excluded.end() &&
            find(selected_nodes.begin(), selected_nodes.end(), candidate) == selected_nodes.end())
        {
            selected_nodes.push_back(candidate);
        }
    }
    return selected_nodes;
}

bool check_connected(vector<vector<int>>& al)
{
    // check is connected all nodes
    int n = al.size();
    if (n == 0) return true;

    vector<bool> visited(n, false);
    stack<int> s;
    s.push(0);
    visited[0] = true;
    int visited_count = 1;

    while (!s.empty())
    {
        int node = s.top();
        s.pop();

        for (int neighbor : al[node])
        {
            if (!visited[neighbor])
            {
                visited[neighbor] = true;
                s.push(neighbor);
                visited_count++;
            }
        }
    }
    return visited_count == n;
}
