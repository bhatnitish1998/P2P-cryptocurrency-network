#ifndef EVENT_H
#define EVENT_H

#define CREATE_TRANSACTION 0 // variable contains creater node id;
#define RECEIVE_TRANSACTION 1 // variable contains transaction pointer;


#include <variant>
#include <queue>
#include "Blockchain.h"

typedef     variant <int,Transaction*,Block*> VO;

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
