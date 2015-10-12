#include <cassert>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <vector>

class AusVote
{
	typedef uint16_t CandidateId; // size to number of candidates (20)
	typedef uint16_t VoteCt; // size to number of votes (1000)
	typedef std::vector<CandidateId> Ballot; // remaining votes
	typedef std::multimap<CandidateId, Ballot> VoteMap; // first vote to remaining votes
	typedef std::vector<VoteCt> Votes;

public:
	// get the list of candidate names
	void getCandidates(std::istream &is)
	{
		std::string tmp;
		std::getline(is, tmp);

		std::istringstream iss(tmp);
		size_t numCandidates = 0;
		iss >> numCandidates;
#ifdef OUTPUT
		std::cout << "Num candidates " << numCandidates << std::endl;
#endif

		while (numCandidates)
		{
			std::getline(is, tmp);
			candidateNames_.push_back(tmp);
#ifdef OUTPUT
			std::cout << "Candidate " << tmp << std::endl;
#endif

			--numCandidates;
		}
	}

	// gather the votes
	void getVotes(std::istream &is)
	{
		votes_.resize(candidateNames_.size());

		// foreach ballot
		std::string tmp;
		std::getline(is, tmp);
		while (is && !tmp.empty())
		{
			// parse votes
			std::istringstream ss(tmp);

			CandidateId firstVote = 0;
			ss >> firstVote;

			// count vote
			++votes_[makeIndex(firstVote)];
			maxVotes_ = std::max(maxVotes_, votes_[makeIndex(firstVote)]);
			minVotes_ = std::min(minVotes_, votes_[makeIndex(firstVote)]);
#ifdef OUTPUT
			std::cout << "Vote for " << candidateNames_[makeIndex(firstVote)] << ' ' << maxVotes_ << std::endl;
#endif

			// store the ballot
			VoteMap::iterator vmi = voteMap_.insert(std::make_pair(firstVote, Ballot()));
			Ballot &v = vmi->second;

			// gather backup votes
			for (size_t i = 1; i < votes_.size(); ++i)
			{
				short c = 0;
				ss >> c;
				v.push_back(c);
			}

			std::getline(is, tmp);
		}

		for (size_t i = 0; i < votes_.size(); ++i)
		{
			if (votes_[i] == 0)
			{
#ifdef OUTPUT
			std::cout << "No votes for " << candidateNames_[i] << std::endl;
#endif
				eliminated_.insert(i);
			}
		}
	}

	// determine winner
	void printWinner(std::ostream &os)
	{
		bool done = false;
		while (!done && maxVotes_ < (voteMap_.size() + 1) / 2)
		{
#ifdef OUTPUT
			std::cout << "Max votes " << maxVotes_ << std::endl;
#endif
			done = reduceCandidates();
		}

		//foreach candidate
		for (size_t i = 0; i < votes_.size(); ++i)
		{
			// if candidate has most votes
			if (votes_[i] == maxVotes_ && eliminated_.count(i) == 0)
				os << candidateNames_[i] << std::endl;
		}
	}

private:
	size_t fromIndex(CandidateId cid)
	{
		return cid + 1;
	}

	size_t makeIndex(CandidateId vote)
	{
		assert(vote > 0);
		return vote - 1;
	}

	bool reduceCandidates()
	{
		std::set<CandidateId> eliminated;
		for (size_t i = 0; i < votes_.size(); ++i)
		{
			if (votes_[i] == minVotes_ && eliminated_.count(i) == 0)
			{
				eliminated.insert(i);
			}
		}

		if (eliminated.size() + eliminated_.size() == votes_.size())
			return true;

		for (auto c : eliminated)
		{
			eliminateCandidate(c);
		}

		// recalculate min and max
		maxVotes_ = 0;
		minVotes_ = std::numeric_limits<VoteCt>::max();
		for (size_t i = 0; i < votes_.size(); ++i)
		{
			if (votes_[i] == 0 || eliminated.count(i) > 0)
				continue;

			maxVotes_ = std::max(maxVotes_, votes_[i]);
			minVotes_ = std::min(minVotes_, votes_[i]);
		}

		if (maxVotes_ > 0)
		{
			for (auto c : eliminated)
			{
#ifdef OUTPUT
			std::cout << "Perma-eliminate " << candidateNames_[c] << std::endl;
#endif
				eliminated_.insert(c);
			}
			return false;
		}
		return true;
	}

	CandidateId getNewVote(Ballot *bp)
	{
		bool done = bp->empty();
		while (!done)
		{
			const CandidateId firstVote = (*bp)[0];
			std::move(bp->begin() + 1, bp->end(), bp->begin());

#ifdef OUTPUT
			std::cout << "New vote for " << firstVote;
#endif

			if (eliminated_.count(makeIndex(firstVote)) == 0)
			{
#ifdef OUTPUT
				std::cout << ". Good vote" << std::endl;
#endif
				return firstVote;
			}
#ifdef OUTPUT
			std::cout << ". Bad vote" << std::endl;
#endif
		}

		return 0;
	}

	void eliminateCandidate(CandidateId cid)
	{
#ifdef OUTPUT
		std::cout << "Reduce " << candidateNames_[cid] << std::endl;
#endif
		votes_[cid] = 0;

		std::pair<VoteMap::iterator, VoteMap::iterator> vmip = voteMap_.equal_range(fromIndex(cid));
		while (vmip.first != vmip.second)
		{
			Ballot b = vmip.first->second;
			voteMap_.erase(vmip.first++);

			const CandidateId newVote = getNewVote(&b);
			if (newVote)
			{
				voteMap_.insert(std::make_pair(newVote, b));
				++votes_[makeIndex(newVote)];
			}
		}
	}

	std::vector<std::string> candidateNames_;
	std::set<CandidateId> eliminated_;
	Votes votes_;
	VoteMap voteMap_;
	VoteCt maxVotes_ = 0;
	VoteCt minVotes_ = std::numeric_limits<VoteCt>::max();
};

void doCase()
{
	AusVote v;
	v.getCandidates(std::cin);

	v.getVotes(std::cin);

	v.printWinner(std::cout);

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
		doCase();

		--numCases;
	}

	return 0;
}

