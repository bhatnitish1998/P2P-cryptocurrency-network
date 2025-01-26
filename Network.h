#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
using namespace std;

class Link
{
  public:
  int peer;
  int propagation_delay; // in milliseconds
  int link_speed; // in Mbps

  vector<int> transactions_sent;

  Link(int peer, int propagation_delay,int link_speed);
};

class Node{
  private:
  static int node_ticket;
  public:
    int id;
    bool fast;
    bool high_cpu;
    vector<Link> peers;

    Node();
};

class Network{
  public:
  int n;
  vector<Node> nodes;

  Network(int n, double percent_fast, double percent_high_cpu);
};

#endif //NETWORK_H
