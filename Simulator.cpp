#include "Simulator.h"


void Simulator::create_genesis()
{
    auto genesis = make_shared<Block>(0, nullptr);
    // create coinbase transaction for each node with initial bitcoin
    for (int i = 0; i < number_of_nodes; i++)
    {
        auto temp = make_shared<Transaction>(i, initial_bitcoin, true);
        genesis->transactions.push_back(temp);
    }

    cout<<" Created genesis block"<<endl;
    // add genesis block to all nodes
    for (int i = 0; i < number_of_nodes; i++)
    {
        network.nodes[i].genesis = genesis;
        network.nodes[i].block_ids_in_tree.insert({genesis->id, simulation_time});

        // update leaves in each node with initial balance, transaction ids.
        auto temp = make_shared<LeafNode>(genesis, 1);
        temp->balance[i] = initial_bitcoin;
        for (const auto& x : genesis->transactions)
            temp->transaction_ids.insert(x->id);

        network.nodes[i].leaves.insert(temp);
    }
    cout << " Added genesis block to all nodes" << endl;
}

// Populate the event queue with initial transactions
void Simulator::create_initial_transactions()
{
    long long event_time = 0; // variable to keep track of future time

    for (int i = 0; i < initial_number_of_transactions; i++)
    {
        // randomly choose node who creates transaction
        const int creator_node_id = uniform_distribution(0, number_of_nodes - 1);

        // create transaction object and put into  event_queue
        create_transaction_object obj(creator_node_id);
        event_time += exponential_distribution(mean_transaction_inter_arrival_time);
        auto e = Event(event_time,CREATE_TRANSACTION, obj);
        event_queue.push(e);
    }
    cout << " Initialized event queue with " <<initial_number_of_transactions << " transactions" << endl;
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
    cout<< " Simulation started" << endl;
    // Process each type of event in event queue
    while (!event_queue.empty())
    {
        // get the event and update the simulation clock
        Event e = event_queue.top();
        event_queue.pop();
        simulation_time = e.time;

        if (e.type == CREATE_TRANSACTION)
        {
            const auto obj = std::get<struct create_transaction_object>(e.object);
            network.nodes[obj.creator_node_id].create_transaction();
        }

        if (e.type == RECEIVE_TRANSACTION)
        {
            auto obj = std::get<struct receive_transaction_object>(e.object);
            network.nodes[obj.receiver_node_id].receive_transaction(obj);
        }
        if (e.type == RECEIVE_BLOCK)
        {
            const auto obj = std::get<struct receive_block_object>(e.object);
            network.nodes[obj.receiver_node_id].receive_block(obj);
        }

        if (e.type == BLOCK_MINED)
        {
            const auto obj = std::get<struct block_mined_object>(e.object);
            network.nodes[obj.miner_node_id].complete_mining(obj.blk);
        }
    }
}
