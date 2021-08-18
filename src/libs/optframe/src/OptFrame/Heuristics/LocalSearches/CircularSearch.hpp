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

#ifndef OPTFRAME_CS_HPP_
#define OPTFRAME_CS_HPP_

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NSEnum.hpp"

namespace optframe {

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class CircularSearch : public LocalSearch<XES, XEv>
{
private:
   sref<GeneralEvaluator<XES, XEv>> eval;
   sref<NSEnum<XES, XEv>> ns;

   int initial_w;

public:
   CircularSearch(sref<GeneralEvaluator<XES, XEv>> _eval, sref<NSEnum<XES, XEv>> _nsEnum)
     : eval(_eval)
     , ns(_nsEnum)
   {
      initial_w = 0;
   }

   virtual ~CircularSearch()
   {
   }

   // DEPRECATED
	//virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
	//{
	//	Evaluation<> e = std::move(ev.evaluate(s));
	//	exec(s, e, stopCriteria);
	//}

	virtual SearchStatus searchFrom(XES& se, const StopCriteria<XEv>& sosc) override
	{
      //XSolution& s = se.first;
      //XEv& e = se.second;
      
      //double timelimit = sosc.timelimit;
      //double target_f = sosc.target_f;
      int Wmax = ns->size();

      int w = initial_w % Wmax;

      do {
         uptr<Move<XES, XEv>> m = ns->indexMove(w);

         //if (m->canBeApplied(s)) {
         if (m->canBeApplied(se)) {
            bool mayEstimate = false;
            ///MoveCost<>& cost = *eval->moveCost(m, se, mayEstimate);
            op<XEv> cost = eval->moveCost(*m, se, mayEstimate);

            //if (eval->isImprovement(*cost)) {
            //if (cost->isImprovingStrict()) {
            if (eval->isStrictImprovement(*cost)) {
               //double old_f = e.evaluation();

               m->applyUpdate(se);
               eval->reevaluate(se); // updates 'e'

               //cout << "CS improvement! w:" << w << " fo=" << e.evaluation() << " (antiga fo="<< old_f << ")" << endl << endl;

               initial_w = w + 1;

               return SearchStatus::IMPROVEMENT;
            }
         }

         w = (w + 1) % Wmax;
      } while (w != initial_w);
      return SearchStatus::NO_REPORT;
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearch<XES, XEv>::idComponent() << ":CS";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class CircularSearchBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~CircularSearchBuilder()
   {
   }

   virtual LocalSearch<XES, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      sptr<GeneralEvaluator<XES, XEv>> eval;
      hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

      sptr<NSEnum<XES, XEv>> nsenum;
      hf.assign(nsenum, *scanner.nextInt(), scanner.next()); // reads backwards!

      return new CircularSearch<XES, XEv>(eval, nsenum);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(), "evaluation function"));
      params.push_back(make_pair(NSEnum<XES, XEv>::idComponent(), "neighborhood structure"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == CircularSearch<XES, XEv>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearchBuilder<S, XEv>::idComponent() << ":CS";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};
}

#endif /*OPTFRAME_CS_HPP_*/
