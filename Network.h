#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <list>
#include "Blockchain.h"

using namespace std;

extern int propagation_delay_min;
extern int propagation_delay_max;

class Link
{
public:
  int peer;
  int propagation_delay; // in milliseconds
  int link_speed; // in Mbps

  vector<int> transactions_sent;

  Link(int peer, int propagation_delay, int link_speed);
};

class Node
{
private:
  static int node_ticket;

public:
  int id;
  bool fast;
  bool high_cpu;
  vector<Link> peers;
  Block* genesis;
  list<LeafNode> leaves;

  Node();
};

class Network
{
public:
  int number_of_nodes;
  vector<Node> nodes;

  Network(int number_of_nodes, double percent_fast, double percent_high_cpu);
};

#endif //NETWORK_H
