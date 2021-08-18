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

#ifndef OPTFRAME_COMPONENT_HELPER_HPP_
#define OPTFRAME_COMPONENT_HELPER_HPP_

#include <cstdlib>
#include <iostream>
#include <vector>

#include "Scanner++/Scanner.hpp"
using namespace scannerpp;

namespace optframe {

class ComponentHelper
{
public:
 
   // taken from HeuristicFactory

   //! \english compareBase is an auxiliar method to compare a pattern to a component id. Check if 'component' inherits from 'base'. \endenglish \portuguese compareBase eh um metodo auxiliar para comparar um padrao a um id de componente. Testa se 'component' herda de 'base'. \endportuguese
   /*!
	 \sa compareBase(string, string)
	 */

   // Check if 'base' is inherited by 'component'
   // EXAMPLE: compareBase("OptFrame:", "OptFrame:Evaluator") returns TRUE!
   static bool compareBase(string _base, string _component)
   {
      if ((_base.length() < 3) || (_component.length() < 3)) {
         cout << "ComponentHelper::compareBase warning: comparing less than 3 characters! with base='" << _base << "' component='" << _component << "'" << endl;
         return false;
      }

      bool baseIsList = (_base.at(_base.length() - 2) == '[') && (_base.at(_base.length() - 1) == ']');
      bool componentIsList = (_component.at(_component.length() - 2) == '[') && (_component.at(_component.length() - 1) == ']');

      if (baseIsList != componentIsList)
         return false;

      // remove list (if exists)
      string base = typeOfList(_base);
      string component = typeOfList(_component);

      bool sameBase = true;

      if (base.length() <= component.length()) {
         for (unsigned i = 0; i < base.length(); i++)
            if (base.at(i) != component.at(i))
               sameBase = false;
      } else
         sameBase = false;

      if (sameBase)
         return true;

      // ------------------
      // check last family
      // ------------------

      Scanner scanner(base);
      scanner.useSeparators(":");

      string family = scanner.next();
      while (scanner.hasNext())
         family = scanner.next();

      Scanner scanComponent(component);
      scanComponent.useSeparators(":");
      string part;
      while (scanComponent.hasNext()) {
         part = scanComponent.next();
         if (part == family)
            sameBase = true;
      }

      return sameBase;
   }

   static string typeOfList(string listId)
   {
      Scanner scanner(listId);
      scanner.useSeparators(" \t\n[]");

      return scanner.next();
   }
};

}

#endif /* OPTFRAME_COMPONENT_HPP_ */
