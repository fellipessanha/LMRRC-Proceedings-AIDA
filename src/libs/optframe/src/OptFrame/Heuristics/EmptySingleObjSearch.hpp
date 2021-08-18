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

#ifndef OPTFRAME_EMPTY_SINGLE_OBJ_SEARCH_HPP_
#define OPTFRAME_EMPTY_SINGLE_OBJ_SEARCH_HPP_

#include <math.h>
#include <vector>

#include "../SingleObjSearch.hpp"

namespace optframe
{

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class EmptySingleObjSearch: public SingleObjSearch<XES>
{
public:

	EmptySingleObjSearch()
	{
	}

	virtual ~EmptySingleObjSearch()
	{
	}

	//pair<S, Evaluation<>>* search(StopCriteria<XEv>& sosc,  const S* _s = nullptr,  const Evaluation<>* _e = nullptr) override
   //virtual std::optional<pair<S, XEv>> search(StopCriteria<XEv>& stopCriteria) override
   //
   //SearchStatus search(const StopCriteria<XEv>& stopCriteria) override
   SearchOutput<XES> search(const StopCriteria<XEv>& stopCriteria) override
	{
		cout << "WARNING: RETURNING A EmptySingleObjSearch!" << endl;
		//return std::nullopt; // nothing to return
      return SearchStatus::NO_REPORT;
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<XES>::idComponent() << "empty";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_EMPTY_SINGLE_OBJ_SEARCH_HPP_*/
