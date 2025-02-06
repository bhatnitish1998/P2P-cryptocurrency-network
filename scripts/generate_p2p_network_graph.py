import matplotlib.pyplot as plt
import networkx as nx
import os

def initialize_path():
    input_filepath = '../files/network.txt'  
    output_folder = '../output'
    return input_filepath,output_folder

def load_network_graph_from_file(filepath):
    graph = nx.Graph()
    with open(filepath, 'r') as f:
        for line in f:
            node_data = list(map(int, line.strip().split()))
            source_node = node_data[0]
            target_node = node_data[1]
            graph.add_edge(source_node, target_node)
    return graph

def plot_network_graph(graph, directory):
    layout = nx.spring_layout(graph)
    num_nodes = len(graph.nodes)  
    # Scale size based on nodes
    size = max(10, num_nodes * 1.05)  
    # print(size)
    plt.figure(figsize=(size, size))
    nx.draw(graph, layout,edge_color='black',node_color='lightgreen',node_size=2000,with_labels=True,)
    
    # create targte directory if not exists
    os.makedirs(directory, exist_ok=True)
    
    # Save the figure as a PNG file within the given directory
    output_file = os.path.join(directory, 'network_graph.png')
    plt.savefig(output_file, format='png')
    plt.close()

if __name__ == "__main__":
    input_filepath,output_folder=initialize_path()
    network_graph = load_network_graph_from_file(input_filepath)
    plot_network_graph(network_graph, output_folder)

