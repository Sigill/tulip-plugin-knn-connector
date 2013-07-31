#include <tulip/ImportModule.h>
#include <tulip/TulipPluginHeaders.h>
#include <stdexcept>
#include <vector>
#include <queue>
#include <cmath>

#define CHECK_PROP_PROVIDED(PROP, STOR) \
	do { \
		if(!dataSet->get(PROP, STOR)) \
		throw std::runtime_error(std::string("No \"") + PROP + "\" property provided."); \
	} while(0)

using namespace std;
using namespace tlp;

template <typename First, typename Second>
class PairFirstComparator
{
private:
	bool asc;
public:
	PairFirstComparator(const bool& ascending_order = true) { asc = ascending_order; }
	bool operator() (const std::pair<First, Second>& lhs, const std::pair<First, Second>& rhs) const
	{
		if (asc) return (lhs.first < rhs.first);
		else return (lhs.first > rhs.first);
	}
};

namespace {
	const char * paramHelp[] = {
		HTML_HELP_OPEN() \
			HTML_HELP_DEF( "type", "LayoutProperty" ) \
			HTML_HELP_BODY() \
			"The layout on which the distance will be computed." \
			HTML_HELP_CLOSE(),

		HTML_HELP_OPEN() \
			HTML_HELP_DEF( "type", "DoubleProperty" ) \
			HTML_HELP_BODY() \
			"The property in which to store the computed distance." \
			HTML_HELP_CLOSE(),

		HTML_HELP_OPEN() \
			HTML_HELP_DEF( "type", "unsigned int" ) \
			HTML_HELP_BODY() \
			"Number of nearest neighbors to connect." \
			HTML_HELP_CLOSE()
	};
}

class KnnConnectorOnLayout:public tlp::Algorithm {
private:
	tlp::LayoutProperty *layout;
	tlp::DoubleProperty *metric;
	unsigned int k;

public:
	PLUGININFORMATIONS("Build knn on layout", "Cyrille FAUCHEUX", "2013-07-31", "", "1.0", "Topology Update")

	KnnConnectorOnLayout(const tlp::PluginContext *context):Algorithm(context) {
		addInParameter< LayoutProperty >   ("layout", paramHelp[0], "viewLayout");
		addInParameter< DoubleProperty >   ("metric", paramHelp[1], "viewMetric");
		addInParameter< unsigned int >     ("k",      paramHelp[2], "10"        );
	}

	bool check(std::string &err) {
		try {
			if(dataSet == NULL)
				throw std::runtime_error("No dataset provided.");

			CHECK_PROP_PROVIDED("layout", this->layout);
			CHECK_PROP_PROVIDED("metric", this->metric);
			CHECK_PROP_PROVIDED("k",      this->k     );

			if(this->k <= 0)
				throw std::runtime_error("The value for the \"k\" must be greater than 0.");
		} catch (std::runtime_error &ex) {
			err.assign(ex.what());
			return false;
		}

		return true;
	}

	bool run() {
		node u, v;
		edge e;
		Coord cu, cv;
		double d;

		std::vector< node > nodes;
		nodes.reserve(graph->numberOfNodes());

		std::vector< double > distances;
		distances.reserve(graph->numberOfNodes());

		{ // Building a list of the nodes, it will make iterating through it easier than with tulip iterators
			Iterator<node> *itNodes = graph->getNodes();

			while(itNodes->hasNext())
				nodes.push_back(itNodes->next());

			delete itNodes;
		}

		std::vector< node >::const_iterator itU, itV, it_end = nodes.end();
		for(itU = nodes.begin(); itU != it_end; ++itU) {
			u = *itU;
			cu = this->layout->getNodeValue(u);

			typedef std::priority_queue< std::pair<double, node>, std::vector< std::pair<double, node> >, PairFirstComparator<double, node> > MinHeap;
			MinHeap q = MinHeap(PairFirstComparator<double, node>());
			for(itV = nodes.begin(); itV != it_end; ++itV) {
				v = *itV;

				if(u == v)
					continue;

				cv = this->layout->getNodeValue(v);

				d = sqrt( (cu[0] - cv[0]) * (cu[0] - cv[0]) + (cu[1] - cv[1]) * (cu[1] - cv[1]) + (cu[2] - cv[2]) * (cu[2] - cv[2]) );

				if(q.size() < k || d < q.top().first) {
					q.push(std::pair<double, node>(d, v));

					if(q.size() > k) { // We only keep k elements
						q.pop();
					}
				}
			}

			int n = q.size();
			for (int i = 0; i < n; ++i) {
				v = q.top().second;
				edge e = graph->existEdge(u, v, false);
				if(!e.isValid()) {
					e = graph->addEdge(u, v);
					metric->setEdgeValue(e, q.top().first);
				}
				q.pop();
			}
		}

		return true;
	}

	~KnnConnectorOnLayout() {}
};

PLUGIN(KnnConnectorOnLayout)
