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
   for( size_t i = 0 ; i < FecGrps.size() ; ++i ){
      FecGrps[ i ].erase( FecGrps[ i ].begin() );
   }
}

void
CirMgr::fraig()
{
   random_device rand;
   default_random_engine engine( rand() );
   uniform_int_distribution < unsigned long long > rngen( 0,  ULONG_MAX );
   SatSolver solver;  
   for( size_t xx = 0 ; xx < repeat ; ++xx ){
      Var v;
      vector< size_t > _collect( piList.size() );//to collecting useful patterns
      size_t count = 0;
      auto FecGrps_copy = FecGrps;
      solver.initialize();
      for ( size_t i = 0 ; i < cirGate.size() ; ++i ){
         if ( cirGate[ i ] == 0 )continue;
         cirGate[ i ] -> setVar( solver.newVar() );
      }
      v = piList[0] -> _var;
      solver.addAigCNF( cirGate[0] -> _var , v , true , v , false );
      for( size_t i = 0 ; i < _dfsList.size() ; ++i ){//fraig~~
         int id =  _dfsList[ i ] ;
         if ( true ){
            CirAigGate* ptr = ( CirAigGate* )cirGate[ id ];
            if ( ptr -> Lchild == ptr -> Rchild ){
               cout<<"Fraig: ";
               if ( cirGate[ 0 ] -> ptrinvert( ptr -> Lchild ) )
               ( cirGate[ 0 ] -> oriptr( ptr -> Lchild ) )-> merge ( cirGate[ 0 ] -> ptrplus( cirGate[ id ] ) );
               else 
               ptr -> Lchild -> merge ( cirGate[ id ] );
               continue;
            }
            else if ( ptr -> Lchild == cirGate[ 0 ] || ptr -> Rchild == cirGate[ 0 ] ){
               cout<<"Fraig: ";
               cirGate[ 0 ] -> merge( cirGate[ id ] );
               continue;
            }
            else if ( ( ptr -> Lchild == ptrplus( cirGate[ 0 ] ) )  || 
                      ( ptr -> Rchild == ptrplus( cirGate[ 0 ] ) ) ) {
               if ( ptr -> Lchild == ptrplus( cirGate[ 0 ] ) ) 
                  swap( ptr -> Lchild , ptr -> Rchild );
               cout<<"Fraig: ";
               if ( cirGate[ 0 ] -> ptrinvert( ptr -> Lchild ) )
               ( cirGate[ 0 ] -> oriptr( ptr -> Lchild ) )-> merge ( cirGate[ 0 ] -> ptrplus( cirGate[ id ] ) );
               else 
               ptr -> Lchild -> merge ( cirGate[ id ] );
               continue;
            }
         }
         cirGate[ id ] -> cnf( solver );
         int tem = cirGate[ id ] -> fecgrpnum;
         if ( tem == -1 ) continue;
         int id_2 = FecGrps_copy[ tem ][ 0 ];
         bool id_2_inverse = false;
         if ( id == id_2  ) continue;//最深不用檢查
         if ( cirGate[ id ] -> sim != cirGate[ id_2 ] -> sim ) id_2_inverse = true;
         Var newV = solver.newVar();
         solver.addXorCNF(newV, cirGate[ id ]->getVar(), false, 
                                  cirGate[ id_2 ]->getVar(), id_2_inverse );
         solver.assumeRelease();  // Clear assumptions
         solver.assumeProperty(newV, true);  // k = 1 <=如果k永遠是0，代表相等
         bool result = solver.assumpSolve();//result 是 sat or unsat 
         if ( result ){ 
            ++count;
            //-----------------------------------------------收集反例
            for( size_t m = 0 ; m < piList.size() ; ++m ){  
               piList[ m ] -> sim = piList[ m ] -> sim << 1;
               int pat = solver.getValue( piList[ m ] -> _var );
               size_t tem = ( rngen( engine ) );
               _collect[ m ] = ( _collect[ m ] << 1 ) | pat;
               piList[ m ] -> sim = ( tem << 1 ) | pat; 
            }  

            for( size_t m = 1 ; m < _dfsList.size() ; ++m ){//sim
               int tem = _dfsList[ m ];
               if ( cirGate[ tem ] == 0 ) continue;
               CirAigGate* ptr = ( CirAigGate* )cirGate[ tem ];
               cirGate[ tem ] -> sim = 
                  ptr -> getsim( ptr -> Lchild ) & ptr -> getsim( ptr -> Rchild );
            }

            //-----------------------------------------------收集反例              
            for ( size_t n = 0 ; n < FecGrps_copy.size() ; ++n ){//剔除不一樣的
               for ( size_t t = 1 ; t < FecGrps_copy[ n ].size(); ++t ){
                  int member = FecGrps_copy[ n ][ t ];
                  int leader = FecGrps_copy[ n ][ 0 ];
                  if ( cirGate[ member ] == 0 )continue;
                  if ( member == leader )continue;
                  if ( ( cirGate[ member ] -> sim != cirGate[ leader ] -> sim ) &&
                      ( ~cirGate[ member ] -> sim != cirGate[ leader ] -> sim ) ){
                     cirGate[ member ] -> fecgrpnum = -1;
                     FecGrps_copy[ n ][ t ] = FecGrps_copy[ n ][ 0 ];
                  }
               }
            }
            if ( count == 64 ){ //收集反例   
               count = 0;
               for ( size_t n = 0 ; n < used_pattern.size() ; ++n ){
                  if ( piList[n] != 0 ) used_pattern[ n ].push_back( _collect[ n ] );
               }
            }
         }     
         if ( !result ){
            cout<<"Fraig: ";
            if ( id_2_inverse ) 
                 cirGate[ id_2 ] -> merge( cirGate[0] -> ptrplus( cirGate[ id ] ) );
            else cirGate[ id_2 ] -> merge( cirGate[ id ] );
         }
      }
      strash();
      if( xx != repeat - 1 ) randomSim();
   }

   if ( !all ) {
      if ( repeat >= 1 ){
         cout<<"repeat : ";
         if ( repeat == 1 ) cout<<"once."<<endl;
         else if ( repeat == 1 ) cout<<"twice."<<endl;
         else cout<<repeat<<" times."<<endl;
      }
      if ( all == true ){
         cout<<"All gates are proved."<<endl;
         proved = true;
      }
      fecgrpexist = false;
      return;
   }
   if ( repeat != 0 )randomSim();
   solver.initialize();

   for ( size_t i = 0 ; i < cirGate.size() ; ++i ){
      if ( cirGate[ i ] == 0 )continue;
      cirGate[ i ] -> setVar( solver.newVar() );
   }

   Var v;
   v = piList[0] -> _var;
   solver.addAigCNF( cirGate[0] -> _var , v , true , v , false );
   for( size_t i = 0 ; i < _dfsList.size() ; ++i ){//fraig~~
      int  id =  _dfsList[ i ] ;
      if ( true ){
         CirAigGate* ptr = ( CirAigGate* )cirGate[ id ];
         if ( ptr -> Lchild == ptr -> Rchild ){
            cout<<"Fraig: ";
            if ( cirGate[ 0 ] -> ptrinvert( ptr -> Lchild ) )
            ( cirGate[ 0 ] -> oriptr( ptr -> Lchild ) )-> merge ( cirGate[ 0 ] -> ptrplus( cirGate[ id ] ) );
            else 
            ptr -> Lchild -> merge ( cirGate[ id ] );
            continue;
         }
         else if ( ptr -> Lchild == cirGate[ 0 ] || ptr -> Rchild == cirGate[ 0 ] ){
            cout<<"Fraig: ";
            cirGate[ 0 ] -> merge( cirGate[ id ] );
            continue;
         }
         else if ( ( ptr -> Lchild == ptrplus( cirGate[ 0 ] ) )  || 
                   ( ptr -> Rchild == ptrplus( cirGate[ 0 ] ) ) ) {
            if ( ptr -> Lchild == ptrplus( cirGate[ 0 ] ) ) 
               swap( ptr -> Lchild , ptr -> Rchild );
            cout<<"Fraig: ";
            if ( cirGate[ 0 ] -> ptrinvert( ptr -> Lchild ) )
            ( cirGate[ 0 ] -> oriptr( ptr -> Lchild ) )-> merge ( cirGate[ 0 ] -> ptrplus( cirGate[ id ] ) );
            else 
            ptr -> Lchild -> merge ( cirGate[ id ] );
            continue;
         }
      }
      cirGate[ id ] -> cnf( solver );
      int tem = cirGate[ id ] -> fecgrpnum;
      if ( tem == -1 ) continue;
      int id_2 = FecGrps[ tem ][ 0 ];
      if ( id_2 == id ) continue;
      bool id_2_inverse = false;
      if ( cirGate[ id ] -> sim != cirGate[ id_2 ] -> sim ) id_2_inverse = true;
      Var newV = solver.newVar();
      solver.addXorCNF(newV, cirGate[ id ]->getVar(), false, 
                               cirGate[ id_2 ]->getVar(), id_2_inverse );
      solver.assumeRelease();  // Clear assumptions
      solver.assumeProperty(newV, true);  // k = 1 <=如果k永遠是0，代表相等
      bool result = solver.assumpSolve();//result 是 sat or unsat 
      if ( result ){ 
      //-----------------------------------------------收集反例
         for( size_t m = 0 ; m < piList.size() ; ++m ){//give pi patterns  
               piList[ m ] -> sim = piList[ m ] -> sim << 1;
               int pat = solver.getValue( piList[ m ] -> _var );
               size_t _tem = ( rngen( engine ) );
               piList[ m ] -> sim = ( _tem << 1 ) | pat; 
         }  
         for( size_t m = 1 ; m < _dfsList.size() ; ++m ){//simulate
               size_t _tem = _dfsList[ m ];
               if ( cirGate[ _tem ] == 0 ) continue;
               CirAigGate* ptr = ( CirAigGate* )cirGate[ _tem ];
               cirGate[ _tem ] -> sim = 
                  ptr -> getsim( ptr -> Lchild ) & ptr -> getsim( ptr -> Rchild );
         }
         //-----------------------------------------------收集反例        
         fecgrps_hash();
         for( size_t m = i ; m < _dfsList.size() ; ++m ){
            int _id = _dfsList[ m ];
            cirGate[ _id ] -> fecgrpnum = -1;
         }
         for( size_t m = 0 ; m < FecGrps.size() ; ++m ){
            for ( size_t n = 0 ; n < FecGrps[ m ].size() ; ++n ){
               cirGate[ FecGrps[ m ][ n ] ] -> fecgrpnum = m;
            }
         }
         --i;
         continue;
      }     
      if ( !result ){
         cout<<"Fraig: ";
         if ( id_2_inverse ) 
              cirGate[ id_2 ] -> merge( cirGate[0] -> ptrplus( cirGate[ id ] ) );
         else cirGate[ id_2 ] -> merge( cirGate[ id ] );
         continue;
      }
   }
   FecGrps.clear();
   strash();
   if ( repeat >= 1 ){
      cout<<"repeat : ";
      if ( repeat == 1 ) cout<<"once."<<endl;
      else if ( repeat == 1 ) cout<<"twice."<<endl;
      else cout<<repeat<<" times."<<endl;
   }
   if ( all == true ){
      cout<<"All gates are proved."<<endl;
      proved = true;
   }
   fecgrpexist = false;
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/



