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

#include <ast/queries.h>
#include <reduceindex/derived_names.h>
#include <boost/algorithm/string/predicate.hpp>

template <typename T>
std::set<T> setUnion(const std::set<T>& a, const std::set<T>& b)
{
  std::set<T> result = a;
  result.insert(b.begin(), b.end());
  return result;
}

namespace Modelica {

    using namespace boost;
    DerivedNamesExp::DerivedNamesExp(){};
    std::set<Name> DerivedNamesExp::operator()(Integer v) const {
      return std::set<Name>();
    }
    std::set<Name> DerivedNamesExp::operator()(Boolean v) const {
      return std::set<Name>();
    }
    std::set<Name> DerivedNamesExp::operator()(String v) const {
      return std::set<Name>();
    }
    std::set<Name> DerivedNamesExp::operator()(Name v) const {
      return std::set<Name>();
    }
    std::set<Name> DerivedNamesExp::operator()(Real v) const {
      return std::set<Name>();
    }
    std::set<Name> DerivedNamesExp::operator()(SubEnd v) const {
      return std::set<Name>();
    }
    std::set<Name> DerivedNamesExp::operator()(SubAll v) const {
      return std::set<Name>();
    }
    std::set<Name> DerivedNamesExp::operator()(BinOp v) const {
      Expression l=v.left(), r=v.right();
      return setUnion(ApplyThis(l), ApplyThis(r));
    }
    std::set<Name> DerivedNamesExp::operator()(UnaryOp v) const {
      Expression exp = v.exp();
      return ApplyThis(exp);
    }
    std::set<Name> DerivedNamesExp::operator()(IfExp v) const {
      return std::set<Name>();
    }
    std::set<Name> DerivedNamesExp::operator()(Range v) const {
      return std::set<Name>();
    }
    std::set<Name> DerivedNamesExp::operator()(Brace v) const {
      return std::set<Name>();
    }
    std::set<Name> DerivedNamesExp::operator()(Bracket v) const {
      return std::set<Name>();
    }


    std::set<Name> DerivedNamesExp::operator()(Call v) const {
      std::set<Name> s;
      if(boost::algorithm::starts_with(v.name(), "der")){
        ExpList args = v.args();
        Expression firstArg = args.front();
	if(is<Reference>(firstArg)){
          Reference r = get<Reference>(firstArg);
          RefTuple x = r.ref().front();
          Name name = boost::get<0>(x);
          s.insert(name);
          return s;
        }
      }
        return std::set<Name>();
    }
    std::set<Name> DerivedNamesExp::operator()(FunctionExp v) const {
      return std::set<Name>();
    }
    std::set<Name> DerivedNamesExp::operator()(ForExp v) const {
      return std::set<Name>();
    }
    std::set<Name> DerivedNamesExp::operator()(Named v) const {
      return std::set<Name>();
    }
    std::set<Name> DerivedNamesExp::operator()(Output v) const {
      return std::set<Name>();
    }
    std::set<Name> DerivedNamesExp::operator()(Reference v) const {
      return std::set<Name>();
    }


    DerivedNames::DerivedNames(): der_names_exp() {};
    std::set<Name> DerivedNames::operator()(Connect v) const {
      ERROR("DerivedNames in connect equation not implemented\n");
      return std::set<Name>();
    };
    std::set<Name> DerivedNames::operator()(Equality v) const {
      std::set<Name> left = Apply(der_names_exp, v.left_ref());
      std::set<Name> right = Apply(der_names_exp, v.right_ref());
      return setUnion(left, right);
    };
    std::set<Name> DerivedNames::operator()(IfEq v) const {
      ERROR("DerivedNames in if equation not implemented\n");
      return std::set<Name>();
    }
    std::set<Name> DerivedNames::operator()(CallEq v) const {
      ERROR("DerivedNames in call equation not implemented\n");
      return std::set<Name>();
    };
    std::set<Name> DerivedNames::operator()(ForEq v) const {
      ERROR("DerivedNames in for equation not implemented\n");
      return std::set<Name>();
    };
    std::set<Name> DerivedNames::operator()(WhenEq v) const {
      ERROR("DerivedNames in when equation not implemented\n");
      return std::set<Name>();
    }
}
