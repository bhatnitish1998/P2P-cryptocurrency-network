#include "Network.h"

#include <algorithm>

#include "utility_functions.h"

int Node::node_ticket = 0;

Link::Link(const int peer, const int propagation_delay, const long long link_speed)
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
    genesis = nullptr;
    currently_mining = false;
}

void Node::create_transaction()
{
    // randomly choose amount and receiver
    const int receiver = uniform_distribution(0, number_of_nodes - 1);
    const int amount = uniform_distribution(transaction_amount_min, transaction_amount_max);

    const auto t = make_shared<Transaction>(receiver,amount,false,id);
    mempool.push(t);

    // send to all peers
    for (Link& x: peers)
        send_transaction_to_link(t,x);

    // if it was first transaction in mempool start mining
    if (mempool.size() == 1 && !currently_mining) mine_block();
}


void Node::send_transaction_to_link(const shared_ptr<Transaction>& txn, Link& link) const
{
    // Compute link latency
    const long long latency = link.propagation_delay + transaction_size/link.link_speed + \
    exponential_distribution(static_cast<double>(queuing_delay_constant)/static_cast<double>(link.link_speed));

    // send transaction by creating receive transaction event for recipient
    link.transactions_sent.insert(txn->id);
    receive_transaction_object obj(id,link.peer,txn);
    event_queue.emplace(simulation_time + latency,RECEIVE_TRANSACTION,obj);
}

void Node::receive_transaction(const receive_transaction_object& obj)
{
    // add transaction to the mempool
    mempool.push(obj.txn);

    // send it to the first unsent peer
    for (Link& x : peers)
    {
        if (x.peer != obj.sender_node_id && x.transactions_sent.count(obj.txn->id) == 0)
        {
            send_transaction_to_link(obj.txn, x);
            return;
        }
    }

    // if it was first transaction in mempool start mining
    if (mempool.size() == 1 && !currently_mining) mine_block();
}

void Node::receive_block(const receive_block_object& obj)
{
    // check if block already present
    if (block_ids_in_tree.count(obj.blk->id) == 1)
        return;

    // if block received before parent
    if (block_ids_in_tree.count(obj.blk->parent_block->id) == 0)
    {
        // simulate resending the block by the peer (assume ACK, NACK mechanism)
        for (const auto& link: peers)
        {
            if (link.peer == obj.sender_node_id)
            {
                const long long size = transaction_size * static_cast<long long>(obj.blk->transactions.size());
                const long long latency = link.propagation_delay + size/link.link_speed + \
                exponential_distribution(static_cast<double>(queuing_delay_constant)/static_cast<double>(link.link_speed));
                Event e(simulation_time + latency,RECEIVE_BLOCK,receive_block_object(obj.sender_node_id,obj.receiver_node_id,obj.blk));
                event_queue.push(e);
            }
        }
        return;
    }
    // if validated and added to the longest chain, re-start mining on longest chain
    if (validate_and_add_block(obj.blk))
        mine_block();
}

bool Node::validate_and_add_block(shared_ptr<Block> blk)
{
    // check if the parent node of the block is leaf node
    const auto it = find_if(leaves.begin(),leaves.end(),\
        [&blk](const shared_ptr<LeafNode>& leaf){return blk->parent_block->id == leaf->block->id;});
    vector<long long > temp_balance;
    set<long long > temp_transaction_ids;
    long long temp_length =1;

    // if parent not a leaf
    if (it == leaves.end())
    {
        temp_balance.resize(number_of_nodes,0);

        // traverse the chain till genesis and get balance and transaction ids
        auto temp_ptr = blk->parent_block;
        while (temp_ptr)
        {
            for (const auto& txn: temp_ptr->transactions)
            {
                temp_transaction_ids.insert(txn->id);

                if (txn->coinbase) temp_balance[txn->receiver]+=txn->amount;
                else
                {
                    temp_balance[txn->sender]-=txn->amount;
                    temp_balance[txn->receiver]+=txn->amount;
                }
            }
            temp_ptr = temp_ptr->parent_block;
            temp_length++;
        }
    }
    // if parent is a leaf get it from leaf node
    else
    {
        temp_balance = (*it)->balance;
        temp_transaction_ids = (*it)->transaction_ids;
        temp_length = (*it)->length+1;
    }


    // validate the block
    for (const auto& txn: blk->transactions)
    {
        temp_transaction_ids.insert(txn->id);
        if (txn->coinbase) temp_balance[txn->receiver]+=txn->amount;
        else
        {
            temp_balance[txn->sender]-= txn->amount;
            // if balance -ve invalid transaction, abort
            if ( temp_balance[txn->sender] < 0 )
                return false;
            temp_balance[txn->receiver]+= txn->amount;
        }
    }

    // if validated broadcast block and insert into tree.
    broadcast_block(blk);
    block_ids_in_tree.insert({blk->id,simulation_time});

    // Update the leaf nodes
    const auto temp_leaf = make_shared<LeafNode>(blk,temp_length);
    temp_leaf->balance= std::move(temp_balance);
    temp_leaf->transaction_ids = std::move(temp_transaction_ids);

    // return true if longest changes after inserting
    const long long previous_longest = (*leaves.begin())->block->id;
    if (it != leaves.end())
        leaves.erase(it);
    leaves.insert(temp_leaf);
    const long long current_longest = (*leaves.begin())->block->id;
    return (previous_longest != current_longest);
}


void Node::mine_block()
{
    if (mempool.empty())
    {
        currently_mining = false;
        return;
    }
    // create the new block with coinbase transaction
    shared_ptr<LeafNode> longest_leaf = *leaves.begin();
    auto blk = make_shared<Block>(simulation_time,longest_leaf->block);
    blk->transactions.push_back(make_shared<Transaction>(id,mining_reward,true));
    vector<long long > temp_balance = longest_leaf->balance;

    // populate block with valid transactions from mempool
    while (blk->transactions.size()< 1000 && !mempool.empty())
    {
        auto txn = mempool.front(); mempool.pop();
        if (longest_leaf->transaction_ids.count(txn->id) == 0)
        {
            if (txn->coinbase) temp_balance[txn->receiver]+=txn->amount;
            else
            {
                if ( temp_balance[txn->sender] - txn->amount < 0 )
                    continue;

                temp_balance[txn->sender]-= txn->amount;
                temp_balance[txn->receiver]+= txn->amount;
            }
        }
    }

    // compute mining time and create event at that time
    const double hashing_fraction = (high_cpu? 10.0 : 1.0)/static_cast<double>(total_hashing_power);
    const long long mining_time = exponential_distribution(block_inter_arrival_time/hashing_fraction);
    block_mined_object obj(id,blk);
    event_queue.emplace(simulation_time + mining_time,BLOCK_MINED, obj);
}


void Node::complete_mining(const shared_ptr<Block>&  blk)
{
    shared_ptr<LeafNode> longest_leaf = *leaves.begin();
    // if longest chain has not changed while mining
    if (blk->parent_block->id == longest_leaf->block->id)
    {
        // validation always succeeds
        validate_and_add_block(blk);
        // start mining next block
        mine_block();
    }

    // if failed return transactions to the mempool
    else
    {
        for (const auto& txn: blk->transactions)
            mempool.push(txn);
    }
}

void Node::broadcast_block(const shared_ptr<Block>& blk)
{
    const long long size = (transaction_size) * static_cast<long long>(blk->transactions.size());

    for (auto link : peers)
    {
        // send block if not already sent to the peer
        if (link.blocks_sent.count(blk->id) == 0)
        {
            link.blocks_sent.insert(blk->id);
            const long long latency = link.propagation_delay + size/link.link_speed + \
            exponential_distribution(static_cast<double>(queuing_delay_constant)/static_cast<double>(link.link_speed));

            // create receive block event for that node at current time + latency
            receive_block_object obj(id,link.peer,blk);
            Event e(simulation_time + latency,RECEIVE_BLOCK,obj);
            event_queue.push(e);
        }
    }
}


Network::Network()
{
    // Node id equal to its index in vector
    nodes.resize(number_of_nodes);

    //   Make ( n * percent_fast) fast nodes
    vector<int> fast_nodes = choose_percent(number_of_nodes, percent_fast / 100.0);
    for (const int node_id : fast_nodes)
        nodes[node_id].fast = true;

    //   Make ( n * percent_high_cpu) high_cpu nodes
    vector<int> high_cpu_nodes = choose_percent(number_of_nodes, percent_high_cpu / 100.0);
    total_hashing_power += static_cast<long long>(high_cpu_nodes.size()) * 10 + \
    (number_of_nodes- static_cast<long long>(high_cpu_nodes.size()));

    for (const int node_id : high_cpu_nodes)
        nodes[node_id].high_cpu = true;

    bool done = false;
    vector<vector<int>> al(number_of_nodes);

    // Until graph is connected
    while (!done)
    {
        al.clear();
        for (int i = 0; i < number_of_nodes; i++)
        {
            int min_peers = min(3,number_of_nodes-1);
            // until at least connected to min peers keep adding neighbors (reverse addition leads to > 3 but < 6 links)
            while (al[i].size() < min_peers)
            {
                vector<int> temp = choose_neighbours(number_of_nodes, min_peers - static_cast<int>(al[i].size()), al[i]);
                for (auto neighbour : temp)
                {
                    // ensure max 6 peers
                    if (neighbour != i && al[neighbour].size() < min(6,number_of_nodes-1))
                    {
                        al[i].push_back(neighbour);
                        al[neighbour].push_back(i);
                    }
                }
            }
        }
        done = check_connected(al);
    }

    // set up link speed and propagation delay for each peer
    for (int i = 0; i < number_of_nodes; i++)
    {
        for (auto x : al[i])
        {
            if (i < x)
            {
                int propagation_delay = uniform_distribution(propagation_delay_min, propagation_delay_max);
                int link_speed = nodes[i].fast && nodes[x].fast ? 100 * 1000 : 5 * 1000; // bits per millisecond
                nodes[i].peers.emplace_back(x, propagation_delay, link_speed);
                nodes[x].peers.emplace_back(i, propagation_delay, link_speed);
            }
        }
    }
}
