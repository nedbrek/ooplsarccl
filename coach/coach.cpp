#include <iostream>
#include <map>
#include <set>
#include <vector>

using Team = std::set<uint8_t>;

class ConstrainedTeams
{
public:
	ConstrainedTeams(int numStudents)
	: teams_(numStudents / 3)
	{
	}

	bool handleConstraint(int first, int second)
	{
#ifdef OUTPUT
		std::cout << "Handle " << first << ' ' << second << std::endl;
#endif
		auto i = teamAssignment.find(first);
		auto j = teamAssignment.find(second);

		int team = numTeamsAssigned;
#ifdef OUTPUT
		std::cout << "Team " << numTeamsAssigned << std::endl;
#endif

		if (i == teamAssignment.end())
		{
			if (j == teamAssignment.end())
			{
				// first assignment for either student
				if (team >= int(teams_.size()))
					return true;

				teamAssignment[first] = team;
				teamAssignment[second] = team;
				teams_[team].insert(first);
				teams_[team].insert(second);
				++numTeamsAssigned;
				return false;
			}
			//else second has a team
			team = j->second;
		}
		else
		{
			// first has a team
			team = i->second;

			if (j != teamAssignment.end())
			{
				// second better match
				return team != j->second;
			}
		}

		teamAssignment[first] = team;
		teamAssignment[second] = team;
		teams_[team].insert(first);
		teams_[team].insert(second);

		return teams_[team].size() > 3;
	}

	void finishAssignments()
	{
#ifdef OUTPUT
		std::cout << "Finish" << std::endl;
#endif
		int nextTeam = 0;
		const int numStudents = teams_.size() * 3;
		for (int s = 1; s <= numStudents; ++s)
		{
#ifdef OUTPUT
			std::cout << "S " << s << std::endl;
#endif
			const auto i = teamAssignment.find(s);
			if (i != teamAssignment.end())
				continue;

			while (teams_[nextTeam].size() >= 3)
				++nextTeam;

#ifdef OUTPUT
			std::cout << "Team " << nextTeam << std::endl;
#endif
			teams_[nextTeam].insert(s);
		}
	}

	void print()
	{
#ifdef OUTPUT
		std::cout << "Print" << std::endl;
#endif
		for (const auto &t : teams_)
		{
#ifdef OUTPUT
			std::cout << "Team " << t.size() << std::endl;
#endif
			for (const auto &m : t)
			{
#ifdef OUTPUT
				std::cout << "M " << int(m) << std::endl;
#endif
				std::cout << int(m) << ' ';
			}
			std::cout << std::endl;
		}
	}

private:
	// 3 students per team
	std::vector<Team> teams_;
	std::map<int, int> teamAssignment; // map from student id to team index
	int numTeamsAssigned = 0;
};

int main(int argc, char **argv)
{
	int numStudents = 0;
	int numConstraints = 0;
	std::cin >> numStudents >> numConstraints;
#ifdef OUTPUT
	std::cout << "Num students " << numStudents << std::endl;
#endif

	ConstrainedTeams c(numStudents);
	while (std::cin && numConstraints)
	{
		int first = 0;
		int second = 0;
		std::cin >> first >> second;
#ifdef OUTPUT
		std::cout << "Constraint: " << first << ' ' << second << std::endl;
#endif
		if (c.handleConstraint(first, second))
		{
			std::cout << -1 << std::endl;
			return 0;
		}
		--numConstraints;
	}

	c.finishAssignments();
	c.print();

	return 0;
}

