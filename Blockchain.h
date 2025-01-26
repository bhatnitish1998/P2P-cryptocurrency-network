//
// Created by nitish on 1/26/25.
//

#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
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

    // TODO 5: Override output operator to print transaction
    // TxnID: IDx pays IDy C coins , TxnID:IDk mines 50 coins
};

class Block
{
    static long long block_ticket;
    public:
    long long id;
    Block* parent_block;
    vector <Transaction> transactions;
    Block();
};

class LeafNode
{
    public:
    Block* block;
    long length;

    LeafNode(Block* block, long length);

};

#endif //BLOCKCHAIN_H
