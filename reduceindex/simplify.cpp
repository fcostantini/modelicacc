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
#include <reduceindex/simplify.h>

namespace Modelica {

    using namespace boost;
    SimplifyExpression::SimplifyExpression(){};
    Expression SimplifyExpression::operator()(Integer v) const { 
      return v;
    }
    Expression SimplifyExpression::operator()(Boolean v) const { 
      return v;
    }
    Expression SimplifyExpression::operator()(String v) const {
      return v;
    }
    Expression SimplifyExpression::operator()(Name v) const { 
      return v;
    }
    Expression SimplifyExpression::operator()(Real v) const { 
      return v;
    }
    Expression SimplifyExpression::operator()(SubEnd v) const { 
      return v;
    }
    Expression SimplifyExpression::operator()(SubAll v) const { 
      return v;
    }
    Expression SimplifyExpression::operator()(BinOp v) const { 
      return v;
    } 
    Expression SimplifyExpression::operator()(UnaryOp v) const { 
      return v;
    } 
    Expression SimplifyExpression::operator()(IfExp v) const { 
      return v;
    }
    Expression SimplifyExpression::operator()(Range v) const { 
      return v;
    }
    Expression SimplifyExpression::operator()(Brace v) const { 
      return v;
    }
    Expression SimplifyExpression::operator()(Bracket v) const { 
      return v;
    }
    Expression SimplifyExpression::operator()(Call v) const {
      if ("der"==v.name()) {
        std::cout << "Simplify der...\n";
      }
      return v;
    }
    Expression SimplifyExpression::operator()(FunctionExp v) const { 
      return v;
    }
    Expression SimplifyExpression::operator()(ForExp v) const {
      return v;
    }
    Expression SimplifyExpression::operator()(Named v) const {
      return v;
    }
    Expression SimplifyExpression::operator()(Output v) const {
      return v;
    }
    Expression SimplifyExpression::operator()(Reference v) const {
      return v;
    }


    SimplifyEquation::SimplifyEquation(): simplify_exp() {};
    Equation SimplifyEquation::operator()(Connect v) const {
      ERROR("Simplify in connect equation not implemented\n");
      return v;
    };
    Equation SimplifyEquation::operator()(Equality v) const {
      v.left_ref()=Apply(simplify_exp, v.left_ref());
      v.right_ref()=Apply(simplify_exp, v.right_ref());
      return v;
    };
    Equation SimplifyEquation::operator()(IfEq v) const {
      ERROR("Simplify in if equation not implemented\n");
      return v;
    }
    Equation SimplifyEquation::operator()(CallEq v) const {
      ERROR("Simplify in call equation not implemented\n");
      return v;
    };
    Equation SimplifyEquation::operator()(ForEq v) const {
      ERROR("Simplify in for equation not implemented\n");
      return v;
    };
    Equation SimplifyEquation::operator()(WhenEq v) const {
      ERROR("Simplify in when equation not implemented\n");
      return v;
    }
}