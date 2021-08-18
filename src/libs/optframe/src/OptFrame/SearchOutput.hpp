#pragma once

#include "BaseConcepts.hpp"
#include "SearchStatus.hpp"

namespace optframe {

template<XESolution XES, XSearch<XES> XSH = XES>
class SearchOutput
{
public:
   SearchStatus status;
   std::optional<XSH> best;

   // two real elements (copy best)
   SearchOutput(SearchStatus status, std::optional<XSH>& best)
     : status{ status }
     , best{ best }
   {}

   // move semantics or nothing
   SearchOutput(SearchStatus status, std::optional<XSH>&& best = std::nullopt)
     : status{ status }
     , best{ best }
   {}

}; // SearchOutput

} // namespace optframe