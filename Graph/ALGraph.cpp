// Assignment : Graph
// Name : Wonhyeong Chae
// Course : CS280
// Term : Spring 2023

#include "ALGraph.h"
#include <algorithm>
#include <queue>
#include <functional>

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
    if (method == DEPTH_FIRST)
    {
        return DepthFirstSearch(start_node);
    }
    else
    {
        return BreadthFirstSearch(start_node);
    }
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

    const auto& adj_nodes = GetAdjList(start_node);

    for (const auto& info : adj_nodes)
    {
        Node& node = nodes[info.id - 1];
        node.info.cost = info.weight;

        pq.push(&node);
        node.info.path.push_back(start_node);
    }

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
    return true;
}

bool ALGraph::Node::operator<(const Node& rhs) const
{
    return info.cost < rhs.info.cost;
}

std::vector<unsigned> ALGraph::DepthFirstSearch(unsigned start_node) const
{
    std::vector<unsigned> visited;
    std::vector<bool> visited_flags(list_.size(), false);
    std::function<void(unsigned)> dfs = [&](unsigned current_node)
    {
        visited_flags[current_node - 1] = true;
        visited.push_back(current_node);

        // Sort neighbors by weight in descending order
        std::vector<AdjacencyInfo> neighbors = list_[current_node - 1];
        std::sort(neighbors.begin(), neighbors.end(), [](const AdjacencyInfo& a, const AdjacencyInfo& b)
        {
            return a.weight > b.weight;
        });

        for (const auto& neighbor : neighbors)
        {
            if (!visited_flags[neighbor.id - 1])
            {
                dfs(neighbor.id);
            }
        }
    };

    dfs(start_node);
    return visited;
}

std::vector<unsigned> ALGraph::BreadthFirstSearch(unsigned start_node) const
{
    std::vector<unsigned> visited;
    std::vector<bool> visited_flags(list_.size(), false);

    std::queue<unsigned> node_queue;
    node_queue.push(start_node);
    visited_flags[start_node - 1] = true;

    while (!node_queue.empty())
    {
        unsigned current_node = node_queue.front();
        node_queue.pop();

        visited.push_back(current_node);

        auto neighbors = list_[current_node - 1];

        // Sort neighbors in decreasing order of their weights
        std::sort(neighbors.begin(), neighbors.end(),
                  [](const AdjacencyInfo& a, const AdjacencyInfo& b) { return a.weight > b.weight; });

        for (const auto& neighbor : neighbors)
        {
            if (!visited_flags[neighbor.id - 1])
            {
                node_queue.push(neighbor.id);
                visited_flags[neighbor.id - 1] = true;
            }
        }
    }

    return visited;
}
