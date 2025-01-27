#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
#include <ostream>
#include <iostream>
#include <bits/fs_fwd.h>

using namespace std;

class Transaction
{
    static long long transaction_ticket;

public:
    long long id;
    int receiver;
    double amount;
    bool coinbase;
    int sender;

    Transaction(int receiver, double amount, bool coinbase, int sender = -1);
    friend ostream& operator<<(ostream& os, const Transaction& txn);
};

class Block
{
    static long long block_ticket;

public:
    long long id;
    Block* parent_block;
    vector<Transaction> transactions;
    double creation_time;


    explicit Block(double creation_time);
    friend ostream& operator<<(ostream& os, const Block& block);
};

class LeafNode
{
public:
    Block* block;
    long length;

    LeafNode(Block* block, long length);
};

#endif //BLOCKCHAIN_H
