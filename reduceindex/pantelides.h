/*****************************************************************************

    This file is part of Modelica C Compiler.

    Modelica C Compiler is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Modelica C Compiler is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Modelica C Compiler.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/

#ifndef PANTELIDES_H
#define PANTELIDES_H

#include <map>
#include <ast/equation.h>
#include <causalize/graph/graph_definition.h>
#include <causalize/causalization_strategy.h>
#include <mmo/mmo_class.h>

using namespace Causalize;

namespace Pant {
class Pantelides : public CausalizationStrategy{
public:
  Pantelides(Modelica::MMO_Class &mmo_class);
  void ApplyPantelides();
private:
  bool MatchEquation(Equation f, std::set<Vertex> &coloured);
  void MakeGraphSets();
  std::set<UnknownVertex> unknownSet;
  std::set<EquationVertex> equationSet;
  std::map<UnknownVertex, UnknownVertex> varMap; //var to derivated var
  std::map<EquationVertex, EquationVertex> eqMap; //eq to derivated eq
  std::map<UnknownVertex, EquationVertex> assign; //var to eqs
};
}

#endif