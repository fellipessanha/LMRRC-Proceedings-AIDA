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

#ifndef OPTFRAME_GENERAL_EVALUATOR_HPP_
#define OPTFRAME_GENERAL_EVALUATOR_HPP_

#include "ADSManager.hpp"
#include "Evaluation.hpp"
#include "Move.hpp"
#include "MoveCost.hpp"
#include "Solution.hpp"

#include <iostream>

#include "Action.hpp"
#include "Direction.hpp"

#include "Solutions/ESolution.hpp" // TODO: remove.. just to enforce compilation errors.

// This evaluator intends to run for both Single and MultiObj

using namespace std;
using namespace scannerpp;

namespace optframe {

// This will have no 'weights', as it doesn't make sense for most cases...
// A special WeightedEvaluator is more welcome than putting it on a General one

//template<XSolution S, XEvaluation XEv, XSearch<S, XEv> XSH>
//template<XSolution S, XEvaluation XEv, XESolution XES, XSearch<XES> XSH = XES> // defaults to single obj.
template<XESolution XES, XEvaluation XEv = Evaluation<>, XSearch<XES> XSH = XES> // defaults to single obj.
// BREAK TIME!! Only 'XES' shall pass!!
class GeneralEvaluator //: public Component // SHOULD BE PURE HERE, OTHERWISE BREAKS DIAMOND PROBLEMS!
{
   //using S = decltype(declval<XES>.first); // error: insufficient contextual information to determine type
   //
// could contain Direction here, or MultiDirection... but both should be abolished I guess.
// Where should betterThan decision take place? On Evaluator or on each Evaluation? 
// Before, it was hard/impossible to do on Evaluation, due to overhead... but now, it may be the case.
// If XEvaluation actually represents Objective Space, it should also contain guiding directions.

public:
   GeneralEvaluator()
   {
   }

   virtual ~GeneralEvaluator()
   {
   }

public:
   // general evaluation for Search Space element.
   // now user is free to update evaluation, and even solution during the process.
   // note that intention is to only change evaluation, but now it's up to user.
   virtual void reevaluate(XSH&) = 0;  // TODO: rename to something more general, 'xevaluate' ?


   // ====================
   // direction primitives
   // ====================

   // this strictly better than parameter 'e' (for mini, 'this' < 'e')
   virtual bool betterStrict(const XEv& e1, const XEv& e2) = 0;


   // this non-strictly better than parameter 'e' (for mini, 'this' <= 'e')
   virtual bool betterNonStrict(const XEv& e1, const XEv& e2)
   {
      return betterStrict(e1, e2) || equals(e1, e2);
   }

   // returns 'true' if this 'cost' (represented by this Evaluation) is improvement
   virtual bool isStrictImprovement(const XEv& e) = 0;

   virtual bool isImprovingStrictly(const XEv& cost, const XEv& e1, const XEv& e2)
   {
      XEv _e1(e1);
      cost.update(_e1); // _e1 := e1 + cost
      return betterStrict(_e1, e2);
   }

   // returns 'true' if this 'cost' (represented by this Evaluation) is improvement
   virtual bool isNonStrictImprovement(const XEv& e) = 0;

   virtual bool equals(const XEv& e1, const XEv& e2) = 0;

   // ==========================

   // Apply movement considering a previous XEv => Faster (used on CheckCommand and locally)
   // Update XEv 'e'
   //Move<XES, XEv, XSH>* applyMoveReevaluate(XEv& e, Move<XES, XEv, XSH>& m, S& s)
   uptr<Move<XES, XEv, XSH>> applyMoveReevaluate(Move<XES, XEv, XSH>& m, XSH& se)
   {
      // apply move and get reverse move
      uptr<Move<XES, XEv, XSH>> rev = m.applyUpdate(se);
      // for now, must be not nullptr
      assert(rev != nullptr);
      // consolidate 'outdated' XEv data on 'e'
      reevaluate(se);

      // create pair
      return rev;
   }


//
   //MoveCost<>* moveCost(Move<XES, XEv>& m, XES& se, bool allowEstimated = false)
   op<XEv> moveCost(Move<XES, XEv>& m, XES& se, bool allowEstimated = false)
   {
      // TODO: in the future, consider 'allowEstimated' parameter
      // TODO: in the future, consider 'e' and 's' as 'const', and use 'const_cast' to remove it.

      //MoveCost<>* p = nullptr;
      op<XEv> p = std::nullopt;
      //if (allowCosts) {
         p = m.cost(se, allowEstimated);
      //}

      // if p not null, do not update 's' => much faster (using cost)
      if (p) {
         return p;
      } else {
         // need to update 's' together with reevaluation of 'e' => slower (may perform reevaluation)

         // TODO: in the future, consider moves with nullptr reverse (must save original solution/evaluation)
         assert(m.hasReverse());

         //XSolution& s = se.first;
         XEv& e = se.second;

         XEv ev_begin(e); // copy
         //XEv ev_begin = e; //TODO: VITOR removing last evaluation
         // saving 'outdated' status to avoid inefficient re-evaluations
         //			bool outdated = e.outdated;
         // apply move to both XEv and Solution
         uptr<Move<XES, XEv>> rev = this->applyMoveReevaluate(m, se);

         XEv e_end(se.second);

/*
         // get final values
         pair<evtype, evtype> e_end = make_pair(e.getObjFunction(), e.getInfMeasure());
         // get final values for lexicographic part
         vector<pair<evtype, evtype>> alternatives(e.getAlternativeCosts().size());
         for (unsigned i = 0; i < alternatives.size(); i++) {
            alternatives[i].first = e.getAlternativeCosts()[i].first;
            alternatives[i].second = e.getAlternativeCosts()[i].second;
         }
*/
         // apply reverse move in order to get the original solution back
         //TODO - Why do not save ev at the begin? Extra evaluation
         //Even when reevaluate is implemented, It would be hard to design a strategy that is faster than copying previous evaluation//==================================================================
         //			Move<XES, XEv>* ini = applyMoveReevaluate(e, *rev, s);
         //
         //			// if XEv wasn't 'outdated' before, restore its previous status
         //			if (!outdated)
         //				e.outdated = outdated;

         

         uptr<Move<XES, XEv>> ini = rev->apply(se);


         XEv mcost = ev_begin.diff(e_end);

         // TODO: why?
         // for now, must be not nullptr
         assert(ini != nullptr);
         // TODO: include management for 'false' hasReverse()
         assert(rev->hasReverse() && ini);

         // recover original evaluation
         e = std::move(ev_begin);
         //==================================================================
/*
         // get original values (also could be calculated in the begin of function)
         pair<evtype, evtype> e_ini = make_pair(e.getObjFunction(), e.getInfMeasure());
         // do the same for lexicographic part
         for (unsigned i = 0; i < alternatives.size(); i++) {
            alternatives[i].first -= e.getAlternativeCosts()[i].first;
            alternatives[i].second -= e.getAlternativeCosts()[i].second;
         }
*/
         // destroy reverse move
         /////delete rev;
         // destroy initial move
         /////delete ini;
         // create a MoveCost object...

         // TODO: try to move this MoveCost generation somewhere else.... 
         // perhaps, in Evaluation class, so that users can easily personalize it.
         // don't know for sure. (TODO)
         //p = new MoveCost<>(e_end.first - e_ini.first, e_end.second - e_ini.second, e.weight);
         
         //
         //p = new MoveCost<>(e_end.first - e_ini.first, e_end.second - e_ini.second);
         
         // ... and set the lexicographic costs
         ////p->setAlternativeCosts(alternatives);

         // return a MoveCost object pointer
         //p = make_optional(Evaluation<>(e_end.first - e_ini.first, e_end.second - e_ini.second));
         //return p;
         return make_optional(mcost);
      }
   }

   // TODO: decide which methods stay here, and which go to Evaluator.hpp


   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":GeneralEvaluator";
      return ss.str();
   }

};

} // namespace optframe

#endif /*OPTFRAME_GENERAL_EVALUATOR_HPP_*/
