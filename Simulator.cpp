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

    cout << " Created genesis block" << endl;
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
    cout << " Initialized event queue with " << initial_number_of_transactions << " transactions" << endl;
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
    cout << " Simulation started" << endl;
    cout << "---------------------LOGS -----------------------"<<endl;
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
    cout << "---------------------LOGS -----------------------"<<endl;
    cout << "Simulation completed, Writing stats to files" << endl;

    // Write stats file
    write_node_stats_to_file();
    cout << "Stats written in ./files/ directory" << endl;
}

void Simulator::write_node_stats_to_file()
{
    // Check if the directory exists, if not create it
    if (fs::path dir = "files"; !fs::exists(dir))
    {
        fs::create_directories(dir);
    }


    ofstream file;
    for (int i = 0; i < number_of_nodes; i++)
    {
        string filepath = "files/Node_" + std::to_string(i) + ".txt";
        file.open(filepath);

        if (!file)
        {
            cerr << "An Error occurred while opening file!" << endl;
            return;
        }

        file << "Node ID: " << network.nodes[i].id << endl;
        file << "Fast node: " << network.nodes[i].fast << endl;
        file << "High_cpu: " << network.nodes[i].high_cpu << endl;
        file << "Peers:" << endl;
        for (auto& peer : network.nodes[i].peers)
        {
            file << "\t Node id: " << peer.peer << " Propagation delay: " << peer.propagation_delay << " ms"
                << " Link speed: " << peer.link_speed << endl;
        }
        file << "Transactions received: " << network.nodes[i].transactions_received << endl;
        file << "Blocks received: " << network.nodes[i].blocks_received << endl;

        long long blocks_created = 0;
        long long blocks_in_longest_chain = 0;
        long long created_blocks_in_longest_chain = 0;

        set<block_stats> blocks; // block statistics to be sorted by first seen time
        set<long long> block_ids; // set to keep track of already inserted blocks

        long long longest = (*network.nodes[i].leaves.begin())->block->id;

        // traverse other chains and get block stats
        for (const auto& leaf : network.nodes[i].leaves)
        {
            bool longest_flag = false;
            if (leaf->block->id == longest)
                longest_flag = true;

            shared_ptr<Block> temp_block = leaf->block;
            while (temp_block)
            {
                if (block_ids.count(temp_block->id) != 0)
                    break;

                block_stats b;
                b.block_id = temp_block->id;
                if (temp_block->parent_block)
                    b.parent_block_id = temp_block->parent_block->id;
                else
                    b.parent_block_id = -1;
                b.first_seen_time = network.nodes[i].block_ids_in_tree[temp_block->id];
                b.num_transactions = static_cast<long long>(temp_block->transactions.size());
                block_ids.insert(b.block_id);

                if (longest_flag)
                {
                    b.part_of_longest = true;
                    blocks_in_longest_chain++;
                }

                else
                    b.part_of_longest = false;

                if (temp_block->transactions[0]->receiver == i && temp_block != network.nodes[i].genesis)
                {
                    b.generated_by_node = true;
                    blocks_created++;
                    if (longest_flag) created_blocks_in_longest_chain++;
                }

                blocks.insert(b);

                temp_block = temp_block->parent_block;
            }
        }


        file << "Blocks mined: " << blocks_created << endl;
        file << "Blocks mined in longest chain: " << created_blocks_in_longest_chain << endl;
        file << "Number of blocks in longest chain: " << blocks_in_longest_chain << endl;
        file << "Fraction of blocks mined in longest chain: " <<
            static_cast<double>(created_blocks_in_longest_chain) / static_cast<double>(blocks_in_longest_chain) << endl;

        file << "Blockchain: " << endl;
        file << "Block_id, parent_block_id, first_seen_time, number_of_transactions, part_of_longest, mined by node" <<
            endl;
        for (auto x : blocks)
        {
            file << x.block_id << ", " << x.parent_block_id << ", " << x.first_seen_time << ", " << x.num_transactions
                << ", " << x.part_of_longest
                << ", " << x.generated_by_node << endl;
        }
        file.close();
    }
}

bool block_stats::operator<(const block_stats& other) const
{
    if (first_seen_time == other.first_seen_time) return block_id < other.block_id;
    return first_seen_time < other.first_seen_time;
}


block_stats::block_stats()
{
    block_id = -1;
    parent_block_id = -1;
    first_seen_time = -1;
    num_transactions = -1;
    part_of_longest = false;
    generated_by_node = false;
}

