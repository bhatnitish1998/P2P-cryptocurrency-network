#include "Network.h"
#include "utility_functions.h"


int Node::node_ticket = 0;

Link::Link(int peer, int propagation_delay, long long link_speed)
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
}

void Node::create_transaction()
{
    int receiver = uniform_distribution(0, number_of_nodes - 1);
    int amount = uniform_distribution(transaction_amount_min, transaction_amount_max);

    auto *t = new Transaction(receiver,amount,false,id);
    mempool.push(t);

    // send to all
    for (Link& x: peers)
        send_transaction_to_link(t,x);
}

void Node::send_transaction_to_link(Transaction* txn, Link& link) const
{
    long long latency = link.propagation_delay + (1 * 1024 * 8)/link.link_speed + exponential_distribution((queuing_delay_constant)/link.link_speed);
    link.transactions_sent.insert(txn->id);

    struct receive_transaction_object obj(id,link.peer,txn);

    Event e(simulation_time + latency,RECEIVE_TRANSACTION,obj);
    event_queue.push(e);
}

void Node::receive_transaction(receive_transaction_object &obj)
{
    Transaction * txn = obj.txn;
    mempool.push(txn);

    for (Link &x: peers)
    {
        if (x.peer != obj.sender_node_id && x.transactions_sent.count(txn->id)==0)
        {
            send_transaction_to_link(txn,x);
            return;
        }
    }
}

void Node::receive_block(Block* blk)
{
    if (block_ids_in_tree.count(blk->id) == 1)
        return;

    if (!validate_block(blk))
        return;
    broadcast_block(blk);
    if (add_block_to_tree(blk))
        mine_block();
}

bool Node::validate_block(Block* blk)
{
    vector<long long> balance(number_of_nodes,0);

    Block *temp = blk;
    while (temp->parent_block != nullptr)
    {
        // TODO 7: Go through all the transactions in each block and update the balances in the above vector
        temp = temp->parent_block;
    }

    for (auto x: blk->transactions)
    {
        // TODO 7: Subtract and add balances here making sure they dont ever go negative.
    }

    return true;
}

bool Node::add_block_to_tree(Block* blk)
{
    block_ids_in_tree.insert(blk->id);
    long long previous_longest = leaves.begin()->block->id;
    for (auto x: leaves)
    {
        if (x.block->id == blk->parent_block->id)
        {
            long long curr_length = x.length;
            auto it = leaves.find(x);
            LeafNode l(blk,curr_length+1);
            l.transaction_ids.insert(it->transaction_ids.begin(),it->transaction_ids.end());
            for (auto t: blk->transactions)
                l.transaction_ids.insert(t->id);
            leaves.erase(it);
            leaves.insert(l);
            return previous_longest != leaves.begin()->block->id;
        }
    }

    long long length = 1;
    Block *temp = blk;
    set<long long > t_ids;
    while (temp->parent_block!=nullptr)
    {
        for (auto t: temp->transactions)
            t_ids.insert(t->id);
        temp = temp->parent_block;
        length++;
    }
    LeafNode l(blk,length);
    l.transaction_ids= t_ids;
    leaves.insert(l);

    return previous_longest != leaves.begin()->block->id;
}

void Node::mine_block()
{
    auto it = leaves.begin();
    Block *longest = it->block;
    auto * blk = new Block(simulation_time,longest);
    auto * txn = new Transaction(id,50,true);
    blk->transactions.push_back(txn);

    while (blk->transactions.size()< 1000 && !mempool.empty())
    {
        Transaction *t = mempool.front(); mempool.pop();
        if (it->transaction_ids.count(t->id) == 0)
            blk->transactions.push_back(t);
    }

    double hashing_fraction = (high_cpu? 10.0 : 1.0)/total_hashing_power;
    long long mining_time = exponential_distribution(block_inter_arrival_time/hashing_fraction);


    block_mined_object obj(id,blk);
    event_queue.emplace(simulation_time + mining_time,BLOCK_MINED, obj);
}

void Node::complete_mining(Block *blk)
{
    Block *longest = leaves.begin()->block;
    if (blk->parent_block->id == longest->id)
    {
        add_block_to_tree(blk);
        broadcast_block(blk);
        mine_block();
    }
}

void Node::broadcast_block(Block* blk)
{
    long long size = 1 * 1024 * 8 * blk->transactions.size();

    for (auto link : peers)
    {
        if (link.blocks_sent.count(blk->id) == 0)
        {
            link.blocks_sent.insert(blk->id);
            long long latency = link.propagation_delay + size/link.link_speed + exponential_distribution((double)(queuing_delay_constant)/link.link_speed);
            receive_block_object obj(link.peer,blk);
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
    {
        nodes[node_id].fast = true;
    }

    //   Make ( n * percent_high_cpu) high_cpu nodes
    vector<int> high_cpu_nodes = choose_percent(number_of_nodes, percent_high_cpu / 100.0);
    total_hashing_power += high_cpu_nodes.size() * 10 + (number_of_nodes- high_cpu_nodes.size());
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
                int link_speed = nodes[i].fast && nodes[x].fast ? 100 * 1000 : 5 * 1000; // bits per millisecond
                nodes[i].peers.emplace_back(x, propagation_delay, link_speed);
                nodes[x].peers.emplace_back(i, propagation_delay, link_speed);
            }
        }
    }
}
