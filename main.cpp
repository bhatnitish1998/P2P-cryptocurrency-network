#include "Network.h"

int main()
{
    // Simulation parameters

    int number_of_nodes = 10;
    double percent_fast = 25;
    double percent_high_cpu = 40;
    double transaction_inter_arrival_time = 5; // seconds
    double block_inter_arrival_time = 600; // seconds
    double queuing_delay_constant = 96; // Kbits

    // TODO 1: take the parameters from command line arguments

    Network network(number_of_nodes,percent_fast,percent_high_cpu);

}