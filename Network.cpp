#include "Network.h"
#include "utility_functions.h"
#include <iostream>

int Node::node_ticket = 0;

Link::Link(int peer, int propagation_delay, int link_speed)
{
    this->peer = peer;
    this->propagation_delay = propagation_delay;
    this->link_speed = link_speed;
}

Node::Node()
{
    id = node_ticket++;
    fast = false;
    high_cpu = false;
    peers.reserve(6);
}

Network::Network(int n, double percent_fast, double percent_high_cpu)
{
    this->n = n;

    // Node id equal to its index in vector
    nodes.resize(n);


    //   Make ( n * percent_fast) fast nodes
    vector<int> fast_nodes = choose_percent(n, percent_fast / 100.0);
    for (const int node_id : fast_nodes)
    {
        nodes[node_id].fast = true;
    }

    //   Make ( n * percent_high_cpu) high_cpu nodes
    vector<int> high_cpu_nodes = choose_percent(n, percent_high_cpu / 100.0);
    for (const int node_id : high_cpu_nodes)
    {
        nodes[node_id].high_cpu = true;
    }

    bool done = false;
    vector<vector<int>> al(n);

    while (!done)
    {
        al.clear();
        for (int i = 0; i < n; i++)
        {
            constexpr int min_peers = 3;
            while (al[i].size() < min_peers)
            {
                vector<int> temp = choose_neighbours(n, min_peers - al[i].size(), al[i]);
                for (auto neighbour : temp)
                {
                    if (neighbour != i && al[neighbour].size() < 6)
                    {
                        al[i].push_back(neighbour);
                        al[neighbour].push_back(i);
                    }
                }
            }
        }

        done = check_connected(al);
    }

    for (int i = 0; i < n; i++)
    {
        for (auto x : al[i])
        {
            if (i < x)
            {
                int propagation_delay = uniform_distribution(10, 500);
                int link_speed = nodes[i].fast && nodes[x].fast ? 100 : 5;
                nodes[i].peers.emplace_back(x, propagation_delay, link_speed);
                nodes[x].peers.emplace_back(i, propagation_delay, link_speed);
            }
        }
    }
}

void Network::create_genesis()
{
    auto* genesis = new Block();
    // create coinbase transaction for each node with initial bitcoin
    for (int i = 0; i < n; i++)
    {
        auto temp = Transaction(i, initial_bitcoin, true);
        genesis->transactions.push_back(temp);
    }

    // Add genesis block to all node
    for (int i = 0; i < n; i++)
    {
        nodes[i].genesis = genesis;
        nodes[i].leaves.emplace_back(genesis, 1);
    }
}
