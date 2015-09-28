#include <iostream>
#include <map>
#include <queue>
#include <tuple>
typedef std::pair<int, int> CostToVert;
typedef std::multimap<int, CostToVert> EdgeMap;

void findShortPath(const int endVert, const EdgeMap &edges)
{
	// give me the node with shortest distance from vert 1
	// pair.first = sum of weights
	// pair.second = vert
	std::priority_queue<CostToVert, std::vector<CostToVert>, std::greater<CostToVert> > distanceTo1;

	std::vector<int> costs(endVert+1);

	// push everything connected to start (vert 1)
	std::pair<EdgeMap::const_iterator, EdgeMap::const_iterator> r = edges.equal_range(1);
	for (; r.first != r.second; ++r.first)
	{
		const int w2 = r.first->second.first;
		const int v2 = r.first->second.second;
#ifdef OUTPUT
		std::cout << "Start: " << w2 << ' ' << v2 << std::endl;
#endif
		if (costs[v2] == 0)
			costs[v2] = w2;
		else
			costs[v2] = std::min(costs[v2], w2);

		distanceTo1.push(r.first->second);
	}

	while (!distanceTo1.empty())
	{
		const CostToVert t = distanceTo1.top();
		distanceTo1.pop();

		const int w1 = t.first;
		const int v1 = t.second;
#ifdef OUTPUT
		std::cout << "Loop: " << w1 << ' ' << v1 << std::endl;
#endif
		if (v1 == endVert)
		{
#ifdef OUTPUT
			std::cout << "Done" << std::endl;
#endif

#ifdef OUTPUT
			int ct = 0;
			for (auto w : costs)
			{
				std::cout << ct << ' ' << w << std::endl;
				++ct;
			}
#endif

			std::vector<int> path;
			auto v = endVert;
			auto c = costs[endVert];
			path.push_back(v);
			while (v != 1)
			{
#ifdef OUTPUT
				std::cout << "Consider " << v << std::endl;
#endif
				std::pair<EdgeMap::const_iterator, EdgeMap::const_iterator> r = edges.equal_range(v);
				for (; r.first != r.second; ++r.first)
				{
					const int w2 = r.first->second.first;
					const int v2 = r.first->second.second;
#ifdef OUTPUT
					std::cout << "Edge " << v2 << ' ' << w2 << std::endl;
#endif
					if (v2 == 1 || c - w2 == costs[v2])
					{
#ifdef OUTPUT
						std::cout << "Next" << std::endl;
#endif
						v = v2;
						c -= w2;
						break;
					}
				}
				path.push_back(v);
			}

			while (!path.empty())
			{
				auto v = path.back();
				std::cout << v << ' ';
				path.pop_back();
			}
			std::cout << std::endl;

			return;
		}

		// push neighbors
		std::pair<EdgeMap::const_iterator, EdgeMap::const_iterator> r = edges.equal_range(v1);
		for (; r.first != r.second; ++r.first)
		{
			const int w2 = r.first->second.first + w1;
			const int v2 = r.first->second.second;
			if (costs[v2] != 0 && w2 >= costs[v2])
			{
#ifdef OUTPUT
				std::cout << "Bail: " << w2 << ' ' << v2 << std::endl;
#endif
				continue;
			}

#ifdef OUTPUT
			std::cout << "Push: " << w2 << ' ' << v2 << std::endl;
#endif
			if (costs[v2] == 0)
				costs[v2] = w2;
			else
				costs[v2] = std::min(costs[v2], w2);

			distanceTo1.push(std::make_pair(w2, v2));
		}
	}
	std::cout << -1 << std::endl;;
}

int main(int argc, char **argv)
{
	int numVerts = 0, numEdges = 0;

	// each problem set starts with the number of verts and edges
	std::cin >> numVerts >> numEdges;
	while (std::cin)
	{
		EdgeMap edges;

		// foreach edge definition
		for (int i = 0; i < numEdges; ++i)
		{
#ifdef OUTPUT
			std::cout << "Read edge" << std::endl;
#endif
			int v1, v2, w;
			std::cin >> v1 >> v2 >> w;
			if (!std::cin)
				return 0;

			edges.insert(std::make_pair(v1, std::make_pair(w, v2)));
			edges.insert(std::make_pair(v2, std::make_pair(w, v1)));
		}

		findShortPath(numVerts, edges);
		// next round
		std::cin >> numVerts >> numEdges;
	}

	return 0;
}

