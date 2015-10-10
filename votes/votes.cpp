#include <cassert>
#include <iostream>
#include <limits>
#include <map>
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
	}

	// determine winner
	void printWinner(std::ostream &os)
	{
		while (maxVotes_ < (voteMap_.size() + 1) / 2)
		{
			reduceCandidates();
		}

		//foreach candidate
		for (size_t i = 0; i < votes_.size(); ++i)
		{
			// if candidate has most votes
			if (votes_[i] == maxVotes_)
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

	void reduceCandidates()
	{
		for (size_t i = 0; i < votes_.size(); ++i)
		{
			if (votes_[i] == minVotes_)
				eliminateCandidate(i);
		}

		// recalculate min and max
		maxVotes_ = 0;
		minVotes_ = std::numeric_limits<VoteCt>::max();
		for (size_t i = 0; i < votes_.size(); ++i)
		{
			if (votes_[i] == 0)
				continue;

			maxVotes_ = std::max(maxVotes_, votes_[i]);
			minVotes_ = std::min(minVotes_, votes_[i]);
		}
	}

	void eliminateCandidate(CandidateId cid)
	{
#ifdef OUTPUT
		std::cout << "Reduce " << candidateNames_[cid] << std::endl;
#endif
		votes_[cid] = 0;

		VoteMap::iterator vmi = voteMap_.find(fromIndex(cid));
		while (vmi != voteMap_.end())
		{
			Ballot b = vmi->second;
			voteMap_.erase(vmi);

			if (!b.empty())
			{
				const CandidateId firstVote = b[0];
#ifdef OUTPUT
				std::cout << "New vote for " << firstVote << std::endl;
#endif

				std::move(b.begin() + 1, b.end(), b.begin());

				voteMap_.insert(std::make_pair(firstVote, b));
				++votes_[makeIndex(firstVote)];
			}

			vmi = voteMap_.find(fromIndex(cid));
		}
	}

	std::vector<std::string> candidateNames_;
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

