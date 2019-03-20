/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

extern int global ;

namespace std {
   template <>
   struct hash<Aig_childs> {
       std::size_t operator()( const Aig_childs& p ) const {
        return (hash<int>()( p.Lchild )
          ^ (hash<int>()( p.Rchild ) << 1));
       }
   };
}

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed

void
CirMgr::strash()
{
   ++global;
   unordered_map < Aig_childs, CirGate* > gateMap;
   gateMap.rehash( cirMgr -> cirGate.size() );
   for ( size_t i = 0; i < cirMgr -> po_number ; ++i ){
      cirGate[ piaig_number + i + 1 ] -> strash( gateMap );
   }
   _dfsList.clear();
   FecGrps.clear();
}

void
CirMgr::fraig()
{
   SatSolver solver;
   solver.initialize();
   genProofModel(solver);
   Var v = piList[0] -> _var;
   solver.addAigCNF( cirGate[0] -> _var , v , true , v , false );
   if ( _dfsList.size() == 0 ){
      _dfsList.push_back( 0 );
      for ( size_t i = 0; i < po_number ; ++i ){
         cirGate[ piaig_number + i + 1 ] -> dfs( _dfsList );
      }
   }
   size_t count = 0;
   while( FecGrps.size() > cirGate.size() / 200){
   if ( 1 ){
      
      for( int i = _dfsList.size() -1 ; i >= 0 ; --i ){ // grp no.1 要最深
         int  id =  _dfsList[ i ] ;
         auto grp_iter = cirGate[ id ] -> getfecgrp();
         if ( grp_iter == FecGrps.end() ) continue;
        auto iter = find( (*grp_iter).begin() , (*grp_iter).end() , id );
         if ( iter == (*grp_iter).end() ) iter = find( (*grp_iter).begin() , (*grp_iter).end() , -id );
         swap( *iter , *(*grp_iter).begin());
      }
      for( size_t i = 0 ; i < _dfsList.size() ; ++i ){//fraig~~

      int  id =  _dfsList[ i ] ;
      auto grp_iter = cirGate[ id ] -> getfecgrp();
      if ( grp_iter == FecGrps.end() ) continue; //不在group裡
      auto iter = find( (*grp_iter).begin() , (*grp_iter).end() , id );
      if ( iter == (*grp_iter).end() ) 
           iter = find( (*grp_iter).begin() , (*grp_iter).end() , -id );
      if ( iter == (*grp_iter).end()   ) continue; // 不在grp裡
      if ( iter == (*grp_iter).begin() ) continue; //最深不用檢查
      if ( (*grp_iter).size() == 0  ) continue; 
      int id_2 =  *(*grp_iter).begin() ;
          id   =  *iter;
      if ( cirGate[ abs(id) ] == 0 )continue;
      if ( cirGate[ abs(id_2) ] == 0 )continue;
      if ( ( cirGate[ abs(id) ] -> sim != cirGate[ abs(id_2) ] -> sim ) &&
          ( ~cirGate[ abs(id) ] -> sim != cirGate[ abs(id_2) ] -> sim ) ){
             continue;
         }
      bool id_inverse = false; 
      bool id_2_inverse = false;
      if ( id < 0 ) id_inverse = true;
      if ( id_2 < 0 ) id_2_inverse = true;
      Var newV = solver.newVar();
      solver.addXorCNF(newV, cirGate[ abs(id) ]->getVar(), id_inverse, 
                               cirGate[ abs(id_2) ]->getVar(), id_2_inverse );
      solver.assumeRelease();  // Clear assumptions
      solver.assumeProperty(newV, true);  // k = 1 <=<=所以如果k永遠是0，代表相等
      bool result = solver.assumpSolve();//result 是 sat or unsat // 1 -> 
      if ( result ){ // 收集反例
         ++count;
         for( size_t m = 1 ; m < cirGate.size() ; ++m ){
            if ( cirGate [ m ] == 0 ) continue;
            cirGate[m] -> sim = cirGate[m] -> sim << 1;
            int pat = solver.getValue( cirGate[m] -> _var );
            if ( pat != 0 && pat != 1 ) pat = 0;
            cirGate[m] -> sim = cirGate[m] -> sim | pat;

         }
         if ( count % 64 == 0 ){
            for ( size_t n = 0 ; n < FecGrps.size() ; ++n ){
               for ( size_t t = 1 ; t < FecGrps[n].size() ; ++t ){
                  int member = FecGrps[n][t];
                  int leader = FecGrps[n][0];
                  if ( cirGate[ abs(member) ] == 0 )continue;
                  if ( cirGate[ abs(leader) ] == 0 )continue;
                  if ( ( cirGate[ abs(member) ] -> sim != cirGate[ abs(leader) ] -> sim ) &&
                      ( ~cirGate[ abs(member) ] -> sim != cirGate[ abs(leader) ] -> sim ) ){
                     FecGrps[n].erase( FecGrps[n].begin() + t );
                  }
               }
            }

            for ( size_t n = 0 ; n < used_pattern.size() ; ++n ){
               if ( piList[n] != 0 ) used_pattern[ n ].push_back( piList[n] -> sim );
            }

         }
      }     
      if ( !result ){
         cout<<"Fraig: ";
         if ( id_inverse ^ id_2_inverse ) 
              cirGate[ abs( id_2 ) ] -> merge( cirGate[0] -> ptrplus( cirGate[ abs( id ) ] ) );
         else cirGate[ abs( id_2 ) ] -> merge( cirGate[ abs( id ) ] );
      }
      }
   }


   for ( size_t n = 0 ; n < used_pattern.size() ; ++n ){
      if ( piList[n] != 0 ) used_pattern[ n ].push_back( piList[n] -> sim );
   }

   strash();

   randomSim();

   solver.initialize();
   genProofModel(solver);
   v = piList[0] -> _var;
   solver.addAigCNF( cirGate[0] -> _var , v , true , v , false );
   if ( _dfsList.size() == 0 ){
      _dfsList.push_back( 0 );
      for ( size_t i = 0; i < po_number ; ++i ){
         cirGate[ piaig_number + i + 1 ] -> dfs( _dfsList );
      }
   }
   count = 0;   
   }
   if ( 1 ){
      
      for( size_t i = 0 ; i < _dfsList.size() ; ++i ){ // grp no.1 要最深
         int  id =  _dfsList[ i ] ;
         auto grp_iter = cirGate[ id ] -> getfecgrp();
         if ( grp_iter == FecGrps.end() ) continue;
         auto iter = find( (*grp_iter).begin() , (*grp_iter).end() , id );
         if ( iter == (*grp_iter).end() ) iter = find( (*grp_iter).begin() , (*grp_iter).end() , -id );
         int a = *iter;
         (*grp_iter).erase( iter );
         (*grp_iter).push_back( a );
      }


      for( size_t i = 0 ; i < FecGrps.size() ; ++i ){//fraig~~
      for( int j = FecGrps[i].size()-(int)1 ; j > 0 ; --j ){//merged
         int id = FecGrps[i][j];
         bool id_inverse = false; 
         if ( id < 0 ) id_inverse = true;
         id = abs( id );
      for( int k = j -1  ; k >= 0 ; --k ){//merger
         assert( j != k );
         int id_2 = FecGrps[i][k];
         bool id_2_inverse = false;
         if ( id_2 < 0 ) id_2_inverse = true;
         id_2 = abs( id_2 );
         if ( FecGrps[i][j] == INT_MAX )break;
         if ( FecGrps[i][k] == INT_MAX )continue;
         assert( id != id_2 );
         assert( cirGate[ abs(id) ] != 0 );
         assert( cirGate[ abs(id_2) ] != 0 );
         if ( ( cirGate[ abs(id) ] -> sim != cirGate[ abs(id_2) ] -> sim ) &&
         ( ~cirGate[ abs(id) ] -> sim != cirGate[ abs(id_2) ] -> sim ) ){
            continue;
         }
         Var newV = solver.newVar();
         solver.addXorCNF(newV, cirGate[ abs(id) ]->getVar(), id_inverse, 
                                cirGate[ abs(id_2) ]->getVar(), id_2_inverse );
         solver.assumeRelease();  // Clear assumptions
         solver.assumeProperty(newV, true);  // k = 1 <=<=所以如果k永遠是0，代表相等
         bool result = solver.assumpSolve();//result 是 sat or unsat // 1 -> 

         if ( result ){
         }


        if ( result ){ // 收集反例
           ++count;
           for( size_t m = 1 ; m < cirGate.size() ; ++m ){
              if ( cirGate [ m ] == 0 ) continue;
              cirGate[m] -> sim = cirGate[m] -> sim << 1;
              int pat = solver.getValue( cirGate[m] -> _var );
              if ( pat != 0 && pat != 1 ) pat = 0;
              cirGate[m] -> sim = cirGate[m] -> sim | pat;

           }
        }     
        if ( !result ){
           cout<<"Fraig: ";
           if ( id_inverse ^ id_2_inverse ) 
                cirGate[ abs( id_2 ) ] -> merge( cirGate[0] -> ptrplus( cirGate[ abs( id ) ] ) );
           else cirGate[ abs( id_2 ) ] -> merge( cirGate[ abs( id ) ] );
           break;
        }
      }
      }
      }
   }
   strash();

}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/


void
CirMgr::genProofModel(SatSolver& s)
{
   // Allocate and record variables; No Var ID for POs
   for (size_t i = 0, n = cirGate.size(); i < n; ++i ) {//取得solver id( var )
      if ( cirGate[i] == 0 ) continue;
      Var v = s.newVar();
      cirGate[i]->setVar(v);
   }
   ++global;
   for ( size_t i = 0 ; i < po_number ; ++i ){
      cirGate[ piaig_number + i + 1 ] -> cnf( s );
   }

}
