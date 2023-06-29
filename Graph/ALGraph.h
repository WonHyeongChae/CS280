// Assignment : Graph
// Name : Wonhyeong Chae
// Course : CS280
// Term : Spring 2023

//---------------------------------------------------------------------------
#ifndef ALGRAPHH
#define ALGRAPHH
//---------------------------------------------------------------------------
#include <vector>

/*!
  Information used to communicate with the driver after Dijkstra's algorithm
  has run. There will typically be one of these per node in the graph.
*/
struct DijkstraInfo
{
    unsigned cost;              //!< The cost to reach this node
    std::vector<unsigned> path; //!< The shortest path that led to this node
};

/*!
  Stores information for the adjacency lists
*/
struct AdjacencyInfo
{
    unsigned id;     //!< The node's unique ID
    unsigned weight; //!< The weigth of the edge leading to this node
};

//! Convenient shorthand for an adjacency list type
typedef std::vector<std::vector<AdjacencyInfo> > ALIST;

/*!
  Class that represents a graph.
*/
class ALGraph
{
public:
    //! The different types of traversals
    enum TRAVERSAL_METHOD { DEPTH_FIRST, BREADTH_FIRST };

    ALGraph(unsigned size);
    ~ALGraph();
    void AddDEdge(unsigned source, unsigned destination, unsigned weight);
    void AddUEdge(unsigned node1, unsigned node2, unsigned weight);

    std::vector<unsigned> SearchFrom(unsigned start_node, TRAVERSAL_METHOD method) const;
    std::vector<DijkstraInfo> Dijkstra(unsigned start_node) const;
    ALIST GetAList() const;
    static bool ImplementedSearches();

private:
    // Private fields and methods ...
    struct Node
    {
        unsigned id;
        bool evaluated;
        DijkstraInfo info;
        bool operator<(const Node& rhs) const;
    };

    struct AdjacentInfo
    {
        unsigned id;
        unsigned weight;
        unsigned cost;
    };

    const unsigned INFINITY_ = static_cast<unsigned>(-1);
    ALIST list_;

    std::vector<unsigned> DepthFirstSearch(unsigned start_node) const;
    std::vector<unsigned> BreadthFirstSearch(unsigned start_node) const;
};

#endif