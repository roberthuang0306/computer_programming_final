/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <algorithm>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <fstream>
#include <random>
#include <climits>
#include <cassert>
#include <bitset>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;
extern int global;


// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/

bool mycompare ( const vector < int > &a , const vector < int > &b  ){
   return a[0] < b[0] ;
}
CirMgr::~CirMgr(){
   for( size_t i = 0 ; i < cirGate.size() ; ++i ){
      if ( cirGate[i] != 0 ) delete cirGate[i];
   }
}
bool
CirMgr::readCircuit(const string& fileName)
{
   global = 0;
   ifstream inf;
   inf.open(fileName);
   if ( !inf ){
      cerr<<"Cannot open design \""<<fileName<<"\"!!"<<endl;
      return false;
   }
   string str;
   getline(inf,str); //cout<<str<<endl;
   string tok;
   size_t pos;
   pos = myStrGetTok( str, tok );//could check if tok == "aag"
   int miloa[5] = { -1,-1,-1,-1,-1};
   for( int i = 0; i<5; ++i){//get m, i, l, o, a
      pos = myStrGetTok( str, tok, pos ,' ');
      myStr2Int( tok, miloa[i]);
   }
   piaig_number = miloa[ 0 ];
   po_number = miloa[ 3 ];
   FecGrps.clear();
   cirGate.clear();
   piList.clear();
   cirGate.reserve( miloa[0]+2+miloa[3] );
   cirGate.insert( cirGate.begin(), miloa[0]+1+miloa[3], 0 );
   cirGate[0] = new CirGate(0);
//------------------------creat pi-------------------------------------
   for( int i = 0; i < miloa[1]; ++i){//I     
      getline(inf,str);
      pos = myStrGetTok( str, tok ); 
      int pi;
      myStr2Int( tok, pi );
      pi /= 2;//could check if pi is even
      cirGate[pi] = new CirPiGate(pi, 2+i);
      piList.push_back( cirGate[pi] );
   }

   used_pattern.reserve( piList.size() );
   for( size_t i = 0 ; i < piList.size() ; ++i ){
      used_pattern.push_back( vector< size_t >() );
   }
//------------------------creat po-------------------------------------
   for( int i = 0; i < miloa[3]; ++i){//O
      getline(inf,str);
      cirGate[miloa[0] +1 +i] = new CirPoGate( miloa[0] +1 +i, miloa[1] +2 +i );
   }
//------------------------creat known Aig-------------------------------------
   for ( int i = 0; i<miloa[4];++i){//creat known A
      getline(inf,str);
      pos = myStrGetTok( str, tok );
      int num;
      myStr2Int( tok, num);
      cirGate[num/2] = new CirAigGate(num/2,miloa[1]+miloa[3]+2+i);
   }
   inf.close();
   inf.open(fileName);
   getline(inf ,str);
   for ( int i = 0;i<miloa[1] ;++i)getline(inf,str);
//------------------------connect Po-------------------------------------
   for( int i = 0; i < miloa[3]; ++i){//O
      getline(inf,str);
      int po;
      pos = myStrGetTok( str, tok );
      myStr2Int( tok, po );//could check if pos == npos
      if ( cirGate[po/2] ==0 ) cirGate[po/2] = new CirGate(po/2);
      if ( po%2 == 0 ){
         cirGate[miloa[0] +1 +i] -> setchild(cirGate[po/2] );
         cirGate[po/2] -> pushparent ( cirGate[miloa[0] +1 +i] );
      }
      else {
         cirGate[miloa[0] +1 +i] -> setchild(ptrplus(cirGate[po/2]) );
         cirGate[po/2] -> pushparent ( ptrplus(cirGate[miloa[0] +1 +i]) );
      }
   }   
//------------------------creat Aig-------------------------------------
   for( int i = 0; i < miloa[4]; ++i){//A
      getline(inf,str);
      int aig[3];
      pos = myStrGetTok( str, tok ); 
      myStr2Int( tok, aig[0] );//could check if tok represent an integer
      pos = myStrGetTok( str, tok, pos,' ' ); //could check is pos = pos+2;
      myStr2Int( tok, aig[1] );
      pos = myStrGetTok( str, tok, pos,' ' );  
      myStr2Int( tok, aig[2] );
      if(cirGate[aig[1]/2] == 0) cirGate[aig[1]/2] = new CirGate( aig[1]/2 );
      if(cirGate[aig[2]/2] == 0) cirGate[aig[2]/2] = new CirGate( aig[2]/2 );
      //setchild
      if(aig[1]%2 == 0) cirGate[aig[0]/2] -> setchild( cirGate[aig[1]/2]);
      else cirGate[aig[0]/2] -> setchild( ptrplus(cirGate[aig[1]/2]));
      if(aig[2]%2 == 0) cirGate[aig[0]/2] -> setchild( cirGate[aig[2]/2]);
      else cirGate[aig[0]/2] -> setchild( ptrplus(cirGate[aig[2]/2]));

      if(aig[1]%2 == 0) cirGate[aig[1]/2] -> pushparent( cirGate[aig[0]/2] );
      else cirGate[aig[1]/2] -> pushparent( ptrplus(cirGate[aig[0]/2]) );
      if(aig[2]%2 == 0) cirGate[aig[2]/2] -> pushparent( cirGate[aig[0]/2] );
      else cirGate[aig[2]/2] -> pushparent( ptrplus(cirGate[aig[0]/2]) );
   }
//------------------------names-------------------------------------
   while ( getline ( inf ,str ) ) {
      if (str == "c")break;
      pos = myStrGetTok( str, tok ); //could check if tok = i/o + integer
      if (tok.front()=='i') {
         tok.erase(0,1);
         int num;
         myStr2Int( tok, num ); 
         str.erase ( 0,pos+1);
         piList[num] -> setname(str)  ;     
      }
      if (tok.front()=='o'){
         tok.erase(0,1);
         int num;
         myStr2Int( tok, num ); 
         str.erase ( 0,pos+1);
         cirGate[ piaig_number + num + 1 ] -> setname(str)  ;           
      }
   }
   inf.close();
   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   int a = 0;
   
   for ( size_t i = 1 ; i < cirGate.size() ; ++i ){
      if ( cirGate[i] != 0 && cirGate[i] -> getTypeStr() == "AIG" )
      ++a;
   }
   
   cout<<endl;
   cout<<"Circuit Statistics"<<endl;
   cout<<"=================="<<endl;
   cout<<"  PI"<<right<<setw(12)<<piList.size()<<endl;
   cout<<"  PO"<<right<<setw(12)<<po_number<<endl;
   cout<<"  AIG"<<right<<setw(11)<<a<<endl;
   cout<<"------------------"<<endl;
   cout<<"  Total"<<right<< setw(9)<<piList.size()+po_number+a<<endl;
}

void
CirMgr::printNetlist() const
{
   ++global;
   int a =0;
   for ( size_t i = 0 ; i < po_number ; ++i ) 
      cirGate[ piaig_number + i + 1 ] -> printGate(a);
   cout<<endl;
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for ( size_t i = 0 ; i < piList.size() ; ++i ){
      cout<<' '<<piList[i] -> getid();
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for ( size_t i = 0 ; i < po_number ; ++i ){
      cout<<' '<<cirGate[ piaig_number + i + 1 ] -> getid();
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   bool exist = false;
   for ( size_t i = 1 ; i < cirGate.size() ; ++i ){
      if ( cirGate[i] != 0 && cirGate[i] -> fltfanin()==true ) {
         if( exist == false ){
            cout<<"Gates with floating fanin(s):";
            exist = true;
         }
         cout<<' '<<i;
      }
   }
   if ( exist == true )cout<<endl;
   exist = false;
   for ( size_t i = 1 ; i < cirGate.size() ; ++i ){
      if ( cirGate[i] != 0 && cirGate[i] -> existparent()==false ) {
         if( exist == false ){
            cout<<"Gates defined but not used  :";
            exist = true;
         }
         cout<<' '<<i;
      }
   }  
   if ( exist == true )cout<<endl;
}

void
CirMgr::printFECPairs() const
{
   if ( fecgrpexist == false )return;
   auto FecGrps_copy = FecGrps;
   for( size_t m = 0 ; m < FecGrps_copy.size() ; ++m )
      sort( FecGrps_copy[m].begin() , FecGrps_copy[m].end() );
   sort( FecGrps_copy.begin() , FecGrps_copy.end() , mycompare );
   for ( size_t i = 0 ; i < FecGrps_copy.size() ; ++i ){
      cout<<'['<<i<<']';
      for ( size_t j = 0 ; j < FecGrps_copy[i].size() ; ++j ){
         if ( cirGate[ FecGrps_copy[i][j] ] -> sim != cirGate[ FecGrps_copy[i][0] ] -> sim )
            cout<< ' '<< '!' << FecGrps_copy[i][j] ;
         else cout<< ' '<< FecGrps_copy[i][j] ;
      }
      cout<<endl;
   }
}

void
CirMgr::writeAag(ostream& outfile) const
{
   int a = countAig();
   outfile<<"aag"<<' '<<cirGate.size()-po_number-1<<' '<<piList.size()<<' '<<'0'
       <<' '<<po_number<<' '<<a<<endl;
   for ( size_t i = 0 ; i < piList.size() ; ++i )
      outfile<< 2*(piList[i] -> getid())<<endl;
   for ( size_t i = 0 ; i < po_number ; ++i ){
      if ( cirGate[ piaig_number + i + 1 ] -> ptrinvert (cirGate[ piaig_number + i + 1 ] -> getchild()))
         outfile<< 2*(cirGate[ piaig_number + i + 1 ] -> oriptr(cirGate[ piaig_number + i + 1 ] -> getchild()) -> getid())+1<<endl;
      else
         outfile<< 2*(cirGate[ piaig_number + i + 1 ] -> getchild() -> getid())<<endl;
   }
   ++global;
   for ( size_t i = 0 ; i < po_number ; ++i ){
      cirGate[ piaig_number + i + 1 ] -> printAig( outfile );
   }
   for ( size_t i = 0 ; i < piList.size() ; ++i ){
      if ( (piList[i] -> getname()).size() > 0 )
      outfile<<'i'<<i<<' '<<piList[i] -> getname()<<endl;
   }
   for ( size_t i = 0 ; i < po_number ; ++i ){
      if ( (cirGate[ piaig_number + i + 1 ] -> getname()).size() > 0 )
      outfile<<'o'<<i<<' '<<cirGate[ piaig_number + i + 1 ] -> getname()<<endl;
   }
   outfile<<'c'<<endl;
   outfile<<"Ric 好帥"<<endl;
}

int 
CirMgr::countAig()const
{
   ++global;
   int a = 0;
   for (size_t i =0 ;i < po_number ; ++i ){
      cirGate[ piaig_number + i + 1 ] -> countaig( a );
   }
   return a;
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
}
