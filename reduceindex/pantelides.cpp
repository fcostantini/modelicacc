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
#include <causalize/unknowns_collector.h>
#include <boost/lambda/lambda.hpp>
#include <ast/equation.h>
#include <boost/variant/get.hpp>
#include <mmo/mmo_class.h>
#include <util/ast_visitors/contains_expression.h>
#include <util/ast_visitors/partial_eval_expression.h>
#include <util/solve/solve.h>
#include <fstream>

using namespace Modelica::AST;
using namespace Causalize;

namespace Pant {
Pantelides::Pantelides(MMO_Class &mmo_class): Causalize::CausalizationStrategy(mmo_class) {
  //TODO: (ask how to do this)
  //add state variables to graph as unknowns
  //initialize varMap
  MakeGraphSets();
}

void Pantelides::ApplyPantelides(){
  std::set<EquationVertex> initialEquationSet = equationSet;
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
          set<UnknownVertex>::iterator it = unknownSet.find(u);
          if (it != unknownSet.end()) {
              UnknownVertex uv = *it;
              Unknown currentUnknown = _graph[uv].unknown;
              //create new unknown
              VertexProperty vp;
              Unknown derUnknown = currentUnknown;
              derUnknown.expression = derivate(currentUnknown.expression, _mmo_class.syms_ref());
              vp.unknown = derUnknown; //is this correct?
              vp.type = U;
              //vp.index = index++; //do these two matter?
              //vp.visited = false:
              Vertex newUnknown = add_vertex(vp, _graph);

              unknownSet.insert(newUnknown);
              varMap[uv] = newUnknown;//.push_back(std::make_pair(currentUnknown, vp.unknown));
          }
          else DEBUG('c', "Something's wrong, we should have coloured unknowns (first for)\n");
        }

        for(Vertex e : coloured){
          set<EquationVertex>::iterator it = equationSet.find(e);
          if (it != equationSet.end()) {
            EquationVertex ev = *it;
            Equation currentEquation = _graph[ev].equation;
            //create new unknown
            VertexProperty vp;
            Equality currentEquality = boost::get<Equality>(currentEquation);
            vp.equation = derivate_equality(currentEquality, _mmo_class.syms_ref()); //is this correct?
            vp.type = E;
            //vp.index = index++; //do these two matter?
            //vp.visited = false:
            Vertex newEquation = add_vertex(vp, _graph);

            equationSet.insert(newEquation);
            eqMap[ev] = newEquation; //.push_back(std::make_pair(currentEquation, vp.equation));

            boost::graph_traits<CausalizationGraph>::adjacency_iterator ai, ai_end;
            for(boost::tie(ai, ai_end) = boost::adjacent_vertices(ev, _graph); ai != ai_end; ++ai){
              UnknownVertex adj = *ai;
              add_edge(newEquation, adj, _graph);
              std::map<UnknownVertex,UnknownVertex>::iterator varMapIt = varMap.find(adj);
              if (varMapIt != varMap.end()){
                UnknownVertex derAdj = varMapIt->second;
                add_edge(newEquation, derAdj, _graph);
              }
            }
          }
          else DEBUG('c', "Something's wrong, we should have coloured equations (second for)\n");
        }

        for(Vertex u : coloured){
          set<UnknownVertex>::iterator it = unknownSet.find(u);
          if (it != unknownSet.end()) {
              UnknownVertex uv = *it;
              //check for exceptions, these should all exist though
              UnknownVertex uvDer = varMap.at(uv);
              EquationVertex assignedToUv = assign.at(uv);
              EquationVertex assignedToUvDer = eqMap.at(assignedToUv);
              assign[uvDer] = assignedToUvDer;
          }
          else DEBUG('c', "Something's wrong, we should have coloured unknowns (third for)\n");
        }

        fVertex = eqMap.at(fVertex); //check for exception, it should exist though
      }
    } while(!match);
  }
}

bool Pantelides::MatchEquation(EquationVertex fVertex, std::set<Vertex> &coloured){
  coloured.insert(fVertex);
  boost::graph_traits<CausalizationGraph>::adjacency_iterator ai, ai_end;
  for(boost::tie(ai, ai_end) = boost::adjacent_vertices(fVertex, _graph); ai != ai_end; ++ai){
    UnknownVertex uv = *ai;
    auto varMapIt = varMap.find(uv); //TODO: types
    auto assignIt = assign.find(uv);
    //if vertex is not assigned, i.e. it has the highest degree
    if(varMapIt == varMap.end() && assignIt == assign.end()){
      assign[uv] = fVertex;
      return true;
    }
  }

  for(boost::tie(ai, ai_end) = boost::adjacent_vertices(fVertex, _graph); ai != ai_end; ++ai){
    UnknownVertex uv = *ai;
    auto varMapIt = varMap.find(uv); //TODO: types
    auto colouredIt = coloured.find(uv);
    //if vertex is not coloured
    if(varMapIt == varMap.end() && colouredIt == coloured.end()){
      coloured.insert(uv);
      EquationVertex assignedToUv = assign.at(uv); //this exists because we would've returned in the previous for if not
      if(MatchEquation(assignedToUv, coloured)){
        assign[uv] = fVertex;
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
      equationSet.insert(v);
    } else {
      unknownSet.insert(v);
    }
  }
}

}