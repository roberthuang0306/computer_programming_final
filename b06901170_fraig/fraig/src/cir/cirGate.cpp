/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <vector>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;
int global = 0;

namespace std {
   template <>
   struct hash<Aig_childs> {
       std::size_t operator()( const Aig_childs& p ) const {
        return (hash<int>()( p.Lchild )
          ^ (hash<int>()( p.Rchild ) << 1));
       }
   };
}

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/

void 
CirGate::merge( CirGate * a ){//用this取代a
   cout<<id<<" merging ";
   if ( ptrinvert( a ) ) cout<<'!';
   cout<< oriptr(a) -> getid() <<"..."<<endl;
   oriptr( ((CirAigGate*)oriptr( a )) -> Lchild ) -> delparent( oriptr( a ) );
   oriptr( ((CirAigGate*)oriptr( a )) -> Rchild ) -> delparent( oriptr( a ) );
   if ( ptrinvert( a ) ) {
      invertptr( oriptr ( a ) -> parent );
      pushparent ( oriptr ( a ) -> parent );
   }
   else pushparent( oriptr ( a ) -> parent );
   for ( size_t i = 0 ; i < oriptr( a ) -> parent.size() ; ++i ){//parent update their child
      oriptr( oriptr ( a ) -> parent[i] ) -> setchild( a , this );
   }
   cirMgr -> cirGate[ oriptr( a ) -> id ] = 0;
   delete (oriptr ( a )) ;
}

void
CirGate::reportGate() const
{
   cout<<"================================================================================"<<endl;
   cout<<"= ";
   if ( name.size()!=0 ) {
      cout<<getTypeStr()<<'('<<id<<')'<<'\"'<<name<<'\"'<<", line "<<lineno<<endl;
   }
   else {
      cout<<getTypeStr()<<'('<<id<<')'<<", line "<<lineno<<endl;
   }
   cout<<"= FECs:";
   auto iter = getfecgrp();
   if ( iter != cirMgr -> FecGrps.end() ){
      bool inverse = ( ~sim == (cirMgr -> cirGate) [ *(*iter).begin() ] -> sim );
      for ( auto it = (*iter).begin() ; it != (*iter).end() ; ++it ){
         if ( !inverse ) {
            if ( *it != id ) {
               cout<<' ';
               if ( *it < 0 )cout<<'!'<<-*it;
               else cout<<*it;
            }
         }
         else {
            if ( *it != -id ){
               cout<<' ';
               if ( *it < 0 )cout<<-*it;
               else cout<<'!'<<*it;
            }
         }
      }
   }
   cout<<endl;
   cout<<"= Value: ";
   bool* ptr;
   ptr = ( bool* )&sim;
   for ( int i = 0 ; i < 8 ; ++i ){  
      if ( i != 0 )cout<<'_';
      cout<<bitset<8>( *(ptr+7-i) ); }
   cout<<endl;
   cout<<"================================================================================"<<endl;
}



void
CirAigGate::reportFanin(int level , bool inverse ,int space) 
{
   assert (level >= 0);
   if ( space == 0 ) ++global;
   else {
      cout<<setw(space)<<' ';
   }
   if ( inverse )cout<<'!';
   cout<<getTypeStr()<<' '<<id;
   if ( travel == global && level > 0 ) cout<<" (*)";
   cout<<endl;
   if ( travel != global && level > 0){
      travel = global;
      oriptr ( Lchild ) -> reportFanin( level-1 , ptrinvert(Lchild) , space+2 );
      oriptr ( Rchild ) -> reportFanin( level-1 , ptrinvert(Rchild) , space+2 );
   }
}

void
CirPoGate::reportFanin(int level , bool inverse ,int space) 
{
   assert (level >= 0);
   if ( space == 0 ) ++global;
   else {
      cout<<setw(space)<<' ';
   }
   if ( inverse )cout<<'!';
   cout<<getTypeStr()<<' '<<id;
   if ( travel == global && level > 0 ) cout<<" (*)";
   cout<<endl;
   if ( travel != global && level >0 ){
      travel = global;
      oriptr ( child ) -> reportFanin( level-1 , ptrinvert(child) , space+2 );
   }
}

void
CirPoGate::reportFanout(int level , bool inverse ,int space) 
{
   assert (level >= 0);
   if ( space != 0 )  cout<<setw(space)<<' ';
   if ( inverse )cout<<'!';
   cout<<getTypeStr()<<' '<<id<<endl;
}

void
CirGate::reportFanin(int level , bool inverse ,int space) 
{
   assert (level >= 0);
   if ( space != 0 )  cout<<setw(space)<<' ';
   if ( inverse )cout<<'!';
   cout<<getTypeStr()<<' '<<id<<endl;
}

void
CirGate::reportFanout(int level , bool inverse ,int space) 
{
   assert (level >= 0);
   if ( space == 0 ) ++global;
   else {
      cout<<setw(space)<<' ';
   }
   if ( inverse )cout<<'!';
   cout<<getTypeStr()<<' '<<id;
   if ( travel == global && level > 0 && parent.size() > 0) cout<<" (*)";
   cout<<endl;
   sort();
   if ( travel != global && level >0 ){
      travel = global;
      for ( size_t i = 0; i < parent.size() ; ++i ){
         oriptr ( parent[i] ) -> reportFanout( level-1 , ptrinvert(parent[i]) , space+2 );
      }
   }
}
void 
CirGate::sort(){
   for ( size_t i=0 ; i< parent.size() ; ++i){
      for ( size_t j=i ; j< parent.size() ; ++j){
         if ( oriptr(parent[i]) -> id > oriptr(parent[j]) -> id ){
            CirGate* a = parent[i];
            parent[i] = parent[j];
            parent[j] = a;
         }
      }
   }
}
void 
CirAigGate::countaig( int &a ) {
  if (travel == global )return;
  ++a;
  oriptr(Lchild) -> countaig ( a );
  oriptr(Rchild) -> countaig ( a );
  travel = global;
}
void 
CirAigGate::printAig( ostream &a ){
   if ( travel == global )return;
   travel = global;
   oriptr(Lchild) -> printAig( a );
   oriptr(Rchild) -> printAig( a );
   a<<2*id<<' ';
   if ( ptrinvert(Lchild) ) a<<2*(oriptr(Lchild) -> getid())+1<<' ';
   else a<<2*(Lchild -> getid())<<' ';
   if ( ptrinvert(Rchild) ) a<<2*(oriptr(Rchild) -> getid())+1<<endl;
   else a<<2*(Rchild -> getid())<<endl;
}
void
CirGate::printGate( int &clock) {
//    cout<<'\n'<<"printing gate num."<<setw(6)<<getid();
    if (travel == global)return;
    travel = global;
    if (id == 0){
       cout<<endl;
       cout<<'['<<clock++<<"] ";
       cout<<"CONST0";
    }
}
void
CirAigGate::printGate( int &clock) {
//    cout<<'\n'<<"printing gate num."<<setw(6)<<getid();
    if (travel == global)return;
    travel = global;
    oriptr(Lchild) -> printGate ( clock);
    oriptr(Rchild) -> printGate ( clock);
    cout<<endl;
    cout<<'['<<clock++<<"] ";
    cout<<getTypeStr()<<' '<<getid()<<' ';
    if ( oriptr(Lchild) -> getTypeStr() == "UNDEF" ) cout<<'*';
    if (ptrinvert(Lchild)) cout<<'!';
    cout<<oriptr(Lchild) -> getid()<<' ';
    if ( oriptr(Rchild) -> getTypeStr() == "UNDEF" ) cout<<'*';
    if (ptrinvert(Rchild)) cout<<'!';
    cout<<oriptr(Rchild) -> getid();
}
void
CirPiGate::printGate( int &clock) {
//    cout<<'\n'<<"printing gate num."<<setw(6)<<getid();
    if ( travel == global )return;
    cout<<endl;
    cout<<'['<<clock++<<"] ";
    cout<<getTypeStr()<<"  "<<getid();
    if ( name.size() != 0 ) cout<<" ("<<name<<')';
    travel = global;
}
void
CirPoGate::printGate( int &clock) {
//    cout<<'\n'<<"printing gate num."<<setw(6)<<getid();
    if ( travel == global ) return;
    oriptr(child) -> printGate ( clock);
    cout<<endl;
    cout<<'['<<clock++<<"] ";
    cout<<getTypeStr()<<"  "<<getid()<<' ';
    if ( oriptr(child) -> getTypeStr() == "UNDEF" ) cout<<'*';
    if (ptrinvert(child)) cout<<'!';
    cout<<oriptr(child) -> getid();
    if ( name.size() != 0 ) cout<<" ("<<name<<')';
    travel = global;
}

void
CirAigGate::setchild( CirGate* a , CirGate* b ){//b取代a 
   if ( ptrinvert( a ) ^ ptrinvert( b ) ){
      if ( oriptr( Lchild ) == oriptr( a ) ) {
         if ( ptrinvert( Lchild ) ) Lchild = oriptr( b );
         else Lchild = invertptr( oriptr( b ) );
      }
      else{
         if ( ptrinvert( Rchild ) ) Rchild = oriptr( b );
         else Rchild = invertptr( oriptr( b ) );
      }
      return;
   }
   else{
      if ( oriptr( Lchild ) == oriptr( a ) ) {
         if ( ptrinvert( Lchild ) ) Lchild = invertptr( oriptr( b ) );
         else Lchild = oriptr( b );
      }
      else {
         if ( ptrinvert( Rchild ) ) Rchild = invertptr( oriptr( b ) );
         else Rchild = oriptr( b );
      }
      return;
   }
    cerr<<"setchild fault"<<endl;
}

void
CirPoGate::setchild( CirGate* a , CirGate* b ){//b取代a
   if ( ptrinvert( a ) ^ ptrinvert( b ) ){
      if ( ptrinvert( child ) ) child = oriptr( b );
      else child = invertptr( oriptr( b ) );
   }
   else{
      if ( ptrinvert( child ) ) child = invertptr( oriptr( b ) );
      else child = oriptr( b );
   }
}

//----------------------------------sweep--------------------------------------------

void
CirGate::sweeping( vector<bool> &a  ) {
   if ( travel == global ) return;
   if ( getTypeStr() == "PI" )return;
   if ( getTypeStr() == "PO" )return;
   if ( getTypeStr() == "CONST" )return;
   if ( !existparent() ) {
      a[id] = true;
      travel = global;
   }
}

void
CirAigGate::sweeping( vector<bool> &a  ) {
   if ( travel == global ) return;
   if ( existparent() ) return;
   travel = global;
   a[id] = true;
   oriptr( Lchild ) -> delparent ( this );
   oriptr( Lchild ) -> sweeping( a );
   oriptr( Rchild ) -> delparent ( this );
   oriptr( Rchild ) -> sweeping( a );
}

//----------------------------------sweep--------------------------------------------

//---------------------------------optimize------------------------------------------

bool
CirPoGate::simplify( CirGate* &_const0  ){
   if ( travel == global ) return false;
   oriptr( child ) -> simplify( _const0 ) ;
   travel = global;
   return false;
}

bool
CirAigGate::simplify( CirGate* &_const0  ){
   if ( travel == global ) return false;
   else travel = global;
   oriptr( Lchild ) -> simplify( _const0 ) ;
   oriptr( Rchild ) -> simplify( _const0 ) ;
   if ( oriptr( Lchild ) == oriptr( Rchild ) ) {
      if ( Lchild != Rchild ){ // ori一樣 有兩種情況
         cout<<"Simplifying: ";
         _const0 -> merge( this );
      }
      else{
         cout<<"Simplifying: ";
         if ( ptrinvert( Lchild ) ) oriptr( Lchild ) -> merge( invertptr( this ) );
         else Lchild -> merge ( this );
      }
      return true;
   }

   if ( oriptr( Rchild ) == _const0 ) swap( Rchild , Lchild );

   if ( oriptr( Lchild ) == _const0 ){
      if ( ptrinvert( Lchild ) ){//Lchild == _const0+1
         cout<<"Simplifying: ";
         if ( ptrinvert( Rchild ) ) oriptr( Rchild ) -> merge( invertptr( this ) );
         else oriptr( Rchild ) -> merge ( this );
      }
      else{//Lchild == _const0
         cout<<"Simplifying: ";
         _const0 -> merge( this );
      }
      return true;
   }
   return false;
}

//---------------------------------optimize------------------------------------------

//----------------------------------strash-------------------------------------------

void
CirPoGate::strash( unordered_map < Aig_childs, CirGate* > &map ) {
   if ( travel == global ) return;
   oriptr( child ) -> strash( map );
   travel = global;
}
void
CirAigGate::strash( unordered_map < Aig_childs, CirGate* > &map ) {
   if ( travel == global ) return;
   oriptr( Lchild ) -> strash( map );
   oriptr( Rchild ) -> strash( map );
   unordered_map < Aig_childs, CirGate* >::iterator iter;
   Aig_childs tem = getchild( true );
   iter = map.find( tem );
   if ( iter == map.end() ) map[ tem ] = this;
   else {
      cout<< "Strashing: ";
      ( iter -> second ) -> merge( this );
   }
   travel = global;
}

//----------------------------------strash-------------------------------------------

//---------------------------------simulate------------------------------------------

bool 
CirPoGate::simulate( ){ // just to call aig.simulate()
   if ( travel == global ) return true;
   else travel = global;
   bool a = oriptr( child ) -> simulate(  ) ;
   if ( a ){
      sim = getsim( child );
      return true;
   }
   return false;
}

bool 
CirAigGate::simulate( ){
   if ( travel == global ) return true;
   else travel = global;
   bool a = oriptr( Lchild ) -> simulate(  );
   bool b = oriptr( Rchild ) -> simulate(  );
   if ( a || b ){
      sim = getsim( Lchild ) & getsim( Rchild );
      return true;
   }
   else return false;
}

vector < vector< int > >:: iterator  
CirGate::getfecgrp() const{//get grp;
   for( size_t i = 0 ; i < cirMgr -> FecGrps.size() ; ++i ){
      if ( sim == cirMgr -> cirGate[ abs( cirMgr -> FecGrps[ i ][ 0 ] ) ] -> sim ){
         vector< int > :: iterator  iter = 
            find( cirMgr -> FecGrps[i].begin() , cirMgr -> FecGrps[i].end() , id );
         if ( iter != cirMgr -> FecGrps[i].end() ) return cirMgr -> FecGrps.begin() + i;
         else iter = 
            find( cirMgr -> FecGrps[i].begin() , cirMgr -> FecGrps[i].end() , -id );
         if ( iter != cirMgr -> FecGrps[i].end() ) return cirMgr -> FecGrps.begin() + i;
         else continue;
      }

      else if ( ~sim == cirMgr -> cirGate[ abs( cirMgr -> FecGrps[ i ][ 0 ] ) ] -> sim ){
         vector< int > :: iterator  iter = 
            find( cirMgr -> FecGrps[i].begin() , cirMgr -> FecGrps[i].end() , -id );
         if ( iter != cirMgr -> FecGrps[i].end() ) return cirMgr -> FecGrps.begin() + i;
         else iter = 
            find( cirMgr -> FecGrps[i].begin() , cirMgr -> FecGrps[i].end() , id );
         if ( iter != cirMgr -> FecGrps[i].end() ) return cirMgr -> FecGrps.begin() + i;
         else continue;
      }
   }
   return cirMgr -> FecGrps.end();
}

void 
CirGate::dfs( vector< int > &a ){ }

void 
CirPoGate::dfs( vector< int > &a ){
   if ( travel == global ) return;
   oriptr( child ) -> dfs( a );
   travel = global;
}

void 
CirAigGate::dfs( vector< int > &a ){
   if ( travel == global ) return;
   oriptr( Lchild ) -> dfs( a );
   oriptr( Rchild ) -> dfs( a );
//   cout<<id<<endl;
   a.push_back( id );
   travel = global;
}

void 
CirAigGate::cnf( SatSolver& s ){
   if ( travel == global ) return;
   oriptr( Lchild ) -> cnf( s );
   oriptr( Rchild ) -> cnf( s );
   s.addAigCNF( _var, oriptr( Lchild ) -> getVar() , ptrinvert( Lchild ),
               oriptr( Rchild ) -> getVar(), ptrinvert( Rchild ) );
   travel = global;
}

void 
CirPoGate::cnf( SatSolver& s ){
   if ( travel == global ) return;
   oriptr( child ) -> cnf( s );
   travel = global;
}
