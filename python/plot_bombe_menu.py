import matplotlib.pyplot as plt
import networkx as nx
import sys


def plot_bombe_menu(filename):
    with open(filename, 'rt') as f:
        registers = list()
        graph = nx.Graph()
        edge_labels = dict()
        for line in f.read().splitlines():
            line = line.upper()
            print(line)
            if line[0] == '+':
                break
            elif line[0] == '=':
                registers.append((line[1], line[3]))
            else:
                graph.add_edge(line[-2], line[-1])
                edge_labels[(line[-2], line[-1])] = line[0:-2]

        print(f'Registers: {registers}')

        plt.figure()
        plt.title(filename)
        pos = nx.spring_layout(graph)
        nx.draw(graph, pos, with_labels=True, font_weight='bold')
        nx.draw_networkx_edge_labels(graph, pos, edge_labels=edge_labels)
        plt.show()


if __name__ == '__main__':
    if len(sys.argv) >= 2:
        plot_bombe_menu(sys.argv[1])
    else:
        plot_bombe_menu(r'../data/m4_project_break2_menu.txt')
