// time : long long : milliseconds
// data : long long : bits
// amount : long long : bitcoin

#include "Network.h"
#include <cstdlib>
#include "Simulator.h"
#include "Event.h"

// experiment constants
int initial_bitcoin = 100;
int initial_number_of_transactions = 1000;
int propagation_delay_min = 10 ;
int propagation_delay_max = 500;
int transaction_amount_min = 5;
int transaction_amount_max = 20;
int queuing_delay_constant = 96000;

// experiment parameters
int number_of_nodes;
int percent_fast;
int percent_high_cpu;
int mean_transaction_inter_arrival_time;
int block_inter_arrival_time;

// Simulation variables
long long simulation_time =0;
EQ event_queue;
long long total_hashing_power=0;

int main()
{
    // Simulation parameters
    // TODO 1: Ensure number_of_nodes at least 6;
    number_of_nodes = 20;
    percent_fast = 40;
    percent_high_cpu = 25;
    mean_transaction_inter_arrival_time = 5000;
    block_inter_arrival_time = 600 * 1000;
    // TODO 1: take the parameters from command line arguments

    // variables needed for random number generation
    unsigned int global_seed = 911;
    srand(global_seed);

    Simulator sim;
    sim.initialize();
    sim.start();
}
