#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <list>
#include <set>
#include <queue>
#include "Blockchain.h"
#include "Event.h"

using namespace std;

extern int number_of_nodes;
extern int transaction_amount_min;
extern int transaction_amount_max;
extern int queuing_delay_constant;
extern int percent_fast;
extern int percent_high_cpu;
extern int propagation_delay_min;
extern int propagation_delay_max;;
extern long long simulation_time;

extern EQ event_queue;


class Link
{
public:
  int peer;
  int propagation_delay;
  long long link_speed;

  set<long long> transactions_sent;

  Link(int peer, int propagation_delay, long long link_speed);
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
  set<LeafNode,greater<>> leaves;
  queue <Transaction *> mempool;
  set<long long> block_ids_in_tree;

  Node();
  void create_transaction();
  void send_transaction_to_link(Transaction * txn, Link & link) const;
  void receive_transaction(receive_transaction_object & obj);
  void receive_block(Block * blk);
  static bool validate_block(Block * blk);
  void add_block_to_tree(Block * blk);
};

class Network
{
public:
  vector<Node> nodes;

  Network();
};

#endif //NETWORK_H
