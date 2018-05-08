#include <ophidian/timing/GenericSTA.h>

namespace ophidian
{
namespace timing
{

    TimingData::TimingData(const library_type& lib, const timing_graph_type& graph) :
        m_library(lib),
        m_nodes_timing(graph),
        m_arcs_timing(graph)
    {
    }

    GraphAndTopology::GraphAndTopology(const timing_graph_type & graph,
                                       netlist_type & netlist,
                                       standard_cells_type & stdCells,
                                       library_mapping_type & libraryMapping) :
        m_graph(graph),
        m_netlist(netlist),
        m_sorted(m_graph.size(TimingGraph::node_type())),
        m_sorted_drivers(m_graph.size(TimingGraph::node_type()))
    {
        using graph_type = timing_graph_type::graph_type;

        graph_type::NodeMap<int> order(m_graph.graph());
        lemon::topologicalSort(m_graph.graph(), order);

        graph_type::NodeMap<int> level(m_graph.graph());

        for(graph_type::NodeIt it(m_graph.graph()); it != lemon::INVALID; ++it)
        {
            level[it] = std::numeric_limits<int>::max();
            m_sorted[ order[it] ] = it;
            m_sorted_drivers[ order[it] ] = it;
            if(lemon::countInArcs(m_graph.graph(), it) == 0)
                level[it] = 0;
        }

        int num_levels = 0;
        for(auto node : m_sorted)
        {
            if(lemon::countInArcs(m_graph.graph(), node) > 0)
            {
                int maxLevel = std::numeric_limits<int>::min();
                for(graph_type::InArcIt arc(m_graph.graph(), node); arc != lemon::INVALID; ++arc)
                    maxLevel = std::max(maxLevel, level[m_graph.source(arc)]);
                level[node] = maxLevel + 1;
                num_levels = std::max(num_levels, maxLevel + 1);
            }
        }

        m_levels.resize(num_levels+1);

        for(auto node : m_sorted)
            if(stdCells.direction(libraryMapping.pinStdCell(m_graph.entity(node))) == standard_cell::PinDirection::OUTPUT)
                m_levels[level[node]].push_back(node);

        auto beg = std::remove_if
        (
            m_levels.begin(), m_levels.end(),
            [this](std::vector<lemon::ListDigraph::Node> & vec)->bool
            {
                return vec.empty();
            }
        );

        m_levels.erase(beg, m_levels.end());


        auto begin = std::remove_if
        (
            m_sorted_drivers.begin(),
            m_sorted_drivers.end(),
            [this, &stdCells, &libraryMapping](graph_type::Node node)->bool
            {
                return stdCells.direction(libraryMapping.pinStdCell(m_graph.entity(node))) == standard_cell::PinDirection::OUTPUT;
            }
        );

        m_sorted_drivers.erase(begin, m_sorted_drivers.end());
    }

}   // namespace timing
}   // namespace ophidian
