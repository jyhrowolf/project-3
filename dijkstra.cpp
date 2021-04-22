#include "dijkstra.h"

#include <stdio.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>

using namespace std;

struct node 
{
    int value;
    float weight;
    node* next;
};

struct graphEdge 
{
    int edge_number;
    int start_vertex;
    int end_vertex;
    float weight;
};

class Graph 
{
    node* makeNode(int value, float weight, node* head) 
    {
        node* newNode = new node;
        newNode->value = value;
        newNode->weight = weight;
        newNode->next = head;   
        return newNode;
    }int Nodes;

    public:// construct
    node** head;
    Graph(graphEdge* edges, int edge, int nodes, bool directed)
    {
        head = new node * [Nodes]();
        this->Nodes = nodes;
        for (int i = 0; i < Nodes; ++i)
        {
            head[i] = nullptr;
        }
        if(directed)
        { 
            for (int i = 0; i < edge; i++) 
            {
                int edge_number = edges[i].edge_number;
                int start_vertex = edges[i].start_vertex;
                int end_vertex = edges[i].end_vertex;
                float weight = edges[i].weight;
                node* newNode = makeNode(end_vertex, weight, head[start_vertex]);
                head[start_vertex] = newNode;
            }
        }
        else
        {
            for (int i = 0; i < edge; i++)
            {
                int edge_number = edges[i].edge_number;
                int start_vertex = edges[i].start_vertex;
                int end_vertex = edges[i].end_vertex;
                float weight = edges[i].weight;
                node* newStartNode = makeNode(end_vertex, weight, head[start_vertex]);
                head[start_vertex] = newStartNode;
                node* newEndNode = makeNode(start_vertex, weight, head[end_vertex]);
                head[end_vertex] = newEndNode;
            }
        }
    }
    ~Graph()//destruct
    {
        for (int i = 0; i < Nodes; i++)
        {
            delete[] head[i];
        }
        delete[] head;
    }
};

struct minHeapNode
{
    int v;
    float dist;
};

struct minHeap
{
    int size;
    int cap;
    int* pos;
    struct minHeapNode** array;
};

struct minHeapNode* newMinHeapNode(int v, float dist, int flag)
{
    struct minHeapNode* minHeapNode = (struct minHeapNode*) malloc(sizeof(struct minHeapNode));
    minHeapNode->v = v;
    minHeapNode->dist = dist;
    if (flag == 1)
        printf("Insert vertex %d, key=%12.4f\n", v, dist);
    return minHeapNode;
}

struct minHeap* createMinHeap(int cap)
{
    struct minHeap* minHeap = (struct minHeap*)malloc(sizeof(struct minHeap));
    minHeap->pos = (int*)malloc(cap * sizeof(int));
    minHeap->size = 0;
    minHeap->cap = cap;
    minHeap->array = (struct minHeapNode**) malloc(cap *sizeof(struct minHeapNode*));
    return minHeap;
}

void swapMinHeapNode(struct minHeapNode** a, struct minHeapNode** b)
{
    struct minHeapNode* temp = *a;
    *a = *b;
    *b = temp;
}
void minHeapify(struct minHeap* minHeap, int index)
{
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < minHeap->size && minHeap->array[left]->dist < minHeap->array[smallest]->dist)
        smallest = left;
    if (right < minHeap->size && minHeap->array[right]->dist < minHeap->array[smallest]->dist)
        smallest = right;
    if (smallest != index)
    {
        minHeapNode* smallestNode = minHeap->array[smallest];
        minHeapNode* idxNode = minHeap->array[index];

        minHeap->pos[smallestNode->v] = index;
        minHeap->pos[idxNode->v] = smallest;

        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[index]);
        minHeapify(minHeap, smallest);
    }
}

int isEmpty(struct minHeap* minHeap)
{
    return minHeap->size == 0;
}

struct minHeapNode* extractMin(struct minHeap* minHeap, int flag)
{
    if (isEmpty(minHeap))
        return NULL;

    struct minHeapNode* root = minHeap->array[0];

    struct minHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    minHeap->pos[root->v] = minHeap->size - 1;
    minHeap->pos[lastNode->v] = 0;
    if (flag == 1)
        printf("Delete vertex %d, key=%12.4f\n", lastNode->v, lastNode->dist);
    minHeap->size--;
    minHeapify(minHeap, 0);

    return root;
}

void decreaseKey(struct minHeap* minHeap, int v, float dist, int flag)
{
    int i = minHeap->pos[v];
    minHeap->array[i]->dist = dist;
    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist)
    {
        minHeap->pos[minHeap->array[i]->v] = (i - 1) / 2;
        minHeap->pos[minHeap->array[(i - 1) / 2]->v] = i;
        swapMinHeapNode(&minHeap->array[i],&minHeap->array[(i - 1) / 2]);
        if (flag == 1)
            printf("Decrease key of vertex %d, from %12.4f to %12.4f\n", v, minHeap->array[i]->dist, minHeap->array[(i - 1) / 2]->dist);
        i = (i - 1) / 2;
    }
}
bool isInMinHeap(struct minHeap* minHeap, int v)
{
    if (minHeap->pos[v] < minHeap->size)
        return true;
    return false;
}
void printArr(int dist[], int n)
{
    printf("Vertex   Distance from Source\n");
    for (int i = 0; i < n; ++i)
        printf("%d \t\t %d\n", i, dist[i]);
}

float* dijkstra(class Graph* graph, int source, int destination, int vertices, int flag)
{
    float* dist = new float[vertices];
    struct minHeap* minHeap = createMinHeap(vertices);

    for (int i = 0; i < vertices; i++)
    {
        dist[i] = INFINITY;
        minHeap->array[i] = newMinHeapNode(i, dist[i], flag);
        minHeap->pos[i] = i;
    }

    minHeap->array[source] = newMinHeapNode(source, dist[source], flag);
    minHeap->pos[source] = source;
    dist[source] = 0;
    decreaseKey(minHeap, source, dist[source], flag);

    minHeap->size = vertices;
    while (!isEmpty(minHeap))
    {
        struct minHeapNode* minHeapNode = extractMin(minHeap, flag);
        int u = minHeapNode->v;
        struct node* dik = graph->head[u];
        while (dik != NULL)
        {
            int v = dik->value;
            if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX && dik->weight + dist[u] < dist[v])
            {
                dist[v] = dist[u] + dik->weight;
                decreaseKey(minHeap, v, dist[v], flag);
            }
            if(dik->value != destination)
                dik = dik->next;
            else
                break;
        }
    }

    return dist;
    delete[] dist;
}

int main(int argc, char* argv[])
{
    std::string fileName = argv[1];
    std::string direction = argv[2];

    ifstream file(fileName);
    std::string line;

    if (file.is_open())
    {
        int number;
        float weight;
        int n = 0;
        int m = 0;

        if (file >> number)
            n = number;
        if (file >> number)
            m = number;

        graphEdge* edges = new graphEdge[m];

        for (int i = 0; i < m; i++) 
        {
            if (file >> number)
                edges[i].edge_number = number;
            if (file >> number)
                edges[i].start_vertex = number;
            if (file >> number)
                edges[i].end_vertex = number;
            if (file >> weight)
                edges[i].weight = weight;
        }
        Graph graph(edges, m, n, direction.compare("directed") == 0);
        delete[] edges;

        std::string::size_type sz;
        int source;
        float* dd= NULL;
        getline(cin, line);
        while (line.compare("stop") != 0)
        {
            std::istringstream iss{line};
            std::vector<std::string> words{std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>()};
            if (words[0].compare("find") == 0)
            {
                cout << "Query: " << line;

                source = stoi(words[1], &sz);
                int destination = stoi(words[2], &sz);
                int flag = stoi(words[3], &sz);
                bool valid = true;
                
                if (graph.head[source] == NULL)
                    valid = false;
                if (flag != 1 && flag != 0)
                    valid = false;
                if (destination == source)
                    valid = false;

                if (valid)
                {
                    dd = dijkstra(&graph, source, destination, m, flag);
                }
                else
                {
                    cout << "Error: invalid find query\n";
                }
            }
            else if (words[0].compare("write"))
            {
                cout << "Query: " << line;
                if (dd != NULL)
                    if (source == stoi(words[2], &sz) && stoi(words[2], &sz) != stoi(words[3], &sz))
                    {
                        int length = sizeof(dd) / sizeof(dd[0]);
                        if (length == 0)
                            cout << "No " << words[2] << "-" << words[3]  <<" path exists.";
                        else if (length == 1)
                            cout << "No " << words[2] << "-" << words[3] << " path has been computed.";
                        else if (dd[stoi(words[3], &sz)] != NULL)
                        {
                            cout << "Shortest path: <";
                            for (int j = 0; j < length; j++)
                                cout << j << ", ";
                            cout << ">\n";
                            cout << "The path weight is : weight(";
                            for (int j = 0; j < length; j++)
                                printf("%12.4f, ", dd[j]);
                            cout << ")\n";
                        }
                        else
                            cout << "Error: invalid source destination pair\n";
                    }
                    else
                        cout << "Error: invalid source destination pair\n";
                else
                    cout << "Error: no path computation done\n";

            }
            getline(cin, line);
        }

        file.close();
    }
    return 0;
}