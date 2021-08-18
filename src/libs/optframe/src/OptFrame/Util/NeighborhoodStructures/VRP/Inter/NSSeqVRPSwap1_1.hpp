// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.


#ifndef OPTFRAME_NSSeqVRPSwap1_1_HPP_
#define OPTFRAME_NSSeqVRPSwap1_1_HPP_
/*
 Classical Problem: Vehicle Routing Problem

 */

// Framework includes
#include "../../../../Move.hpp"
#include "../../../../NSSeq.hpp"

using namespace std;

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS>
template<class T, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<vector<vector<T> >,ADS> S = CopySolution<vector<vector<T>>,ADS>, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class MoveVRPSwap1_1: public Move<XES, XEv>
{
	typedef vector<vector<T> > Routes;

protected:
	int r1, r2; // route 1 and route 2, respectively
	int c1, c2; // client in route 1 and client in route 2, respectively

	OPTFRAME_DEFAULT_PROBLEM* problem;
public:

	MoveVRPSwap1_1(int _r1, int _r2, int _c1, int _c2, OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr) :
		r1(_r1), r2(_r2), c1(_c1), c2(_c2), problem(_problem)
	{
	}

	virtual ~MoveVRPSwap1_1()
	{
	}

	int get_r1()
	{
		return r1;
	}

	int get_r2()
	{
		return r2;
	}

	int get_c1()
	{
		return c1;
	}

	int get_c2()
	{
		return c2;
	}

	virtual bool canBeApplied(const XES& se) override
	{
      const Routes& rep = se.first.getR();
		bool all_positive = (r1 >= 0) && (r2 >= 0) && (c1 >= 0) && (c2 >= 0);
		return all_positive && (rep.size() >= 2);
	}

	virtual void updateNeighStatus(ADS& ads)
	{

	}

	virtual uptr<Move<XES>> apply(XES& se) override
	{
      Routes& rep = se.first.getR();
		T aux;
		aux = rep.at(r1).at(c1);
		rep.at(r1).at(c1) = rep.at(r2).at(c2);
		rep.at(r2).at(c2) = aux;

		return uptr<Move<XES>>(new MoveVRPSwap1_1(r2, r1, c2, c1));
	}

	virtual bool operator==(const Move<XES>& _m) const
	{
		const MoveVRPSwap1_1& m1 = (const MoveVRPSwap1_1&) _m;
		return ((m1.r1 == r1) && (m1.r2 == r2) && (m1.c1 == c1) && (m1.c2 == c2));
	}

	void print() const
	{
		cout << "MoveVRPSwap1_1( ";
		cout << " route[" << r1 << "] client " << c1 << " <=>  route[" << r2 << "] client " << c2 << ")";
		cout << endl;
	}
};

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveVRPSwap1_1<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM>
template<class T, class ADS, XBaseSolution<vector<vector<T>>,ADS> S, class MOVE = MoveVRPSwap1_1<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class NSIteratorVRPSwap1_1: public NSIterator<XES, XEv>
{

	typedef vector<vector<T> > Routes;

protected:
	uptr<Move<XES>> m;
	vector<uptr<Move<XES>>> moves;
	int index; //index of moves
	const Routes& r;

	P* p; // has to be the last

public:

	NSIteratorVRPSwap1_1(const Routes& _r, const ADS& _ads, P* _p = nullptr) :
		r(_r), p(_p)
	{

		m = nullptr;
		index = 0;
	}

	virtual ~NSIteratorVRPSwap1_1()
	{
	}

	virtual void first() override
	{

		for (int r1 = 0; r1 < r.size() - 2; r1++)
			for (int c1 = 0; c1 < r.at(r1).size(); c1++)
				for (int r2 = r1 + 1; r2 < r.size() - 1; r2++)
					for (int c2 = 0; c2 < r.at(r2).size(); c2++)
						moves.push_back(uptr<Move<XES>>(new MOVE(r1, r2, c1, c2, p)));

		if (moves.size() > 0)
		{
			m = std::move(moves[index]); // stealing from vector... verify if this is correct! otherwise, must need clone() on Move
		}
		else
			m = nullptr;

	}

	virtual void next() override
	{
		index++;
		if (index < moves.size())
		{
			m = std::move(moves[index]); // stealing from vector... verify if this is correct! otherwise, must need clone() on Move
		}
		else
			m = nullptr;

	}

	virtual bool isDone() override
	{
		return m == nullptr;
	}

	virtual uptr<Move<XES>> current() override
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSSeqVRPSwap1_1. Aborting." << endl;
			exit(1);
		}

      uptr<Move<XES>> m2 = std::move(m);

		return m2;
	}
};

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveVRPSwap1_1<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRPSwap1_1<T, ADS, MOVE, P> >
template<class T, class ADS, XBaseSolution<vector<vector<T> >,ADS> S, class MOVE = MoveVRPSwap1_1<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRPSwap1_1<T, ADS, S, MOVE, P>, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, XSearch<XES> XSH = std::pair<S, XEv>>
class NSSeqVRPSwap1_1: public NSSeq<XES, XEv, XSH>
{

	typedef vector<vector<T> > Routes;

private:
	P* p; // has to be the last

public:

	NSSeqVRPSwap1_1(P* _p = nullptr) :
		p(_p)
	{
	}

	virtual ~NSSeqVRPSwap1_1()
	{
	}

	uptr<Move<XES>> randomMove(const XES& se) override
	{
      const Routes& rep = se.first.getR();
		//getchar();
		if (rep.size() < 2)
			return uptr<Move<XES>>(new MOVE(-1, -1, -1, -1, p));

		int r1 = rand() % rep.size();

		if (rep.at(r1).size() == 0)
			return uptr<Move<XES>>(new MOVE(-1, -1, -1, -1, p));

		int r2;

		do
		{
			r2 = rand() % rep.size();
		} while (r1 == r2);

		if (rep.at(r2).size() == 0)
			return uptr<Move<XES>>(new MOVE(-1, -1, -1, -1, p));

		int c1 = rand() % rep.at(r1).size();
		int c2 = rand() % rep.at(r2).size();

		return uptr<Move<XES>>(new MOVE(r1, r2, c1, c2, p));
	}

	virtual uptr<NSIterator<XES>> getIterator(const XES& se) override
	{
      XSolution& s = se.first;
		return uptr<NSIterator<XES>>(new NSITERATOR(s.getR(), s.getADS(), p));
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqVRPSwap1_1 with move: " << MOVE::idComponent();
		return ss.str();
	}

	virtual void print()
	{
		cout << "NSSeqVRPSwap1_1 with move: " << MOVE::idComponent();
	}
};

#endif /*OPTFRAME_NSSeqVRPSwap1_1_HPP_*/
