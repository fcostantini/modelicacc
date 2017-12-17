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

#include <reduceindex/pantelides.h>
#include <causalize/causalization_strategy.h>
#include <causalize/graph/graph_definition.h>
#include <causalize/graph/graph_printer.h>
#include <ast/ast_types.h>
#include <causalize/for_unrolling/process_for_equations.h>
#include <util/debug.h>
#include <util/derivate.h>
#include <util/derivate_equality.h>
#include <reduceindex/variables_collector.h>
#include <boost/lambda/lambda.hpp>
#include <ast/equation.h>
#include <boost/variant/get.hpp>
#include <mmo/mmo_class.h>
#include <util/ast_visitors/contains_expression.h>
#include <util/ast_visitors/partial_eval_expression.h>
#include <util/solve/solve.h>
#include <fstream>

#include "variables_collector.h"

using namespace Modelica::AST;
using namespace Causalize;

namespace Pant {
Pantelides::Pantelides(MMO_Class &mmo_class): Causalize::CausalizationStrategy(mmo_class, false) {

  process_for_equations(mmo_class);

  const EquationList &equations = mmo_class.equations_ref().equations_ref();

  VariablesCollector collector(mmo_class);
  std::pair<ExpList, std::vector<std::pair<Expression, Expression>>> collection = collector.collectVariables();
  ExpList unknowns = collection.first;
  std::vector<std::pair<Expression, Expression>> unknownDerMap = collection.second;

   _equationIndex = 0;

  std::list<Vertex> eqVerts;
  std::list<Vertex> unknownVerts;

  DEBUG('c', "Building causalization graph...\n");
  DEBUG('c', "Equation indexes:\n");

  foreach_(Equation e, equations) {
    VertexProperty vp;
    Equality &eq = get<Equality>(e);
    PartialEvalExpression eval(_mmo_class.syms_ref(),false);
    eq.left_ref()= Apply(eval ,eq.left_ref());
    eq.right_ref()= Apply(eval ,eq.right_ref());
    vp.equation = e;
    vp.type = E;
    vp.index = _equationIndex++;
    vp.visited = false;
    Vertex v = add_vertex(vp, _graph);
    eqVerts.push_back(v);
    if (debugIsEnabled('c'))
      cout << vp.index << ":" << e << endl;
  }

  DEBUG('c', "Unknown indexes:\n");

  _unknownIndex = 0;
  foreach_(Expression e, unknowns) {
    VertexProperty vp;
    vp.unknown = Unknown(e);
    vp.type = U;
    vp.index = _unknownIndex++;
    vp.visited = false;
    Vertex v = add_vertex(vp, _graph);
    unknownVerts.push_back(v);
    if (debugIsEnabled('c'))
      cout << vp.index << ":" << e << endl;
   }

  DEBUG('c', "Graph edges as (equation_index, unknown_index):\n");

  list<Vertex>::iterator acausalEqsIter, unknownsIter;
  foreach_(Vertex eqVertex, eqVerts) {
    foreach_(Vertex unknownVertex , unknownVerts) {
      Modelica::ContainsExpression occurrs(_graph[unknownVertex].unknown());
      Equation e = _graph[eqVertex].equation;
      ERROR_UNLESS(is<Equality>(e), "Causalization of non-equality equation is not supported");
      Equality eq = boost::get<Equality>(e);
      const bool rl = Apply(occurrs,eq.left_ref());
      const bool ll = Apply(occurrs,eq.right_ref());
      if(rl || ll) {
        add_edge(eqVertex, unknownVertex, _graph);
        DEBUG('c', "(%d, %d) ", _graph[eqVertex].index, _graph[unknownVertex].index);
      }
    }
  }

  DEBUG('c', "\n");

  GraphPrinter<VertexProperty,EdgeProperty> gp(_graph);
  gp.printGraph("initial_graph.dot");

  MakeGraphSets();
  InitializeVarMap(unknownDerMap);
}

void Pantelides::ApplyPantelides(){
  std::set<EquationVertex> initialEquationSet = _equationSet;
  for(EquationVertex eq : initialEquationSet){
    EquationVertex fVertex = eq;
    bool match;
    do {
//      //search for the node corresponding to the equation f (it should exist obviously)
//      EquationVertex fVertex;
//      for(EquationVertex eqV : equationSet){
//        if(_graph[eqV].equation == f){
//          fVertex = eqV;
//          break;
//        }
//      }
      std::set<Vertex> coloured;
      match = MatchEquation(fVertex, coloured);
      if(!match){
        for(Vertex u : coloured){
          set<UnknownVertex>::iterator it = _unknownSet.find(u);
          if (it != _unknownSet.end()) {
              UnknownVertex uv = *it;
              Unknown currentUnknown = _graph[uv].unknown;
              //create new unknown
              VertexProperty vp;
              Unknown derUnknown = currentUnknown;
              derUnknown.expression = derivate(currentUnknown.expression, _mmo_class.syms_ref());
              vp.unknown = derUnknown; //is this correct?
              vp.type = U;
              vp.index = _unknownIndex++;
              //vp.visited = false; //does this matter?
              Vertex newUnknown = add_vertex(vp, _graph);

              _unknownSet.insert(newUnknown);
              _varMap[uv] = newUnknown;//.push_back(std::make_pair(currentUnknown, vp.unknown));
          }
        }

        for(Vertex e : coloured){
          set<EquationVertex>::iterator it = _equationSet.find(e);
          if (it != _equationSet.end()) {
            EquationVertex ev = *it;
            Equation currentEquation = _graph[ev].equation;
            //create new unknown
            VertexProperty vp;
            Equality currentEquality = boost::get<Equality>(currentEquation);
            vp.equation = derivate_equality(currentEquality, _mmo_class.syms_ref()); //is this correct?
            _mmo_class.equations_ref().addEquation(vp.equation);
            vp.type = E;
            vp.index = _equationIndex++;
            //vp.visited = false; //does this matter?
            Vertex newEquation = add_vertex(vp, _graph);

            _equationSet.insert(newEquation);
            _eqMap[ev] = newEquation; //.push_back(std::make_pair(currentEquation, vp.equation));

            boost::graph_traits<CausalizationGraph>::adjacency_iterator ai, ai_end;
            for(boost::tie(ai, ai_end) = boost::adjacent_vertices(ev, _graph); ai != ai_end; ++ai){
              UnknownVertex adj = *ai;
              EdgeProperty ep;
              ep.color = "red";
              add_edge(newEquation, adj, ep, _graph);
              std::map<UnknownVertex,UnknownVertex>::iterator varMapIt = _varMap.find(adj);
              if (varMapIt != _varMap.end()){
                UnknownVertex derAdj = varMapIt->second;
                add_edge(newEquation, derAdj, ep, _graph);
              }
            }
          }
        }

        for(Vertex u : coloured){
          set<UnknownVertex>::iterator it = _unknownSet.find(u);
          if (it != _unknownSet.end()) {
              UnknownVertex uv = *it;
              //check for exceptions, these should all exist though
              UnknownVertex uvDer = _varMap.at(uv);
              EquationVertex assignedToUv = _assign.at(uv);
              EquationVertex assignedToUvDer = _eqMap.at(assignedToUv);
              _assign[uvDer] = assignedToUvDer;
          }
        }

        fVertex = _eqMap.at(fVertex); //check for exception, it should exist though
      }
    } while(!match);
  }

  GraphPrinter<VertexProperty,EdgeProperty> gp(_graph);
  gp.printGraph("graph_after_pantelides.dot");

  std::map<UnknownVertex, EquationVertex>::iterator assignIt;
  std::cout << "Final assignation: \n";
  for ( assignIt = _assign.begin(); assignIt != _assign.end(); assignIt++ ){
    UnknownVertex unknown = assignIt -> first;
    EquationVertex equation = assignIt -> second;
    std::cout << _graph[unknown].unknown() << " -> " << _graph[equation].index+1 << "\n";
  }

}

bool Pantelides::MatchEquation(EquationVertex fVertex, std::set<Vertex> &coloured){
  coloured.insert(fVertex);
  boost::graph_traits<CausalizationGraph>::adjacency_iterator ai, ai_end;
  for(boost::tie(ai, ai_end) = boost::adjacent_vertices(fVertex, _graph); ai != ai_end; ++ai){
    UnknownVertex uv = *ai;
    auto varMapIt = _varMap.find(uv); //TODO: types
    auto assignIt = _assign.find(uv);
    //if vertex is not assigned, i.e. it has the highest degree
    if(varMapIt == _varMap.end() && assignIt == _assign.end()){
      _assign[uv] = fVertex;
      return true;
    }
  }

  for(boost::tie(ai, ai_end) = boost::adjacent_vertices(fVertex, _graph); ai != ai_end; ++ai){
    UnknownVertex uv = *ai;
    auto varMapIt = _varMap.find(uv); //TODO: types
    auto colouredIt = coloured.find(uv);
    //if vertex is not coloured
    if(varMapIt == _varMap.end() && colouredIt == coloured.end()){
      coloured.insert(uv);
      EquationVertex assignedToUv = _assign.at(uv); //this exists because we would've returned in the previous for if not
      if(MatchEquation(assignedToUv, coloured)){
        _assign[uv] = fVertex;
        return true;
      }
    }
  }
  return false;
}

void Pantelides::MakeGraphSets(){
  CausalizationGraph::vertex_iterator vi, vi_end;
  for(boost::tie(vi,vi_end) = vertices(_graph); vi != vi_end; ++vi) {
    Vertex v = *vi;
    if (_graph[v].type == E) {
      _equationSet.insert(v);
    } else {
      _unknownSet.insert(v);
    }
  }
}

void Pantelides::InitializeVarMap(std::vector<std::pair<Expression, Expression>> expMap){
  for(auto uv : _unknownSet) {
    Unknown u = _graph[uv].unknown;
    auto expMapIt = std::find_if( expMap.begin(), expMap.end(), [u](const std::pair<Expression, Expression>& exp){ return exp.first == u.expression;} );
    if(expMapIt != expMap.end()){
      Expression uDer = expMapIt->second;
      auto unknownIt = std::find_if( _unknownSet.begin(), _unknownSet.end(), [this, uDer](const UnknownVertex& uv){ return _graph[uv].unknown.expression == uDer;} );
      if(unknownIt != _unknownSet.end()){
          UnknownVertex der = *unknownIt;
          _varMap[uv] = der;
      }
    }
  }
}

}