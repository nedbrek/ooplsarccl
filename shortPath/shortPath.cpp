#include <cassert>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <tuple>
typedef std::pair<int, int> CostToVert;
typedef std::multimap<int, CostToVert> EdgeMap;

class ShuffleQueue : public std::priority_queue<CostToVert, std::vector<CostToVert>, std::greater<CostToVert> >
{
public:
};

class ShortPath
{
public:
	ShortPath(int ev, const EdgeMap &e)
	: edges(e)
	, endVert(ev)
	{
		for (int i = 0; i <= endVert; ++i)
		{
			distance.push_back(std::numeric_limits<int>::max());
			prev.push_back(0);
		}
		distance[1] = 0;
	}

	void findShortPath(const int endVert);

private:
	void printPath();
	void removeDist(const int v)
	{
		decltype(distanceTo1) tmp;
		while (!distanceTo1.empty())
		{
			const CostToVert t = distanceTo1.top();
			//const int w1 = t.first;
			const int v1 = t.second;
			distanceTo1.pop();
			if (v1 != v)
				tmp.push(t);
		}
		distanceTo1 = tmp;
	}

	std::vector<int> distance;
	std::vector<int> prev;

	// give me the node with shortest distance from vert 1
	// pair.first = sum of weights
	// pair.second = vert
	ShuffleQueue distanceTo1;
	const EdgeMap &edges;
	const int endVert;
};

void ShortPath::printPath()
{
#ifdef OUTPUT
			int ct = 0;
			for (auto w : distance)
			{
				std::cout << ct << ' ' << w << std::endl;
				++ct;
			}
#endif

			std::vector<int> path;
			auto v = endVert;
			path.push_back(v);
			while (v != 1)
			{
				v = prev[v];
				path.push_back(v);
			}

			while (!path.empty())
			{
				auto v = path.back();
				std::cout << v << ' ';
				path.pop_back();
			}
			std::cout << std::endl;
}

void ShortPath::findShortPath(const int endVert)
{
	// start at the beginning
	distanceTo1.push(std::make_pair(0, 1));

	int bestPath = std::numeric_limits<int>::max();
	while (!distanceTo1.empty())
	{
		const CostToVert t = distanceTo1.top();
		const int w1 = t.first;
		const int v1 = t.second;
		distanceTo1.pop();
#ifdef OUTPUT
		std::cout << "Pop: " << w1 << ' ' << v1 << std::endl;
#endif

		if (v1 == endVert)
		{
			printPath();
			return;
		}
		if (w1 > bestPath)
			continue;

		std::pair<EdgeMap::const_iterator, EdgeMap::const_iterator> r = edges.equal_range(v1);
		for (; r.first != r.second; ++r.first)
		{
			const int w2 = r.first->second.first + w1;
			const int v2 = r.first->second.second;

			if (w2 > bestPath)
				continue;

			if (w2 < distance[v2])
			{
				if (v2 == endVert)
				{
					bestPath = std::min(bestPath, w2);
				}

#ifdef OUTPUT
				std::cout << "Loop: " << w2 << ' ' << v2 << std::endl;
#endif
				distance[v2] = w2;
				prev[v2] = v1;
				removeDist(v2);
				distanceTo1.push(std::make_pair(w2, v2));
			}
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

		ShortPath s(numVerts, edges);
		s.findShortPath(numVerts);
		// next round
		std::cin >> numVerts >> numEdges;
	}

	return 0;
}

