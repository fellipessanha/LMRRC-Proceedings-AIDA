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

#ifndef OPTFRAME_FCORE_FMOVE_HPP_
#define OPTFRAME_FCORE_FMOVE_HPP_

#include <functional>

#include <OptFrame/Move.hpp>

namespace optframe {

// Remember: on g++-10.1, if passing default function directly on template, it will complain!
// "function ... has no linkage"
//      bool (*fCanBeApplied)(const XES&) = [](const XES&) -> bool { return false; } // fCanBeApplied
//  So, looks like we need to have function in global scope, for it to have linkage.
//
template<class M, XESolution XES>
auto fDefaultCanBeApplied =
  [](const M&, const XES&) -> bool {
   return true;
};
//
template<class M, XESolution XES>
auto fDefaultCompareEq =
  [](const M& me, const Move<XES>& other) -> bool {
   return false;
};
//
template< class M, XESolution XES >
class FMove final : public Move<XES, typename XES::second_type>
{
   using XEv = typename XES::second_type;
   using XSH = XES; // only single objective
   using Self = FMove<M, XES>;

public:
   M m; // internal structure for move

   M (*fApply)(const M&, XES&);                                                // fApply
   bool (*fCanBeApplied)(const M&, const XES&) = fDefaultCanBeApplied<M, XES>; // fCanBeApplied
   bool (*fCompareEq)(const M&, const Move<XES>&) = fDefaultCompareEq<M, XES>;  // fCompareEq

   FMove(
      const M& _m,
      M (*_fApply)(const M&, XES&),                                                // fApply
      bool (*_fCanBeApplied)(const M&, const XES&) = fDefaultCanBeApplied<M, XES>, // fCanBeApplied
      bool (*_fCompareEq)(const M&, const Move<XES>&) = fDefaultCompareEq<M, XES>  // fCompareEq
   )
   :
      m{ _m },
      fApply{ _fApply},
      fCanBeApplied{ _fCanBeApplied},
      fCompareEq{ _fCompareEq }
   {
   }

   /*
   FMove(M&& _m) noexcept
     : m(std::move(_m))
   {
      //std::cout << "FMove() => " << m << " address=" << this << std::endl;
   }
   */

   /*
   FMove(const M& _m) noexcept
     : m(_m)
   {
      //std::cout << "FMove() => " << m << " address=" << this << std::endl;
   }
   */

   virtual bool canBeApplied(const XES& se) override
   {
      return fCanBeApplied(m, se);
   }

   virtual uptr<Move<XES, XEv, XSH>> apply(XSH& se) override
   {
      // fApply will require a reverse move to ALWAYS exist.
      // FCore must be simple! Otherwise, just use fallback class-based mode
      return uptr<Move<XES, XEv, XSH>>{ new Self{ fApply(m, se), fApply, fCanBeApplied, fCompareEq } };
   }

   virtual bool operator==(const Move<XES, XEv, XSH>& move) const override
   {
      const Move<XES>& move2 = (Move<XES>&)move;
      return fCompareEq(this->m, move2);
   }

   bool operator!=(const Move<XES, XEv, XSH>& m) const
   {
      return !(*this == m);
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":FMove";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }

   // use 'operator<<' for M
   virtual void print() const override
   {
      std::cout << m << std::endl;
   }
};

} // namespace optframe

#endif /*OPTFRAME_FCORE_FMOVE_HPP_*/
