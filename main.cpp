#include "Network.h"
#include <cstdlib>


int main()
{
    // Simulation parameters

    // TODO 1: Ensure number_of_nodes at least 6;
    int number_of_nodes = 20;
    double percent_fast = 25;
    double percent_high_cpu = 40;
    double transaction_inter_arrival_time = 5; // seconds
    double block_inter_arrival_time = 600; // seconds
    double queuing_delay_constant = 96; // Kbits
    unsigned int global_seed = 911;
    srand(global_seed);

    // TODO 1: take the parameters from command line arguments

    // Create the network of nodes
    Network network(number_of_nodes, percent_fast, percent_high_cpu);

    // Initialize each node with genesis block
    network.create_genesis();
}
