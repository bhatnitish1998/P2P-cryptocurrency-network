#ifndef EVENT_H
#define EVENT_H

#define CREATE_TRANSACTION 0 // variable contains creater node id;
#define RECEIVE_TRANSACTION 1 // variable contains transaction pointer;


#include <variant>
#include <queue>
#include "Blockchain.h"

struct create_transaction_object
{
    int creator_node_id;
    explicit create_transaction_object(int creator_node_id);

};

struct receive_transaction_object
{
    int sender_node_id;
    int receiver_node_id;
    Transaction * txn;

    receive_transaction_object(int sender_node_id, int receiver_node_id, Transaction * txn);
};

struct receive_block_object
{
    int receiver_node_id;
    Block *blk;
    receive_block_object(int receiver_node_id, Block *blk);
};

typedef     variant <create_transaction_object, receive_transaction_object> VO;

class Event
{
public:
    long long time;
    int type;
    VO object;

    Event(long long time, int type, VO object);
    bool operator > (const Event &other) const;
};

typedef priority_queue<Event, vector<Event>, greater<>>  EQ;


#endif //EVENT_H
