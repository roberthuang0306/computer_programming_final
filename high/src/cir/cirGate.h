/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <sat.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include "cirDef.h"
#include <bitset>
#include <unordered_map>

using namespace std;
class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class Aig_childs{//return type of aig::getchild     
  public:
    Aig_childs( int a, int b ):Lchild(a), Rchild(b) { 
      if ( Lchild > Rchild ) swap( Lchild, Rchild );
    }
    bool operator == ( const Aig_childs & another ) const{
      if ( Lchild == another.Lchild && Rchild == another.Rchild ) return true;
      return false;
    }
    friend ostream& operator << ( ostream& os, Aig_childs& p) {
        os << "Aig_childs: " << p.Lchild << ", " << p.Rchild ;
        return os;
    }
    int Lchild;//lchild<=rchild
    int Rchild;
};

class CirGate
{
  friend class CirMgr;
  friend class CirAigGate;
  friend class CirPoGate;
  public:
    CirGate(){}
    CirGate( int a ):id(a) {}
    virtual ~CirGate() {}

    // Basic access methods

    virtual void dfs( vector< int > &a );

    virtual void strash( unordered_map < Aig_childs, CirGate* > &map ) {}

    virtual bool fltfanin() const{ return false ;}

    virtual void setchild( CirGate* a ){}

    virtual void setchild( CirGate* a , CirGate* b ){};

    virtual void reportFanin(int level , bool inverse = false , int space = 0 ) ;

    virtual void reportFanout(int level , bool inverse = false , int space = 0 ) ;

    virtual void printGate( int &clock ) ;

    virtual void printAig( ostream &a ) {}

    virtual CirGate* getchild() const { return 0; }

    virtual Aig_childs getchild( bool x ) const { return Aig_childs( -1, -1); }

    virtual void countaig( int &a ) {}

    virtual bool isAig() const { return false; } //new todo
    
    virtual void sweeping( vector<bool> &a );

    virtual bool simplify( CirGate* &_const0 ) { return false; }

    virtual void cnf( SatSolver& s ) {}

    virtual void var( SatSolver& s );

    unsigned     getLineNo()  const{ return lineno; }

    int          getid()      const{ return id;}

    void         setname( const string str ) { name = str; }

    string       getname(){return name;}

    void         pushparent ( CirGate* a ) { parent.push_back(a); }

    void         reportGate() const;
    
    void         sort();

    void         merge( CirGate * a ) ;

    void         pushparent ( vector<CirGate*> &a ) { 
       parent.insert( parent.end() , a.begin() , a.end() );
    }

    virtual size_t operator () () const{
      return 0;
    }

    bool operator == (const CirGate &other) const {
      return false;
    }

    virtual string getTypeStr() const{ 
       if ( id   == 0 ) return "CONST" ;
       return "UNDEF" ;
    }

    bool ptrinvert ( CirGate* const &a ) const {
       size_t tem = (size_t)a;
       return (bool)(tem%2);
    }
    bool existparent() { 
       if ( getTypeStr() == "PO" )return true;
       if ( parent.empty() )return false;
       return true;
    }

    int dec( int a ) const {
       int b = 0;
       do{
         a/=10;
         ++b;
       }while( a!=0 );
       return b;
    }

    bool delparent( CirGate* a ){
       vector<CirGate*>::iterator iter;
       iter = find( parent.begin() , parent.end() , a );
       if ( iter == parent.end() ) {
         iter = find( parent.begin() , parent.end() , ptrplus(a));
       }
       if ( iter == parent.end() ) return false;
       swap( *iter , *( parent.rbegin() ) );
       parent.pop_back();
       return true;
    }
   
    CirGate* popparent (){ 
       if (parent.size() == 0)return 0;
       CirGate* a = parent.back(); 
       parent.pop_back();
       return a;
    };

    CirGate* ptrminus ( CirGate* const &a ) const {
       size_t tem = (size_t)a;
       --tem;
       CirGate* b = (CirGate*)tem;
       return b;
    }

    CirGate* ptrplus ( CirGate* const &a ) const {
       size_t tem = (size_t)a;
       ++tem;
       CirGate* b = (CirGate*)tem;
       return b;
    }

    CirGate* oriptr ( CirGate* const &a ) const {
       if ( ptrinvert(a) ) return ptrminus(a);
       else return a;
    }

    CirGate* invertptr ( CirGate* const &a ) const {
       if ( ptrinvert(a) ) return ptrminus(a);
       else return ptrplus(a);
    }

    vector<CirGate*> invertptr ( vector<CirGate*> &a ){
       for ( size_t i = 0 ; i < a.size() ; ++i ){
          a[i] = invertptr( a[i] );
       }
       return a;
    }

    void setVar( Var v ){ _var = v; }

    Var getVar(){ return _var; }

    vector< vector< int > >:: iterator  getfecgrp()const ;//get grp.begin();

    virtual bool simulate ( ) { return true; }

    
  protected:
    vector<CirGate*> parent ;
    string           name      = "" ;
    size_t           sim       = 0  ;
    unsigned         lineno    = 0  ;
    int              fecgrpnum = -1 ;
    int              travel    = 0  ;
    int              id;
    Var              _var;
};

//-----------------------------------------------------------------------------------------//

class CirAigGate : public CirGate
{
  friend class CirMgr;
  friend class CirGate;
  public:

    void cnf( SatSolver& s );
    
    void var( SatSolver& s );

    CirAigGate( int a, unsigned b) { id=a; lineno=b; }

    void dfs( vector< int > &a );

    void strash( unordered_map < Aig_childs, CirGate* > &map ) ;

    void setchild( CirGate* a , CirGate* b ) ;

    void printAig( ostream &a ) ;

    bool simplify( CirGate* &_const0  ) ;

    void sweeping( vector<bool> &a  ) ;

    void printGate( int & clock) ;

    bool simulate ( ) ;

    void reportFanin(int level , bool inverse = false ,int space = 0) ;

    Aig_childs getchild ( bool x ) const {
      int a = oriptr( Lchild ) -> getid();
      int b = oriptr( Rchild ) -> getid();
      a *= 2;
      b *= 2;
      if ( ptrinvert( Lchild ) ) a += 1;
      if ( ptrinvert( Rchild ) ) b += 1;
      return Aig_childs( a, b );
    }

    bool operator == (const CirGate &other) const {
      if ( other.getTypeStr() != "AIG" ) return false;
      CirAigGate* tem = (CirAigGate*)((size_t)&other);
      if ( tem -> Lchild == Lchild &&
           tem -> Rchild == Rchild ){
         return true;
      }
      return false;
    }

    void setchild(CirGate* a){
      if( Lchild == 0 ) Lchild = a;
      else Rchild = a;
    }

    bool fltfanin() const{
      if ( oriptr(Lchild) -> getTypeStr() == "UNDEF" ) return true ;
      if ( oriptr(Rchild) -> getTypeStr() == "UNDEF" ) return true ;   
      return false;
    }
    string getTypeStr() const{ 
      return "AIG" ;
    }

    size_t getsim( CirGate* a ){
      if ( ptrinvert( a ) ) return ~(oriptr( a ) -> sim );
      else return a -> sim;
    }

    void countaig( int &a ) ;
  private:
    CirGate* Lchild =0;
    CirGate* Rchild =0;
};

//-----------------------------------------------------------------------------------------//

class CirPiGate : public CirGate
{
  public:

    CirPiGate( int a, unsigned b ) {id=a; lineno=b;}

    string  getname(){ return name;};

    string getTypeStr() const{ return "PI" ; }

    void printGate( int & clock) ;

};

//-----------------------------------------------------------------------------------------//

class CirPoGate : public CirGate
{
  public:
    CirPoGate( int a, unsigned b, CirGate* gate ) :child(gate){id=a; lineno=b; }
    CirPoGate( int a, unsigned b ) {id=a; lineno=b;}
  
    size_t getsim( CirGate* a ){
      if ( ptrinvert( a ) ) return ~(oriptr( a ) -> sim );
      else return a -> sim;
    }

    void cnf( SatSolver& s );

    void var( SatSolver& s );

    void dfs( vector< int > &a );

    bool simulate ( ) ;

    void strash( unordered_map < Aig_childs, CirGate* > &map ) ;

    bool simplify( CirGate* &_const0  ) ;
    
    void printGate( int & clock) ;

    void reportFanin(int level , bool inverse = false ,int space = 0) ;

    void reportFanout(int level , bool inverse = false ,int space = 0) ;

    void printAig( ostream &a ) { oriptr(child) -> printAig( a ); }

    void setchild ( CirGate* a ){ child = a; }

    void setchild( CirGate* a , CirGate* b ) ;

    void countaig ( int &a ){ oriptr(child) -> countaig(a); }

    bool fltfanin() const{
      if ( oriptr(child) -> getTypeStr() == "UNDEF" ) return true ;
      else return false;
    }

    string  getname(){ return name;};

    string getTypeStr() const{ return "PO" ; }

    CirGate* getchild() const{ return child; }

  private:
    CirGate* child =0;
};
#endif // CIR_GATE_H
