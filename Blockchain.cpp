//
// Created by nitish on 1/26/25.
//

#include "Blockchain.h"

long long Transaction::transaction_ticket =0;
long long Block::block_ticket =0;

Transaction::Transaction(int receiver, double amount, bool coinbase, int sender)
{
    id = transaction_ticket++;
    this->receiver = receiver;
    this->amount = amount;
    this->coinbase = coinbase;
    this->sender = sender;
}

Block::Block()
{
    id = block_ticket++;
    parent_block = nullptr;
}

LeafNode::LeafNode(Block* block, long length)
{
    this->block = block;
    this->length = length;
}