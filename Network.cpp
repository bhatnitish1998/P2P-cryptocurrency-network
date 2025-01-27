#include "Network.h"
#include "utility_functions.h"


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

Network::Network(int number_of_nodes, double percent_fast, double percent_high_cpu)
{
    this->number_of_nodes = number_of_nodes;

    // Node id equal to its index in vector
    nodes.resize(number_of_nodes);


    //   Make ( n * percent_fast) fast nodes
    vector<int> fast_nodes = choose_percent(number_of_nodes, percent_fast / 100.0);
    for (const int node_id : fast_nodes)
    {
        nodes[node_id].fast = true;
    }

    //   Make ( n * percent_high_cpu) high_cpu nodes
    vector<int> high_cpu_nodes = choose_percent(number_of_nodes, percent_high_cpu / 100.0);
    for (const int node_id : high_cpu_nodes)
    {
        nodes[node_id].high_cpu = true;
    }

    bool done = false;
    vector<vector<int>> al(number_of_nodes);

    while (!done)
    {
        al.clear();
        for (int i = 0; i < number_of_nodes; i++)
        {
            constexpr int min_peers = 3;
            while (al[i].size() < min_peers)
            {
                vector<int> temp = choose_neighbours(number_of_nodes, min_peers - al[i].size(), al[i]);
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

    for (int i = 0; i < number_of_nodes; i++)
    {
        for (auto x : al[i])
        {
            if (i < x)
            {
                int propagation_delay = uniform_distribution(propagation_delay_min, propagation_delay_max);
                int link_speed = nodes[i].fast && nodes[x].fast ? 100 : 5;
                nodes[i].peers.emplace_back(x, propagation_delay, link_speed);
                nodes[x].peers.emplace_back(i, propagation_delay, link_speed);
            }
        }
    }
}
