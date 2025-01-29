#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
#include <ostream>
#include <iostream>
#include <bits/fs_fwd.h>
#include <set>

using namespace std;

class Transaction
{
    static long long transaction_ticket;

public:
    long long id;
    int receiver;
    long long amount;
    bool coinbase;
    int sender;

    Transaction(int receiver, int amount, bool coinbase, int sender = -1);
    friend ostream& operator<<(ostream& os, const Transaction& txn);
};

class Block
{
    static long long block_ticket;

public:
    long long id;
    Block* parent_block;
    vector<Transaction *> transactions;
    long long creation_time;

    Block(long long creation_time, Block* parent_block);
    friend ostream& operator<<(ostream& os, const Block& block);
};

class LeafNode
{
public:
    Block* block;
    long long length;
    set<long long> transaction_ids;

    LeafNode(Block* block, long long length);
    bool operator > (const LeafNode &other) const;
    bool operator < (const LeafNode &other) const;
};

#endif //BLOCKCHAIN_H
