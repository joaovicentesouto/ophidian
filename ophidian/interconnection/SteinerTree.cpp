#include "SteinerTree.h"
#include <ophidian/geometry/Distance.h>

namespace ophidian {
namespace interconnection {

SteinerTree::SteinerTree() :
    position_(graph_)
{

}

uint32_t SteinerTree::numSegments() const
{
    return lemon::countEdges(graph_);
}

uint32_t SteinerTree::numPoints() const
{
    return lemon::countNodes(graph_);
}


namespace {
SteinerTree::GraphType::Node findNodeWithPositionEqualsTo(const geometry::Point &position, const SteinerTree::GraphType::NodeMap<geometry::Point> & position_, const SteinerTree::GraphType & graph_)
{
    geometry::ManhattanDistance distance;
    for(SteinerTree::GraphType::NodeIt i(graph_); graph_.valid(i); ++i)
    {
        if(distance(position, position_[i]) == 0.0)
        {
            return i;
        }
    }
    return lemon::INVALID;
}
}

SteinerTree::Point SteinerTree::addPoint(const geometry::Point &position)
{
    GraphType::Node node = findNodeWithPositionEqualsTo(position, position_, graph_);
    if(node == lemon::INVALID)
    {
        node = graph_.addNode();
        position_[node] = position;
    }
    return Point(node);
}

SteinerTree::Segment SteinerTree::addSegment(const SteinerTree::Point &p1, const SteinerTree::Point &p2)
{
    auto edge = graph_.addEdge(p1.el_, p2.el_);
    return Segment(edge);
}

void SteinerTree::position(const SteinerTree::Point &p1, const geometry::Point &position)
{
    position_[p1.el_] = position;
}

SteinerTree::Point SteinerTree::u(const SteinerTree::Segment &segment) const
{
    return Point(graph_.u(segment.el_));
}

SteinerTree::Point SteinerTree::v(const SteinerTree::Segment &segment) const
{
    return Point(graph_.v(segment.el_));
}

geometry::Point SteinerTree::position(const SteinerTree::Point &p) const
{
    return position_[p.el_];
}

double SteinerTree::length(const SteinerTree::Segment &segment) const
{
    const auto kU = graph_.u(segment.el_);
    const auto kV = graph_.v(segment.el_);
    geometry::ManhattanDistance distance;
    return distance(position_[kU], position_[kV]);
}

std::pair<SteinerTree::PointIterator, SteinerTree::PointIterator> SteinerTree::points() const
{
    PointIterator first{Point{GraphType::NodeIt{graph_}}};
    PointIterator second{Point{static_cast<GraphType::Node>(lemon::INVALID)}};
    return std::make_pair(first, second);
}

std::pair<SteinerTree::SegmentIterator, SteinerTree::SegmentIterator> SteinerTree::segments(const SteinerTree::Point &point) const
{
    SegmentIterator first{{graph_, point.el_}};
    SegmentIterator second{lemon::INVALID};
    using GraphT = GraphType;
    using DegItT = GraphT::IncEdgeIt;
    return std::make_pair(first, second);
}

SteinerTree::PointIterator::PointIterator(const SteinerTree::Point &p) :
    point_(p)
{

}

SteinerTree::SegmentIterator::SegmentIterator(GraphType::IncEdgeIt it) :
    it_(it)
{

}

}

namespace geometry
{
template <>
geometry::Segment make<geometry::Segment>(const interconnection::SteinerTree & tree, const interconnection::SteinerTree::Segment & segment)
{
    return make<geometry::Segment>({tree.position(tree.u(segment)), tree.position(tree.v(segment))});
}
}

}

