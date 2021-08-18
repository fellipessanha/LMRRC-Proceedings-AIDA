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

#ifndef OPTFRAME_ILSL_HPP_
#define OPTFRAME_ILSL_HPP_

#include <math.h>
#include <vector>

#include "ILSLPerturbation.hpp"
#include "IteratedLocalSearch.hpp"

#include "ILS.h"

namespace optframe {

typedef pair<pair<int, int>, pair<int, int>> levelHistory;

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class IteratedLocalSearchLevels : public IteratedLocalSearch<levelHistory, XES, XEv>
{
protected:
   sref<LocalSearch<XES, XEv>> ls;
   sref<ILSLPerturbation<XES, XEv>> p;
   int iterMax, levelMax;

public:
   //IteratedLocalSearchLevels(Evaluator<XES, XEv>& e, Constructive<S>& constructive, LocalSearch<XES, XEv>& _ls, ILSLPerturbation<S, XEv>& _p, int _iterMax, int _levelMax) :
   IteratedLocalSearchLevels(sref<GeneralEvaluator<XES, XEv>> e, sref<InitialSearch<XES, XEv>> constructive, sref<LocalSearch<XES, XEv>> _ls, sref<ILSLPerturbation<XES, XEv>> _p, int _iterMax, int _levelMax)
     : IteratedLocalSearch<levelHistory, XES, XEv>(e, constructive)
     , ls(_ls)
     , p(_p)
     , iterMax(_iterMax)
     , levelMax(_levelMax)
   {
   }

   virtual ~IteratedLocalSearchLevels()
   {
   }

   virtual sref<levelHistory> initializeHistory()
   {
      //cout << "initializeHistory()" << endl;
      pair<int, int> vars(0, 0);

      // IterMax e LevelMax
      pair<int, int> maxs(iterMax, levelMax);

      return sref<levelHistory>(new levelHistory(vars, maxs));
   }

   virtual void localSearch(XES& se, const StopCriteria<XEv>& stopCriteria) override
   {
      //cout << "localSearch(.)" << endl;
      ls->searchFrom(se, stopCriteria);
   }

   virtual void perturbation(XES& se, const StopCriteria<XEv>& stopCriteria, sref<levelHistory> history) override
   {
      //cout << "perturbation(.)" << endl;

      int iter = history->first.first;
      int level = history->first.second;
      int iterMax = history->second.first;
      //int levelMax = history.second.second;

      //cout << "level = " << level << " e iter = " << iter << endl;

      // nivel atual: 'level'
      p->perturb(se, stopCriteria, level);

      // Incrementa a iteracao
      iter++;

      if (Component::debug)
         cout << "ILSL::iter " << iter << endl;

      if (iter >= iterMax) {
         iter = 0;
         level++;
         if (Component::information)
            cout << "ILSL::level " << level << ".." << endl;
      }

      // Atualiza o historico
      history->first.first = iter;
      history->first.second = level;
   }

   virtual bool acceptanceCriterion(const Evaluation<>& e1, const Evaluation<>& e2, sref<levelHistory> history)
   {
      //cout << "acceptanceCriterion(.)" << endl;

      //if (IteratedLocalSearch<levelHistory, XES, XEv>::evaluator.betterThan(e1, e2))
      //if (e1.betterStrict(e2))
      //std::cout << "ILSL will compare(" << e1.outdated << ";" << e2.outdated << ")" << std::endl;

      if (IteratedLocalSearch<levelHistory, XES, XEv>::evaluator->betterStrict(e1, e2)) {
         if (Component::information) {
            cout << "ILSL::Best fo: on [iter " << history->first.first << " of level " << history->first.second << "] => ";
            e1.print();
         }

         // =======================
         //  Atualiza o historico
         // =======================
         // iter = 0
         history->first.first = 0;
         // level = 0
         history->first.second = 0;

         // =======================
         //    Retorna s2
         // =======================
         return true;
      } else
         return false;
   }

   virtual bool terminationCondition(sref<levelHistory> history)
   {
      //cout << "terminationCondition(.)" << endl;
      int level = history->first.second;
      int levelMax = history->second.second;

      return (level >= levelMax);
   }

   virtual string id() const
   {
      return idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << IteratedLocalSearch<levelHistory, XES, XEv>::idComponent() << "ilsl";
      return ss.str();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class IteratedLocalSearchLevelsBuilder : public ILS
  , public SingleObjSearchBuilder<S, XEv, XES>
{
public:
   virtual ~IteratedLocalSearchLevelsBuilder()
   {
   }

   virtual SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      sptr<GeneralEvaluator<XES, XEv>> eval = nullptr;
      hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!
      if (!eval)
         return nullptr;

      //Constructive<S>* constructive = nullptr;
      sptr<InitialSearch<XES, XEv>> constructive = nullptr;
      hf.assign(constructive, *scanner.nextInt(), scanner.next()); // reads backwards!
      if (!constructive)
         return nullptr;

      string rest = scanner.rest();

      pair<sptr<LocalSearch<XES, XEv>>, std::string> method;
      method = hf.createLocalSearch(rest);

      sptr<LocalSearch<XES, XEv>> h = method.first;

      scanner = Scanner(method.second);
      if (!h)
         return nullptr;

      sptr<ILSLPerturbation<XES, XEv>> pert;
      hf.assign(pert, *scanner.nextInt(), scanner.next()); // reads backwards!
      if (!pert)
         return nullptr;

      int iterMax = -1;

      auto oiterMax = scanner.nextInt();

      if (!oiterMax) {
         return nullptr;
      }

      iterMax = *oiterMax;

      int levelMax = -1;

      auto olevelMax = scanner.nextInt();

      if (!olevelMax) {
         return nullptr;
      }

      levelMax = *olevelMax;

      return new IteratedLocalSearchLevels<XES, XEv>(eval, constructive, h, pert, iterMax, levelMax);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(), "evaluation function"));
      //params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));
      params.push_back(make_pair(InitialSearch<XES, XEv>::idComponent(), "constructive heuristic"));
      params.push_back(make_pair(LocalSearch<XES, XEv>::idComponent(), "local search"));
      params.push_back(make_pair(ILSLPerturbation<XES, XEv>::idComponent(), "ilsL perturbation"));
      params.push_back(make_pair("int", "max number of iterations without improvement"));
      params.push_back(make_pair("int", "levelMax of perturbation"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == IteratedLocalSearchLevels<XES, XEv>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << SingleObjSearchBuilder<S, XEv>::idComponent() << ILS::family() << "ILSLevels";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

}

#endif /*OPTFRAME_ILSL_HPP_*/
