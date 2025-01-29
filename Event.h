#ifndef EVENT_H
#define EVENT_H

#define CREATE_TRANSACTION 0
#define RECEIVE_TRANSACTION 1
#define RECEIVE_BLOCK 2
#define BLOCK_MINED 3

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

struct block_mined_object
{
    int miner_node_id;
    Block * blk;
    explicit block_mined_object(int miner_node_id,Block *blk);
};

typedef     variant <create_transaction_object, receive_transaction_object,receive_block_object,block_mined_object> VO;

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
