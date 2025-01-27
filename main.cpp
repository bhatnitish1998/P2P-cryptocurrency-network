#include "Network.h"
#include <cstdlib>
#include "Simulator.h"

int initial_bitcoin = 50;
int initial_number_of_transactions = 100;
int propagation_delay_min = 10 ; // milliseconds
int propagation_delay_max = 500; // milliseconds
int transaction_amount_min = 5;
int transaction_amount_max = 20;

int main()
{
    // Simulation parameters
    // TODO 1: Ensure number_of_nodes at least 6;
    int number_of_nodes = 20;
    double percent_fast = 25;
    double percent_high_cpu = 40;
    double mean_transaction_inter_arrival_time = 5; // seconds
    double block_inter_arrival_time = 600; // seconds
    double queuing_delay_constant = 96; // Kbits
    // TODO 1: take the parameters from command line arguments

    // variables needed for random number generation
    unsigned int global_seed = 911;
    srand(global_seed);

    Simulator sim(number_of_nodes,percent_fast,percent_high_cpu,mean_transaction_inter_arrival_time,block_inter_arrival_time,queuing_delay_constant);
    sim.initialize();
}
