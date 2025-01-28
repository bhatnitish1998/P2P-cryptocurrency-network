//
// Created by nitish on 1/28/25.
//

#include "Event.h"

bool Event::operator >(const Event& other) const
{
    if (time == other.time) return type > other.type;
    else return time > other.time;
}

create_transaction_object::create_transaction_object(int creator_node_id)
{
    this->creator_node_id = creator_node_id;
}

receive_transaction_object::receive_transaction_object(int sender_node_id, int receiver_node_id, Transaction* txn)
{
    this->sender_node_id = sender_node_id;
    this->receiver_node_id = receiver_node_id;
    this->txn = txn;
}

Event::Event(long long time, int type, VO obj): time(time), type(type), object(obj){}
