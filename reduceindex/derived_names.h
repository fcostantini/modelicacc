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

#ifndef DERIVED_NAMES
#define DERIVED_NAMES
#include <boost/variant/static_visitor.hpp>
#include <ast/expression.h>
#include <ast/equation.h>

namespace Modelica {

  using namespace Modelica::AST;
  class DerivedNamesExp: public boost::static_visitor<Expression> {
  public:
    DerivedNamesExp();
    std::set<Name> operator()(Integer v) const;
    std::set<Name> operator()(Boolean v) const;
    std::set<Name> operator()(String v) const;
    std::set<Name> operator()(Name v) const;
    std::set<Name> operator()(Real v) const;
    std::set<Name> operator()(SubEnd v) const;
    std::set<Name> operator()(SubAll v) const;
    std::set<Name> operator()(BinOp) const;
    std::set<Name> operator()(UnaryOp) const;
    std::set<Name> operator()(Brace) const;
    std::set<Name> operator()(Bracket) const;
    std::set<Name> operator()(Call) const;
    std::set<Name> operator()(FunctionExp) const;
    std::set<Name> operator()(ForExp) const;
    std::set<Name> operator()(IfExp) const;
    std::set<Name> operator()(Named) const;
    std::set<Name> operator()(Output) const;
    std::set<Name> operator()(Reference) const;
    std::set<Name> operator()(Range) const;
  };

  class DerivedNmes: public boost::static_visitor<Equation> {
  public:
    DerivedNames();
    std::set<Name> operator()(Connect) const;
    std::set<Name> operator()(Equality) const;
    std::set<Name> operator()(CallEq) const;
    std::set<Name> operator()(ForEq) const;
    std::set<Name> operator()(IfEq) const;
    std::set<Name> operator()(WhenEq) const;

    DerivedNamesExp der_names_exp;
  };
}
#endif