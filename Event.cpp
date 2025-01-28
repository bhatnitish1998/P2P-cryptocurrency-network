//
// Created by nitish on 1/28/25.
//

#include "Event.h"

bool Event::operator >(const Event& other) const
{
    if (time == other.time) return type > other.type;
    else return time > other.time;
}

Event::Event(long long time, int type, VO object)
{
    this->time = time;
    this->type = type;
    this->object = object;
}