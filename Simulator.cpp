#include "Simulator.h"

#include <utility>



void Simulator::create_genesis()
{
    auto* genesis = new Block(0);
    // create coinbase transaction for each node with initial bitcoin
    for (int i = 0; i < number_of_nodes; i++)
    {
        auto temp = Transaction(i, initial_bitcoin, true);
        genesis->transactions.push_back(temp);
    }

    // Add genesis block to all node
    for (int i = 0; i < number_of_nodes; i++)
    {
        network.nodes[i].genesis = genesis;
        network.nodes[i].leaves.emplace_back(genesis, 1);
    }
}


void Simulator::create_initial_transactions()
{
    long long event_time = 0;

    for (int i = 0; i < initial_number_of_transactions; i++)
    {
        int creater_id = uniform_distribution(0, number_of_nodes - 1);

        // TODO 6: return next arrival time using exponential distribution
        event_time += exponential_distribution(mean_transaction_inter_arrival_time);
        Event e = Event(event_time,CREATE_TRANSACTION, creater_id);
        event_queue.push(e);
    }
}

void Simulator::initialize()
{
    // create genesis blocks and add it to all nodes
    create_genesis();

    // create initial set of transactions and put it into event queue;
    create_initial_transactions();
}

void Simulator::start()
{
    while (!event_queue.empty())
    {
        Event e = event_queue.top(); event_queue.pop();
        simulation_time = e.time;

        if (e.type == CREATE_TRANSACTION)
        {
            int creater_id = get<int> (e.object);;
            network.nodes[creater_id].create_transaction(e);
        }
    }
}
