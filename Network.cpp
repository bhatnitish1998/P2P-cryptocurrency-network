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

    add_block_to_tree(blk);
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

void Node::add_block_to_tree(Block* blk)
{
    block_ids_in_tree.insert(blk->id);

    for (auto x: leaves)
    {
        if (x.block->id == blk->parent_block->id)
        {
            long long curr_length = x.length;
            auto it = leaves.find(x);
            leaves.erase(it);
            leaves.insert({blk,curr_length+1});
            return;
        }
    }

    long long length = 1;
    Block *temp = blk;
    while (temp->parent_block!=nullptr)
    {
        temp = temp->parent_block;
        length++;
    }
    leaves.insert({blk,length});
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
