#ifndef SIMULATOR_H
#define SIMULATOR_H

#define CREATE_TRANSACTION 0 // void pointer contains creater node id;


// global configuration parameters
extern int initial_bitcoin;
extern int initial_number_of_transactions;
extern int transaction_amount_min;
extern int transaction_amount_max;


#include <iostream>
#include <queue>
#include "Network.h"
#include "utility_functions.h"
#include "Blockchain.h"
#include <variant>

using namespace std;

class Event
{
    public:
    double time;
    int type;
    variant <int,Transaction*,Block*> object;

    Event(double time, int type, variant <int,Transaction*,Block*> object);

    bool operator > (const Event &other) const;
};

class Simulator
{
    void create_genesis();
    void create_initial_transactions();

public:
    double simulation_time; // in milliseconds

    int number_of_nodes;
    double percent_fast;
    double percent_high_cpu;
    double mean_transaction_inter_arrival_time; // seconds
    double block_inter_arrival_time; // seconds
    double queuing_delay_constant; // Kbits


    Network nw;
    priority_queue<Event, vector<Event>, greater<>> event_queue;

    Simulator(int number_of_nodes, double percent_fast, double percent_high_cpu, double mean_transaction_inter_arrival_time, double block_inter_arrival_time, double queuing_delay_constant);

    void initialize();
};
#endif //SIMULATOR_H
