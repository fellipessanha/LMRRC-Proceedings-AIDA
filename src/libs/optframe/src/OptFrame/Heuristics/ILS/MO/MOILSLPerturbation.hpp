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

#ifndef OPTFRAME_MOILSLPerturbation_HPP_
#define OPTFRAME_MOILSLPerturbation_HPP_

#include <math.h>
#include <vector>

#include "../../../NS.hpp"
#include "../../../RandGen.hpp"
#include "../../../MultiObjSearch.hpp"


#include "MOILS.h"

namespace optframe
{

//template<XSolution S, XEvaluation XEv=Evaluation<>>
template<XESolution XMES, XEvaluation XMEv=MultiEvaluation<>>
class MOILSLPerturbation: public Component, public MOILS
{
public:

	virtual ~MOILSLPerturbation()
	{
	}

	virtual void perturb(XMES& smev, const StopCriteria<XMEv>& stopCriteria, int level) = 0;

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << MOILS::family() << "LevelPert";
		return ss.str();

	}
};

//template<XSolution S, XEvaluation XEv=Evaluation<>, XEvaluation XMEv=MultiEvaluation<>, XESolution XMES = pair<S, XMEv>>
template<XESolution XMES, XEvaluation XMEv=MultiEvaluation<>>
class MOILSLPerturbationLPlus2: public MOILSLPerturbation<XMES, XMEv>
{
private:
	vector<NS<XMES, XMEv>*> ns;
	//MultiEvaluator<S, XEv, XMEv, XMES>& evaluator;
   GeneralEvaluator<XMES, XMEv>& evaluator;
	RandGen& rg;

public:
	//MOILSLPerturbationLPlus2(MultiEvaluator<S, XEv, XMEv, XMES>& _e, NS<XMES, XMEv>& _ns, RandGen& _rg) :
   MOILSLPerturbationLPlus2(GeneralEvaluator<XMES, XMEv>& _e, NS<XMES, XMEv>& _ns, RandGen& _rg) :
			evaluator(_e), rg(_rg)
	{
		ns.push_back(&_ns);
	}

	virtual ~MOILSLPerturbationLPlus2()
	{
	}

	void add_ns(NS<XMES, XMEv>& _ns)
	{
		ns.push_back(&_ns);
	}

	//void perturb(S& s, XMEv& mev, const StopCriteria<XEv>& stopCriteria, int level)
   void perturb(XMES& smev, const StopCriteria<XMEv>& stopCriteria, int level) override
	{
      //XES se = make_pair(s, Evaluation<>()); // TODO: multiev

		int a = 0; // number of appliable moves

		level += 2; // level 0 applies 2 moves

		while (a < level)
		{
			int x = rg.rand(ns.size());

			uptr<Move<XMES, XMEv>> m = ns[x]->validRandomMove(smev);

			if (m)
			{
				a++;
				//Component::safe_delete(m->applyMEVUpdate(mev, s));
            //m->applyMEVUpdate(mev, se);
            m->applyUpdate(smev);
			}
			else
				if(Component::warning)
					cout << "ILS Warning: perturbation had no effect in level " << a << "!" << endl;

			//delete m;
		}

		evaluator.reevaluate(smev); // updates 'mev'
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (MOILSLPerturbation<XMES, XMEv>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << MOILSLPerturbation<XMES, XMEv>::idComponent() << ":LPlus2";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};



//template<XSolution S, XEvaluation XEv=Evaluation<>, XESolution XES = pair<S, XEv>>
template<XSolution S, XEvaluation XEv=Evaluation<>, XEvaluation XMEv=MultiEvaluation<>, XESolution XMES = pair<S, XMEv>>
class MOILSLPerturbationLPlus2Prob: public MOILSLPerturbation<XMES, XMEv>
{
private:
	vector<NS<XMES, XMEv>*> ns;
	vector<pair<int, double> > pNS;
	MultiEvaluator<S, XEv, XMEv, XMES>& evaluator;
	RandGen& rg;

public:
	MOILSLPerturbationLPlus2Prob(MultiEvaluator<S, XEv, XMEv, XMES>& _e, NS<XMES, XMEv>& _ns, RandGen& _rg) :
			evaluator(_e), rg(_rg)
	{
		ns.push_back(&_ns);
		pNS.push_back(make_pair(1, 1));
	}

	virtual ~MOILSLPerturbationLPlus2Prob()
	{
	}

	void add_ns(NS<XMES, XMEv>& _ns)
	{
		ns.push_back(&_ns);
		pNS.push_back(make_pair(1, 1));

		double soma = 0;
		for (int i = 0; i < ns.size(); i++)
			soma += pNS[i].first;
		for (int i = 0; i < ns.size(); i++)
			pNS[i].second = pNS[i].first / soma;
	}

	void changeProb(vector<int> pri)
	{
		int nNeighborhoods = ns.size();
		if (pri.size() != nNeighborhoods)
		{
			cout << "ERROR ON PRIORITES SIZE!" << endl;
			return;
		}

		double soma = 0;
		for (int i = 0; i < nNeighborhoods; i++)
		{
			pNS[i].first = pri[i];
			soma += pri[i];
		}

		for (int i = 0; i < ns.size(); i++)
			pNS[i].second = pNS[i].first / soma;

		cout<<"Printing probabilities ILSLPerturbationLPlus2Prob:"<<endl;
		for (int i = 0; i < ns.size(); i++)
			cout << "pNS[i].first: " << pNS[i].first << "\t pNS[i].second: " << pNS[i].second << endl;
		cout<<endl;
	}

	//void perturb(S& s, MultiEvaluation<>& mev, const StopCriteria<XEv>& stopCriteria, int level) override
   void perturb(XMES& smev, const StopCriteria<XMEv>& stopCriteria, int level) override
	{
		int a = 0; // number of appliable moves

		level += 2; // level 0 applies 2 moves

		while (a < level)
		{
			double prob = rg.rand01();
			int x = 0;
			double sum = pNS[x].second;

			while (prob > sum)
			{
				x++;
				sum += pNS[x].second;
			}

			Move<S, XEv>* m = ns[x]->validMove(smev);

			if (m)
			{
				a++;
				//Component::safe_delete(m->applyMEVUpdateSolution(mev, s));
            m->applyUpdate(smev);
			}
			else
				if(Component::warning)
					cout << "ILS Warning: perturbation had no effect in level " << a << "!" << endl;

			delete m;
		}

		//evaluator.reevaluateMEV(mev, s); // updates 'e'
      evaluator.reevaluate(smev); // updates 'mev'
	}

	static string idComponent()
	{
		stringstream ss;
		ss << MOILSLPerturbation<XMES, XMEv>::idComponent() << ":LPlus2Prob";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

} // namespace optframe

#endif /*OPTFRAME_MOILSLPerturbation_HPP_*/
