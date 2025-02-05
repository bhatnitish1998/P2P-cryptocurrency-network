#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <set>
#include <map>
#include <queue>
#include "Blockchain.h"
#include "Event.h"

using namespace std;

extern int transaction_amount_min;
extern int transaction_amount_max;
extern int queuing_delay_constant;
extern int percent_fast;
extern int percent_high_cpu;
extern int propagation_delay_min;
extern int propagation_delay_max;;
extern long long simulation_time;
extern long long total_hashing_power;
extern int block_inter_arrival_time;
extern int transaction_size;
extern int mining_reward;


extern EQ event_queue;


// Link between two nodes
class Link
{
public:
  int peer; // peer node id
  int propagation_delay;
  long long link_speed;

  // keeps track of transactions and blocks sent to avoid loops
  set<long long> transactions_sent;
  set<long long> blocks_sent;

  Link(int peer, int propagation_delay, long long link_speed);
};

class Node
{
private:
  // to generate unique node ids (equal to index in node vector)
  static int node_ticket;
  // send transaction to particular link
  void send_transaction_to_link(const shared_ptr<Transaction>& txn, Link &link) const;

public:
  int id;
  bool fast;
  bool high_cpu;
  vector<Link> peers; // stores links to all its peers
  shared_ptr<Block> genesis; // genesis block pointer
  set<shared_ptr<LeafNode>,CompareLeafNodePtr> leaves; // stores information about all leaf nodes of blockchain tree
  queue<shared_ptr<Transaction>> mempool;
  map<long long, long long> block_ids_in_tree; // stores received blocks <block id, time first seen>

  Node();

  // creates a random transaction and broadcasts it to its peers
  void create_transaction();

  //  receive a transaction from peer
  void receive_transaction(const receive_transaction_object &obj);

  // receive a block from peer
  void receive_block(const receive_block_object &obj);

  // true: block added to the longest chain
  // false: validation failed or block added to some forked branch
  bool validate_and_add_block(shared_ptr<Block> blk);

  // Prepare block and start mining
  void mine_block();

  // add mined block to tree if longest not changed
  void complete_mining(const shared_ptr<Block>& blk);

  // broadcast block to all peers
  void broadcast_block(const shared_ptr<Block>& blk);
};

class Network
{
public:
  vector<Node> nodes;

  Network();
};

#endif //NETWORK_H
