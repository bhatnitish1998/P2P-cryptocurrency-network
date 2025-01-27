#include "Blockchain.h"

long long Transaction::transaction_ticket = 0;
long long Block::block_ticket = 0;

Transaction::Transaction(int receiver, double amount, bool coinbase, int sender)
{
    id = transaction_ticket++;
    this->receiver = receiver;
    this->amount = amount;
    this->coinbase = coinbase;
    this->sender = sender;

    if (coinbase && sender != -1) throw invalid_argument("sender present for coinbase transaction");
}

Block::Block(double creation_time)
{
    id = block_ticket++;
    parent_block = nullptr;
    this->creation_time = creation_time;
}

LeafNode::LeafNode(Block* block, long length)
{
    this->block = block;
    this->length = length;
}

ostream& operator<<(ostream& os, const Transaction& txn)
{
    if (txn.coinbase)
        os << txn.id << ": " << txn.receiver << " mines " << txn.amount << " coins";
    else
        os << txn.id << ": " << txn.sender << " pays " << txn.receiver << " " << txn.amount << " coins";
    return os;
}

ostream& operator<<(ostream& os, const Block& block)
{
    os << "Block id: " << block.id << " Txns: " << block.transactions.size() << endl;
    for (Transaction txn : block.transactions)
        os << txn << endl;
    return os;
}
