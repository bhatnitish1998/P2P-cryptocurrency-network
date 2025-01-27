#include "Simulator.h"

#include <utility>

bool Event::operator >(const Event& other) const
{
    if (time == other.time) return type > other.type;
    else return time > other.time;
}

Event::Event(double time, int type, variant<int, Transaction*, Block*> object)
{
    this->time = time;
    this->type = type;
    this->object = object;
}

void Simulator::create_genesis()
{
    auto* genesis = new Block(0.0);
    // create coinbase transaction for each node with initial bitcoin
    for (int i = 0; i < number_of_nodes; i++)
    {
        auto temp = Transaction(i, initial_bitcoin, true);
        genesis->transactions.push_back(temp);
    }

    // Add genesis block to all node
    for (int i = 0; i < number_of_nodes; i++)
    {
        nw.nodes[i].genesis = genesis;
        nw.nodes[i].leaves.emplace_back(genesis, 1);
    }
}

Simulator::Simulator(int number_of_nodes, double percent_fast, double percent_high_cpu,
                     double mean_transaction_inter_arrival_time, double block_inter_arrival_time,
                     double queuing_delay_constant): nw(number_of_nodes, percent_fast, percent_high_cpu)
{
    simulation_time = 0;
    this->number_of_nodes = number_of_nodes;
    this->percent_fast = percent_fast;
    this->percent_high_cpu = percent_high_cpu;
    this->mean_transaction_inter_arrival_time = mean_transaction_inter_arrival_time;
    this->block_inter_arrival_time = block_inter_arrival_time;
    this->queuing_delay_constant = queuing_delay_constant;
}

void Simulator::create_initial_transactions()
{
    double event_time = 0;

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
