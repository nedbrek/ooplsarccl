#include <deque>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <vector>

class AusVote
{
	typedef uint16_t CandidateId; // size to number of candidates (20)

public:
	/// get the list of candidate names
	void getCandidates(std::istream &is)
	{
#ifdef OUTPUT
		std::cout << "Getting candidates" << std::endl;
#endif
		std::string tmp;
		std::getline(is, tmp);

		std::istringstream iss(tmp);
		CandidateId numCandidates = 0;
		iss >> numCandidates;

		while (numCandidates)
		{
			std::getline(is, tmp);
			candidateNames_.push_back(tmp);
			--numCandidates;
		}
	}

	/// gather the votes
	bool getVotes(std::istream &is)
	{
#ifdef OUTPUT
		std::cout << "Getting votes" << std::endl;
#endif
		numVotes_.resize(candidateNames_.size());

		// foreach ballot
		std::string tmp;
		std::getline(is, tmp);
		while (is && !tmp.empty())
		{
			// parse votes
			std::istringstream ss(tmp);

			CandidateId firstVote = 0;
			ss >> firstVote;
			--firstVote; // convert from count to offset

			Ballot b;
			// gather the remaining ballot
			for (size_t i = 1; i < candidateNames_.size(); ++i)
			{
				CandidateId c = 0;
				ss >> c;
				b.push_back(c - 1); // convert from count to offset
			}

			// register this voter with his first choice candidate
			voteMap_.insert(std::make_pair(firstVote, votes_.size()));

			// store the ballot
			votes_.push_back(b);

			++numVotes_[firstVote];

			std::getline(is, tmp);
		}

		return prepForFirstRound();
	}

	void printWinner(std::ostream &os, bool earlyDecision)
	{
#ifdef OUTPUT
		std::cout << "Who will win?" << std::endl;
#endif
		// weird case - no votes
		if (remainingCandidates_.size() == 0)
		{
			printAll(os);
			return;
		}

		// if decision can be told from initial condtions
		if (earlyDecision)
		{
			printRemainder(os);
			return;
		}

		bool done = false;
		while (!done && maxVotes_ < (votes_.size() + 2) / 2)
		{
			done = reduceCandidates();
		}

		if (done)
		{
			printRemainder(os);
		}
		else
		{
			printMaxVotes(os);
		}
	}

private:
	// calc min and max, and load remainingCandidates_ set
	bool prepForFirstRound()
	{
		bool anySet = false;
		bool allSame = true;
		for (CandidateId c = 0; c < numVotes_.size(); ++c)
		{
			if (numVotes_[c] == 0)
				continue;

			remainingCandidates_.insert(c);

			if (!anySet)
			{
				anySet = true;
				maxVotes_ = numVotes_[c];
				minVotes_ = numVotes_[c];
				continue;
			}

			if (numVotes_[c] != maxVotes_)
				allSame = false;

			maxVotes_ = std::max(maxVotes_, numVotes_[c]);
			minVotes_ = std::min(minVotes_, numVotes_[c]);
		}
		return allSame;
	}

	// recalc min and max
	bool prepForRound()
	{
		bool anySet = false;
		bool allSame = true;
		for (auto c : remainingCandidates_)
		{
			if (!anySet)
			{
				anySet = true;
				maxVotes_ = numVotes_[c];
				minVotes_ = numVotes_[c];
				continue;
			}

			if (numVotes_[c] != maxVotes_)
				allSame = false;

			maxVotes_ = std::max(maxVotes_, numVotes_[c]);
			minVotes_ = std::min(minVotes_, numVotes_[c]);
		}
#ifdef OUTPUT
		std::cout << "Next round " << remainingCandidates_.size() << ' ' << maxVotes_ << ' ' << minVotes_ << std::endl;
#endif

		return allSame;
	}

	bool reduceCandidates()
	{
		std::vector<CandidateId> eliminated;
		for (CandidateId c = 0; c < numVotes_.size(); ++c)
		{
			if (numVotes_[c] == minVotes_ && remainingCandidates_.count(c))
			{
				eliminated.push_back(c);
			}
		}

		if (eliminated.size() == remainingCandidates_.size())
			return true; // no one else to eliminate - everyone is a winner!

		for (auto c : eliminated)
		{
			eliminateCandidate(c);
		}

		// recalc min and max
		return prepForRound();
	}

	void eliminateCandidate(CandidateId c)
	{
#ifdef OUTPUT
		std::cout << "Reduce " << candidateNames_[c] << std::endl;
#endif
		remainingCandidates_.erase(remainingCandidates_.find(c));

		// transfer his votes
		//-- find all supporters
		for (std::pair<VoteMap::iterator, VoteMap::iterator> vmip = voteMap_.equal_range(c); vmip.first != vmip.second; ++vmip.first)
		{
			// who voted
			const VoteCt supporter = vmip.first->second;

			// get next vote
			CandidateId nextVote = votes_[supporter].front();
			votes_[supporter].pop_front(); // destructive

			while (!votes_[supporter].empty() && !remainingCandidates_.count(nextVote))
			{
				nextVote = votes_[supporter].front();
				votes_[supporter].pop_front();
			}

			if (votes_[supporter].empty())
			{
#ifdef OUTPUT
				std::cout << supporter << " out of votes" << std::endl;
#endif
				continue;
			}

#ifdef OUTPUT
			std::cout << "New vote for " << candidateNames_[nextVote] << std::endl;
#endif

			// register the new supporter
			voteMap_.insert(std::make_pair(nextVote, supporter));
			++numVotes_[nextVote];
		}

		// Ned? delete old entries? (time vs. space - and space takes time...)
		//voteMap_.erase(c);
	}

	void printAll(std::ostream &os)
	{
		for (auto &name : candidateNames_)
		{
			os << name << std::endl;
		}
	}

	void printRemainder(std::ostream &os)
	{
		for (auto cid : remainingCandidates_)
		{
			os << candidateNames_[cid] << std::endl;
		}
	}

	void printMaxVotes(std::ostream &os)
	{
		for (auto cid : remainingCandidates_)
		{
			if (numVotes_[cid] == maxVotes_)
				os << candidateNames_[cid] << std::endl;
		}
	}

private:
	typedef uint16_t VoteCt; // size to number of votes 1000
	typedef std::deque<CandidateId> Ballot; // remaining votes
	typedef std::vector<Ballot> Votes; // all the votes
	typedef std::multimap<CandidateId, VoteCt> VoteMap; // first vote to offset into all Votes

	std::vector<std::string> candidateNames_;
	std::set<CandidateId> remainingCandidates_;
	Votes votes_;
	VoteMap voteMap_;
	std::vector<VoteCt> numVotes_;

	VoteCt maxVotes_ = 0;
	VoteCt minVotes_ = std::numeric_limits<VoteCt>::max();
};

void doCase(bool end)
{
	AusVote v;
	v.getCandidates(std::cin);

	const bool earlyDecision = v.getVotes(std::cin);

	v.printWinner(std::cout, earlyDecision);

	if (!end)
		std::cout << std::endl;
}

int main(int argc, char **argv)
{
	std::string s;
	std::getline(std::cin, s);
	std::istringstream iss(s);
	int numCases = 0;
	iss >> numCases;

	// skip the blank line
	std::getline(std::cin, s);

	while (numCases)
	{
		doCase(numCases == 1);

		--numCases;
	}

	return 0;
}

