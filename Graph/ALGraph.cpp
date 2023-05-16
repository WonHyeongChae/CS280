// Assignment : Graph
// Name : Wonhyeong Chae
// Course : CS280
// Term : Spring 2023

#include "ALGraph.h"
#include <algorithm>
#include <queue>

ALGraph::ALGraph(unsigned size)
{
    list_.reserve(size);

    for (size_t i = 0; i < size; ++i)
        list_.push_back(ALIST::value_type());
}

ALGraph::~ALGraph()
{
}

void ALGraph::AddDEdge(unsigned source, unsigned destination, unsigned weight)
{
    unsigned id = source - 1;
    AdjacencyInfo info{ destination, weight };
    auto& adjacent_list = list_.at(id);

    auto it = std::find_if(adjacent_list.begin(), adjacent_list.end(), [&](const AdjacencyInfo& adj_info)
    {
        auto [adj_id, adj_weight] = adj_info;
        return (info.weight < adj_weight) || (info.weight == adj_weight && info.id < adj_id);
    });

    adjacent_list.insert(it, info);
}

void ALGraph::AddUEdge(unsigned node1, unsigned node2, unsigned weight)
{
    AddDEdge(node1, node2, weight);
    AddDEdge(node2, node1, weight);
}

std::vector<unsigned> ALGraph::SearchFrom([[maybe_unused]] unsigned start_node, [[maybe_unused]] TRAVERSAL_METHOD method) const
{
    return std::vector<unsigned>();
}

std::vector<DijkstraInfo> ALGraph::Dijkstra(unsigned start_node) const
{
    auto GetAdjList = [=](unsigned i) { return list_.at(i - 1); };
    auto comp = [](const Node* lhs, const Node* rhs) -> bool { return *lhs < *rhs; };

    std::priority_queue<Node*, std::vector<Node*>, decltype(comp)> pq(comp);
    std::vector<DijkstraInfo> result;
    std::vector<Node> nodes;

    result.reserve(list_.size());
    nodes.reserve(list_.size());

    //initialize all nodes in graph
    for (unsigned i = 1; i < list_.size() + 1; ++i)
    {
        Node node;
        if (i != start_node)
        {
            node.evaluated = false;
            node.info.cost = INFINITY_;
        }
        else
        {
            node.evaluated = true;
            node.info.cost = 0;
            node.info.path.push_back(start_node);
        }

        node.id = i;
        nodes.push_back(node);
    }

    //push all nodes into queue with updated edge costs.
    const auto& adj_nodes = GetAdjList(start_node);

    for (const auto& info : adj_nodes)
    {
        Node& node = nodes[info.id - 1];
        node.info.cost = info.weight;

        pq.push(&node);
        node.info.path.push_back(start_node);
    }

    //go through the priority queue and evaluate all nodes.
    while (!pq.empty())
    {
        Node* v = pq.top();
        pq.pop();

        v->evaluated = true;
        v->info.path.push_back(v->id);

        const auto& adj_list = GetAdjList(v->id);

        for (const auto& info : adj_list)
        {
            Node& u = nodes.at(info.id - 1);

            unsigned new_cost = info.weight + v->info.cost;

            if (new_cost < u.info.cost)
            {
                u.info.cost = new_cost;
                u.info.path = v->info.path;

                if (!v->evaluated)
                    u.info.path.push_back(v->id);

                pq.push(&u);
            }
        }
    }

    for (const auto& node : nodes)
    {
        result.push_back(node.info);
    }
    return result;
}

ALIST ALGraph::GetAList() const
{
    return list_;
}

bool ALGraph::ImplementedSearches()
{
    return false;
}

bool ALGraph::Node::operator<(const Node& rhs) const
{
    return info.cost < rhs.info.cost;
}