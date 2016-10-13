import networkx as nx
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Button
from matplotlib.widgets import RadioButtons
import pylab
from networkx.drawing.nx_agraph import graphviz_layout
import subprocess
from tkinter import Tk, filedialog
import multiprocessing

fig = plt.figure()
fig.canvas.set_window_title('Graph Visualizer')
graph = nx.Graph().to_undirected()

colors = []
graphPlace = fig.add_subplot(1,1,1)
nx.draw(graph)
graphPlace.annotate("Welcome to the Graph Visualizer!!!\nSelect an input file for the graph...",[0.3,0.5],weight="extra bold",size="xx-large")

nodeColors = "grey"

def readInputGraph(filename):
    global graph
    graphFile = open(filename,"r")
   
    graph.clear()
    graphInput = graphFile.read(1);

    while  graphInput != '':
        if graphInput == 'c':
            graphFile.readline()
        if graphInput == 'p':
            statement = graphFile.readline().split()
            vertexNumber = statement[1]
            edgesNumber = statement[2]
        if graphInput == 'e':
            edge = graphFile.readline().split()
            graph.add_edge(edge[0],edge[1])
        graphInput = graphFile.read(1)
  
    graphFile.close()
    
    global pos
    pos=nx.circular_layout(graph)
    global node_labels
    node_labels = {node:node for node in graph.nodes()};
    drawGraph()

def readColors():
    bashMixColors="cat colorList | shuf > colorList2 ; mv colorList{2,}"
    subprocess.Popen(['/bin/bash','-c',bashMixColors])
    global colors
    colorsFile = open("colorList","r")
    colors = colorsFile.read().split()

def closeVisualizer(event):
    plt.close()
    quit()

def drawNotColored(event):
    global nodeColors
    nodeColors = "grey"
    drawGraph()

def returnColor(node):
    return colors[int(currentColors[int(node)-1])]

def drawColored(event):
    global lastDrawn
    colorFile = open("temp/answer","r")
    numberOfColors = colorFile.readline()
    colorNumber=plt.axes([0.48, 0.85, 0.13, 0.1])
    nx.draw(nx.Graph(),ax=colorNumber)
    colorNumber.annotate(numberOfColors,[0.0,0.9],weight="extra bold")

    global currentColors
    currentColors = colorFile.readline()
    colorFile.close()
    readColors()
    global nodeColors
    nodeColors = [returnColor(node) for node in graph.nodes()]
    drawGraph()

def drawGraph():
    global graphPlace
    graphPlace.clear();
    nx.draw(graph, pos, ax=graphPlace, node_color = nodeColors, node_size=1500, edge_color='black')
    nx.draw_networkx_labels(graph, pos, ax=graphPlace, labels=node_labels)

    plt.draw()

def layoutFunc(label):
    global pos
    if label == "circular(circo)":
        pos=graphviz_layout(graph,prog="circo")
    else:
        pos=graphviz_layout(graph,prog="fdp")
    drawGraph()

def chooseFile(event):
    Tk().withdraw()
    filename = filedialog.askopenfilename()
    readInputGraph(filename)
    
    global graphFilename
    graphFilename = filename

def changeAlgorithm(label):
    global chosenAlgorithm
    if label == "Genetic Algorithm":
        chosenAlgorithm = 0
    else:
        chosenAlgorithm = 1

def runAlgorithms(event):
    runAlgorithm="xterm -e mpirun -n "+str(multiprocessing.cpu_count())+" bin/"
    if chosenAlgorithm == 0:
        runAlgorithm+="genetic"
    else:
        runAlgorithm+="antcol"

    runAlgorithm+=" "+graphFilename
    runAlgorithm+=" "+"temp/answer";
    subprocess.Popen(['/bin/bash','-c',runAlgorithm])

closeButton = Button(plt.axes([0.87, 0.025, 0.1, 0.04]), 'Exit')
closeButton.on_clicked(closeVisualizer)

layoutSelectorPlace=plt.axes([0.01, 0.85, 0.13, 0.1])
layoutSelectorPlace.annotate("Select graph display layout:\n",[0.0,0.9],weight="bold",annotation_clip=True)
layoutSelector = RadioButtons(layoutSelectorPlace,('circular(circo)','graphviz(fdp)'))
layoutSelector.set_active(1)
layoutSelector.on_clicked(layoutFunc)

algorithmSelectorPlace=plt.axes([0.01,0.01,0.15,0.1])
algorithmSelectorPlace.annotate("Select coloring algorithm:\n",[0.0,0.9],weight="bold")
algorithmSelector = RadioButtons(algorithmSelectorPlace,('Genetic Algorithm','Ant Colony'))
algorithmSelector.on_clicked(changeAlgorithm)
algorithmSelector.set_active(0)

runAlgorithmButton = Button(plt.axes([0.17, 0.025, 0.1, 0.04]), 'Run algorithm')
runAlgorithmButton.on_clicked(runAlgorithms)

loadInputFile = Button(plt.axes([0.87, 0.93, 0.1, 0.04]), 'Load input graph')
loadInputFile.on_clicked(chooseFile)

colorButton = Button(plt.axes([0.67, 0.025, 0.1, 0.04]), 'Color')
colorButton.on_clicked(drawColored)

uncolorButton = Button(plt.axes([0.57, 0.025, 0.1, 0.04]), 'Remove colors')
uncolorButton.on_clicked(drawNotColored)

plt.show()

