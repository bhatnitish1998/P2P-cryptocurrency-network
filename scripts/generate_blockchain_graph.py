import matplotlib.pyplot as plt
import networkx as nx
from collections import defaultdict,deque
import os

def initialize_path():
    input_filepath = '../files/blockchain.txt'  
    output_folder = '../output'
    return input_filepath,output_folder


def load_blockchain_graph_from_file(filepath):
    graph = nx.DiGraph()
    # print(graph)
    edges = []  #
    with open(filepath, 'r') as f:
        lines = f.readlines()
    
    # Locate Blockchain section
    start_index = None
    for i, line in enumerate(lines):
        if line.strip() == "Blockchain:":
            # node creating data present at start_index +2 (skipping headers)
            start_index = i + 2  
            break

    if start_index is None:
        raise ValueError("Blockchain section not found in the file")

    # Process the blockchain node data
    for line in lines[start_index:]:
        parts = line.strip().split(',')
        if len(parts) < 6:
            continue  # Skip invalid lines
        block_id = int(parts[0].strip())
        parent_id = int(parts[1].strip())
        if parent_id != -1:
            edges.append((parent_id, block_id))

    graph.add_edges_from(edges)
    
    return graph

def assign_positions(graph, root, dx=4.0, dy=3.0):
    """Assigns x, y positions using BFS."""
    pos = {}  # Store node positions
    level_nodes = defaultdict(list)  # Nodes at each level
    queue = deque([(root, 0)])  # (node, level)
    visited = set()

    # BFS to determine levels
    while queue:
        node, level = queue.popleft()
        if node in visited:
            continue
        visited.add(node)
        level_nodes[level].append(node)

        for neighbor in graph.successors(node):
            if neighbor not in visited:
                queue.append((neighbor, level + 1))

    # Assign positions
    for level, nodes in level_nodes.items():
        num_nodes = len(nodes)
        start_y = -((num_nodes - 1) * dy) / 2  # Center nodes vertically
        for i, node in enumerate(nodes):
            pos[node] = (level * dx, start_y + i * dy)

    return pos

def plot_blockchain_graph(graph,node_colors,directory):
    """
    plots the blocks chain graph based on position.
    """
    pos = assign_positions(graph, root=0, dx=2.0, dy=1.5)
    num_nodes = len(graph.nodes) 
    
    # Scale width based on #nodes
    width = max(10, num_nodes * 1.05)  
    plt.figure(figsize=(width, 10))  
    nx.draw(graph,pos=pos,edge_color='black',node_color=node_colors,node_size=1000,with_labels=True,node_shape='s')

    # Display the graph
    plt.title("block chain visualization")
    
    # create targte directory if not exists
    os.makedirs(directory, exist_ok=True)
    
    # Save the figure as a PNG file within the given directory
    output_file = os.path.join(directory, 'blockchain_graph.png')
    plt.savefig(output_file, format='png')
    plt.close()

if __name__ == "__main__":
    input_filepath,output_folder=initialize_path()
    blockchain_graph = load_blockchain_graph_from_file(input_filepath)
    # print("Edges:", list(blockchain_graph)) 
    node_colors = ['lightblue' if blockchain_graph.degree(node) >= 2 else 'lightgreen' for node in blockchain_graph.nodes()]
    plot_blockchain_graph(blockchain_graph,node_colors,output_folder)

