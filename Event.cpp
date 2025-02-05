#include "Event.h"

bool Event::operator >(const Event& other) const
{
    if (time == other.time) return type > other.type;
    else return time > other.time;
}

create_transaction_object::create_transaction_object(const int creator_node_id)
{
    this->creator_node_id = creator_node_id;
}

receive_transaction_object::receive_transaction_object(const int sender_node_id, const int receiver_node_id,
                                                       const shared_ptr<Transaction>& txn)
{
    this->sender_node_id = sender_node_id;
    this->receiver_node_id = receiver_node_id;
    this->txn = txn;
}

receive_block_object::receive_block_object(const int sender_node_id, const int receiver_node_id, const shared_ptr<Block>& blk)
{
    this->sender_node_id = sender_node_id;
    this->receiver_node_id = receiver_node_id;
    this->blk = blk;
}

block_mined_object::block_mined_object(const int miner_node_id, const shared_ptr<Block>& blk)
{
    this->miner_node_id = miner_node_id;
    this->blk = blk;
}

Event::Event(const long long time, const int type, VO object): time(time), type(type), object(std::move(object))
{
}
