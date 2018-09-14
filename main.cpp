#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <queue>
#include <algorithm>
#define MAX 2147483647;

/* ---- STRUCTURES ---- */

typedef struct vertix *Vertix;
typedef struct edge *Edge;
typedef struct vertixPosition * Position;

struct edge {
  Vertix w;
  int originalCapacity;
  int flow;
};

struct vertix {
  Position position;
  std::vector<Edge> vertixEdges;
  bool plan;
};

struct vertixPosition {
  int line;
  int column;
  int id;
};

/* ---- GLOBAL VARIABLES ---- */

int m, n, vertixIdCounter = 0, totalWeight = 0;
Vertix ** globalGraph = new Vertix*[m];


/* ---- AUXILIAR FUNCTIONS ---- */

/*defines a new edge between two vertices*/
Edge createEdge(Vertix y, int capacity) {
  Edge e = new edge();
  e->w = y;
  e->originalCapacity = capacity;
  e->flow = 0;
  return e;
}

/* ---- DFS ---- */
/* DFS used after the main algorithm to find the min-cut */

void DFS(Vertix ** globalGraph, Vertix originAndDestiny[2], bool * visited, Position * visitedData) {
  std::queue<Vertix> vertixQueue;
  Vertix u;
  int i,j,size;

  for(i=0; i<vertixIdCounter; i++) {
    visited[i] = false;
  }

  vertixQueue.push(originAndDestiny[0]);

  while(vertixQueue.size()>0) {
    u = vertixQueue.front();
    vertixQueue.pop();
    int id = u->position->id;
    if(!visited[id]) {
      visited[id] = true;
      visitedData[id] = u->position;
      size = u->vertixEdges.size();
      for(j=0; j<size; j++) 
        if((u->vertixEdges[j]->originalCapacity - u->vertixEdges[j]->flow) > 0)
          vertixQueue.push(u->vertixEdges[j]->w);
    }
  }

}

/* ---- passFlow ---- */

void passFlow(Vertix u, Position * previous, Vertix ** globalGraph, Vertix originAndDestiny[2]) {
  int h, edges, parentId, parentLine, parentColumn, maxFlow = MAX;

  /*we always start analysing the found path through the sink*/
  Vertix destino = u; 

  /*finds the bottleneck*/
  while(destino->position->id != 0) {
    Vertix origem;
    /*id do vertice de origem da edge*/
    parentId = previous[destino->position->id]->id;
    parentLine =  previous[destino->position->id]->line;
    parentColumn =  previous[destino->position->id]->column;
    /*caso o parent seja a source*/
    if(parentId == 0) 
      origem = originAndDestiny[0];

    /*vertix da matriz que corresponde ao parent*/
    else
      origem = globalGraph[parentLine][parentColumn];
    
    edges = origem->vertixEdges.size();
    for(h=0; h<edges; h++) {
      /*encontrada a edge*/
      if(origem->vertixEdges[h]->w->position->id == destino->position->id) {
        if((origem->vertixEdges[h]->originalCapacity - origem->vertixEdges[h]->flow) < maxFlow) 
          maxFlow = origem->vertixEdges[h]->originalCapacity - origem->vertixEdges[h]->flow;
      }
    }
    destino = origem;
  }

  /*updates the flow in each edge*/
  destino = u;

  while(destino->position->id != 0) {
    Vertix origem;
    parentId = previous[destino->position->id]->id;
    parentLine =  previous[destino->position->id]->line;
    parentColumn =  previous[destino->position->id]->column;
    if(parentId == 0) 
      origem = originAndDestiny[0];
    else
      origem = globalGraph[parentLine][parentColumn];
    edges = origem->vertixEdges.size();
    for(h=0; h<edges; h++) 
      if(origem->vertixEdges[h]->w->position->id == destino->position->id) 
        origem->vertixEdges[h]->flow += maxFlow;
    destino = origem;
  }

}

/* ---- BFS ---- */

bool BFS(Vertix ** globalGraph, Vertix originAndDestiny[2]) {
  int i, j;
  std::queue<Vertix> vertixQueue; 
  int * queued = new int[vertixIdCounter]; /*keeps track of the vertices in queue*/
  Position * previous = new Position[vertixIdCounter]; /*keeps track of the parent nodes*/
  int * processedVertix = new int[vertixIdCounter]; /*keeps track of the vertices whose adjacents have all been visited*/
  Vertix u;

  /*initialization*/
  for(i=0; i<vertixIdCounter; i++) {
    previous[i] = new vertixPosition();
    previous[i]->id = -1;
    previous[i]->line = -1;
    previous[i]->line = -1;
    processedVertix[i] = -1;
    queued[i] = -1;
  }

  /*we start with the source vertix*/
  vertixQueue.push(originAndDestiny[0]);
  queued[originAndDestiny[0]->position->id] = 1;
  previous[originAndDestiny[0]->position->id]->id = -2;
  previous[originAndDestiny[0]->position->id]->line = -2;
  previous[originAndDestiny[0]->position->id]->column = -2;

  while (vertixQueue.size() > 0) {
      u = vertixQueue.front();
      vertixQueue.pop();
      
      /*if the destiny vertix was found we make a path*/
      if(u->position->id == originAndDestiny[1]->position->id) {
        passFlow(u, previous, globalGraph, originAndDestiny);
        int size2 = vertixQueue.size();

        /*cleans the queue memory*/
        for(i = 0; i < size2; i++)
          vertixQueue.pop();

        return true;
      }
      /*visits each adjacent vertix of the current vertix*/
      int size = u->vertixEdges.size();
      for(j=0; j<size; j++) {
        /*if the node has already been processed skip over it*/
        if(processedVertix[u->vertixEdges[j]->w->position->id] == 1)
         continue;

        if(queued[u->vertixEdges[j]->w->position->id] != 1 && (u->vertixEdges[j]->originalCapacity - u->vertixEdges[j]->flow > 0)) {
          vertixQueue.push(u->vertixEdges[j]->w);
          queued[u->vertixEdges[j]->w->position->id] = 1;
          previous[u->vertixEdges[j]->w->position->id]->id = u->position->id;
          previous[u->vertixEdges[j]->w->position->id]->line = u->position->line;
          previous[u->vertixEdges[j]->w->position->id]->column = u->position->column;
        }
      }
      /*we finished processing this vertix*/
      processedVertix[u->position->id] = 1;
  }
  
  delete[] queued;
  delete[] previous;
  delete[] processedVertix;

  return false;
}

void dinic(Vertix ** globalGraph, Vertix originAndDestiny[2]) {
  int i, h, size;
  bool * visited = new bool[vertixIdCounter];
  Position * visitedData = new Position[vertixIdCounter]; /*keeps track of the parent nodes*/

  /*while there is a path from s to sink*/
  while(BFS(globalGraph, originAndDestiny)) {}

  /*to find the min-cut*/
  DFS(globalGraph, originAndDestiny, visited, visitedData);

  /*used to calc the total weight*/
  for(i = 1; i < vertixIdCounter-1; i++) 
    if(visited[i]) {
      Vertix u = globalGraph[visitedData[i]->line][visitedData[i]->column];
      size = u->vertixEdges.size();
      for(h = 0; h < size; h++) {
        Vertix destiny = u->vertixEdges[h]->w;
        if(!visited[destiny->position->id]) {
          u->plan = true;
          destiny->plan = false;

          if(destiny->position->id != vertixIdCounter-1)
            totalWeight += u->vertixEdges[h]->originalCapacity;
        }
      }
    }
  
  
  size = originAndDestiny[0]->vertixEdges.size();
  for(i = 0; i < size; i++) 
    if(!originAndDestiny[0]->vertixEdges[i]->w->plan) 
      totalWeight += originAndDestiny[0]->vertixEdges[i]->originalCapacity;
    else if(originAndDestiny[0]->vertixEdges[i]->w->plan)
      totalWeight += originAndDestiny[0]->vertixEdges[i]->w->vertixEdges[0]->originalCapacity;

  delete[] visited;
  delete[] visitedData;
}

int main() {
  int i, j, cp, lp, capacity;

  scanf("%d %d", &m, &n);

  /*bidimensional table of Vertix, origin vertix, destiny vertix*/
  Vertix ** globalGraph = new Vertix*[m];

  for(i=0; i<m; i++) globalGraph[i] = new Vertix[n];

  /* originAndDestiny[0] = source, originAndDestiny[1] = sink*/
  Vertix * originAndDestiny = new Vertix[2];
  originAndDestiny[0] = new vertix();
  originAndDestiny[1] = new vertix();
  originAndDestiny[0]->position = new vertixPosition();
  originAndDestiny[1]->position = new vertixPosition();

  originAndDestiny[0]->position->id = vertixIdCounter++;
  originAndDestiny[0]->position->line = -2;
  originAndDestiny[0]->position->column = -2;
  originAndDestiny[1]->position->line = -3;
  originAndDestiny[1]->position->column = -3;
  
  /*scans the lp value of each vertix and creates an edge between the origin
    and each vertix with the capacity of lp*/
  for(i=0; i<m; i++) {
    for(j=0; j<n; j++) {
      scanf("%d\n", &lp);
      globalGraph[i][j] = new vertix();
      globalGraph[i][j]->position = new vertixPosition();
      globalGraph[i][j]->position->line = i;
      globalGraph[i][j]->position->column = j;
      globalGraph[i][j]->position->id = vertixIdCounter++;
      originAndDestiny[0]->vertixEdges.push_back(createEdge(globalGraph[i][j], lp));
    }
  }

  originAndDestiny[1]->position->id = vertixIdCounter++;

  /*scans the cp value of each vertix and creates an edge between each vertix
    and the destiny with the capacity of cp*/
  int counter = 0;
  for(i=0; i<m; i++) {
    for(j=0; j<n; j++) {
      scanf("%d\n", &cp);
      int flow = std::min(originAndDestiny[0]->vertixEdges[counter]->originalCapacity, cp);
      originAndDestiny[0]->vertixEdges[counter]->flow = flow;
      Edge e = createEdge(originAndDestiny[1],cp);
      e->flow = flow;
      globalGraph[i][j]->vertixEdges.push_back(e);
      counter++;
    }
  }

  /*creates one horizontal edge between each two vertices in each direction and scans the capacity value*/
  for(i=0; i<m; i++) {
    for(j=0; j<n-1; j++) {
      scanf("%d\n", &capacity);
      /*creates an edge in each direction*/
      globalGraph[i][j]->vertixEdges.push_back(createEdge(globalGraph[i][j+1], capacity));
      globalGraph[i][j+1]->vertixEdges.push_back(createEdge(globalGraph[i][j], capacity));
    }
  }



  /*creates one vertical edge between each two vertices in each direction and scans the capacity value*/
  for(i=0; i<m-1; i++) {
    for(j=0; j<n; j++) {
      scanf("%d\n", &capacity);
      /*creates an edge in each direction*/
      globalGraph[i][j]->vertixEdges.push_back(createEdge(globalGraph[i+1][j], capacity));
      globalGraph[i+1][j]->vertixEdges.push_back(createEdge(globalGraph[i][j], capacity));
    }
  }

  /*executes the main algorithm*/
  dinic(globalGraph,originAndDestiny);

  /*output*/
  printf("%d\n\n", totalWeight);
  for(i=0; i<m; i++) {
    for(j=0; j<n; j++) {
      if(!globalGraph[i][j]->plan) 
        printf("P ");
      else
        printf("C ");
    }
    printf("\n");
  }

  for(i=0; i<m; i++) {
    for(j=0; j<n; j++) {
      globalGraph[i][j]->vertixEdges.clear();
      delete globalGraph[i][j];
    }
    delete[] globalGraph[i];
  }
  delete[] globalGraph;
  
  for(i=0; i<2; i++) {
    originAndDestiny[i]->vertixEdges.clear();
    delete originAndDestiny[i];
  }
  delete[] originAndDestiny;

  return 0;
}
