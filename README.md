# P2P Cryptocurrency Network

## Introduction
This project is an introduction to blockchain and cryptocurrencies, implementing a peer-to-peer (P2P) network for simulating blockchain transactions and mining.

## Compilation and Execution
Compiling Code: g++ *.cpp -o main  
Running program: ./main <number_of_nodes> <percent_fast> <percent_high_cpu> <mean_transaction_inter_arrival_time_in_ms> <block_inter_arrival_time_in_sec>  

Example:./main 10 4 4 100 1   

## Changing configuration
Modify experimental parameters in main.cpp under the // experiment constants comment section. Key variables include:  

initial_bitcoin  
initial_number_of_transactions  
propagation_delay_min  
propagation_delay_max  
transaction_amount_min  
transaction_amount_max  
queuing_delay_constant  
transaction_size  
mining_reward  

## Output Data

After the program completes u will see files folder which contains stats related to all nodes with all details.  
Eg.Open Node_18.txt    
Fast node: 0  
High_cpu: 0  
Peers:  
	 Node id: 3 Propagation delay: 330 ms Link speed: 5000  
	 Node id: 5 Propagation delay: 351 ms Link speed: 5000  
	 Node id: 9 Propagation delay: 350 ms Link speed: 5000  
Transactions received: 2  
Blocks received: 20  
Blocks mined: 1  
Blocks mined in longest chain: 1  
Number of blocks in longest chain: 21  
Fraction of blocks mined in longest chain: 0.047619  
Number of forks: 33  
Longest fork length:3  
Shortest fork length:1  
Average fork length: 1  

Entire blockchain detail  


## Visualization Scripts
### Prerequisites
Extract scripts.zip into the project root directory  

Directory structure should be:  
P2P-CRYPTOCURRENCY-NETWORK/  
├── files/  
└── scripts/  

Set up virtual environment:  

In directory P2P-CRYPTOCURRENCY-NETWORK run:  
python3 -m venv myenv  
source myenv/bin/activate  
cd scripts/  
pip3 install -r requirements.txt  

## Network Visualization
Run python3 generate_p2p_network_graph.py to create p2p network graph.

## Blockchain Visualization
Run python3 generate_blockchain_graph.py to create block chain graph.

Outputs will be stored in output folder.

