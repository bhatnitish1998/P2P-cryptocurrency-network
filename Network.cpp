#include "Network.h"
#include "utility_functions.h"
#include <iostream>

int Node::node_ticket = 0;

Link::Link(int peer, int propagation_delay,int link_speed)
{
    this->peer=peer;
    this->propagation_delay=propagation_delay;
    this->link_speed=link_speed;
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

    /* TODO 2: Randomly choose percent_fast nodes, and change their attribute fast to true.
       Randomly choose percent_high_cpu nodes and change their attribute high_cpu to true.
       Write the utility function in utility.cpp
    */

    //   Get ( n * percent_fast) fast nodes 
    vector<int> fast_nodes = choose_percent(n, percent_fast / 100.0);
    cerr << "Num Fast Nodes: " << fast_nodes.size() << endl;
    for (int node_id : fast_nodes) {
        nodes[node_id].fast = true;
    }

    //   Get ( n * percent_high_cpu) high_cpu nodes 
    vector<int> high_cpu_nodes = choose_percent(n, percent_high_cpu / 100.0);
    cerr << "Num High CPU Nodes: " << high_cpu_nodes.size() << endl;
    for (int node_id : high_cpu_nodes) {
        nodes[node_id].high_cpu = true;
    }

    bool done = false;
    vector<vector<int>>al(n);
    int trial = 0;

    while (!done)
    {
        trial++;
        cerr << "Trial: " << trial << endl;
        al.clear();
        int min_peers = 3;
        for (int i = 0; i < n; i++)
        {
            while (al[i].size() < min_peers)
            {
                cerr << "\rNode: " << i << " Peers: " << al[i].size() << flush;

                vector<int> temp = choose_neighbours(n, min_peers - al[i].size(), al[i]);
                for (auto neighbour : temp)
                {
                    if (neighbour!=i && al[neighbour].size() < 6)
                    {
                        al[i].push_back(neighbour);
                        al[neighbour].push_back(i);
                    }
                }
                
            }
        }

        // TODO 3: Write check_connected using dfs
        done = check_connected(al);
        if (done)
        {
            for (int i = 0; i < n; i++){
                cerr << "\rNode: " << i << " Peers: " << al[i].size() << " [";
                for (int peer : al[i]) {
                    cerr << peer << " ";
                }
                cerr << "]" << endl;
            }
        }
        cerr << "Connectedness Check Trial " << trial << ", Result: " << done << "\n" << endl;
    }

    for (int i = 0; i < n; i++)
    {
        for (auto x: al[i])
        {
            if (i < x)
            {
                // TODO 4: choose propagation delay from uniform distribution (10,500)
                int propagation_delay = uniform_distribution(10,500);
                int link_speed = nodes[i].fast && nodes[x].fast?100:5;
                nodes[i].peers.emplace_back(x,propagation_delay,link_speed);
                nodes[x].peers.emplace_back(i,propagation_delay,link_speed);
            }
        }
    }
}

void Network::create_genesis()
{
    auto *genesis = new Block();
    // create coinbase transaction for each node with initial bitcoin
    for (int i = 0; i < n; i++)
    {
        auto temp = Transaction(i,initial_bitcoin,true);
        genesis->transactions.push_back(temp);
    }

    // Add genesis block to all node
    for (int i = 0; i < n; i++)
    {
        nodes[i].genesis = genesis;
        nodes[i].leaves.push_back({genesis,1});
    }
}
