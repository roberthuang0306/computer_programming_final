/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <climits>
#include <cassert>
#include <bitset>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;
extern int global;

bool mycompare ( const vector < int > &a , const vector < int > &b  ){
   return a[0] < b[0] ;
}
bool mycompare2 ( const int &a , const int &b ){
   return abs( a ) < abs( b );
}

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
  random_device rand;
  default_random_engine engine( rand() );
  uniform_int_distribution < unsigned long long > rngen( 0,  ULONG_MAX );

  // dfs simulate
  size_t pattern_num = 0;
  size_t track = FecGrps.size();
  for ( size_t xx = 0 ; xx < used_pattern[ 0 ].size() ; ++ xx ){
     cerr<<'\r'<<"Total #FEC Group = "<<FecGrps.size()<<"          "<<'\b'<<'\b'<<'\b'<<'\b'<<'\b'<<'\b'<<'\b'<<'\b'<<'\b'<<'\b';
     size_t n = FecGrps.size();
     for( size_t i = 0 ; i < piList.size() ; ++i ){ // give pi sim_num
        piList[ i ] -> sim = used_pattern[ i ][xx];
     }

     ++global;
      for( size_t i = 0 ; i < po_number ; ++i ){ // simulate
         cirGate[ piaig_number + 1 + i ] -> simulate();
      }
     if ( _simLog != 0 ){
        for( int i = 0 ; i < 63 ; ++i ){
          size_t a = (size_t)1 << i;
          for ( size_t m = 0 ; m < piList.size() ; ++m ){
             size_t b = a & piList[m] -> sim;
             (b == 0)?*_simLog<<'0':*_simLog<<'1';
          }
          *_simLog<<' ';
          for ( size_t m = 0 ; m < po_number ; ++m ){
             size_t b = a & cirGate[ piaig_number + m + 1 ] -> sim;
             (b == 0)?*_simLog<<'0':*_simLog<<'1';
          }
          *_simLog<<endl;
        }
     }
      if ( FecGrps.empty() ){//如果是第一次
         FecGrps.push_back( vector< int > () );
         ++global;
         if ( _dfsList.size() == 0 ){
            _dfsList.push_back( 0 );
            for ( size_t i = 0; i < po_number ; ++i ){
               cirGate[ piaig_number + i + 1 ] -> dfs( _dfsList );
            }
         }
         FecGrps[0] = _dfsList;
      }
      for ( size_t i = 0 ; i < n ; ++i ){// 每個fecgrp 都要hash 一次
         if ( FecGrps[i].size() == 2 ){
            if ( cirGate[ (FecGrps[i][0]) ] -> sim == cirGate[ abs(FecGrps[i][1]) ] -> sim ){
               FecGrps[i][1] = abs(FecGrps[i][1]);
               FecGrps.push_back( FecGrps[i] );
               continue;
            }
            else if ( cirGate[ abs(FecGrps[i][0]) ] -> sim == ~ cirGate[ abs(FecGrps[i][1]) ] -> sim){
               FecGrps[i][1] = -abs(FecGrps[i][1]);
               FecGrps.push_back( FecGrps[i] );
               continue;
            }
            else continue;
         }
         unordered_map < size_t , vector< int > > fecMap; 
         fecMap.rehash( FecGrps[i].size() );
         unordered_map < size_t , vector< int > > :: iterator iter;
    
        //------hash-----
   
           for ( size_t j = 0 ; j < FecGrps[i].size() ; ++j ){
              if ( FecGrps[i][j] < 0 ) FecGrps[i][j] = -FecGrps[i][j] ;
              iter = fecMap.find( cirGate[ FecGrps[i][j]] -> sim );
              if ( iter != fecMap.end() ) {
                 iter -> second .push_back( FecGrps[i][j] );
                 continue;
              }
              else  // if not found
                 iter = fecMap.find( ~( cirGate[ FecGrps[i][j] ] -> sim ) );
              if ( iter != fecMap.end() ) {
                 FecGrps[i][j] = -FecGrps[i][j] ;
                 iter -> second .push_back( FecGrps[i][j] );
                 continue;
              }
              else {// even the inverse is not found
                 vector < int > tem ;
                 tem.push_back( FecGrps[i][j] );
                 fecMap [ cirGate[ FecGrps[i][j] ] -> sim ] = tem; // creat new vector
              }
           }
   
        //------hash-----
        //-----建立新的vector
   
           for ( size_t k = 0 ; k < fecMap.bucket_count() ; ++k ){// for each bucket
              for ( auto iter = fecMap.begin( k ); iter!=fecMap.end( k ); ++iter ){// for each vector
                 if ( ( iter -> second ).size() < 2 ) continue;
                 else FecGrps.push_back( iter -> second );
               }
           }
      }  
        FecGrps.erase( FecGrps.begin() , FecGrps.begin() + n );
   }
   for( size_t m = 0 ; m < FecGrps.size() ; ++m ){
      sort( FecGrps[m].begin() , FecGrps[m].end() , mycompare2 );
      if ( FecGrps[m][0] < 0 ){
         for ( size_t n = 0 ; n < FecGrps[m].size() ; ++n ){
            FecGrps[m][n] = - FecGrps[m][n];
         }
      }
  }
  cout<<'\r';
  cout<<( used_pattern[0].size()*64 )<<" patterns simulated."<<endl; 
  cout<<"used pattern simulated "<<endl;
  for ( size_t xx = 0 ; xx < cirGate.size()/50 + 5 ; ++xx ){ //xx is the number to sim
     cerr<<'\r'<<"Total #FEC Group = "<<FecGrps.size()<<"          "<<'\b'<<'\b'<<'\b'<<'\b'<<'\b'<<'\b'<<'\b'<<'\b'<<'\b'<<'\b';
     size_t n = FecGrps.size();
     if ( xx % 50 == 0 ){
        if ( track - n > 0 && track - n < 5 ) break;
        else track = n ;
      }
     pattern_num += 64;
     for( size_t i = 0 ; i < piList.size() ; ++i ){ // give pi sim_num
        piList[ i ] -> sim = rngen( engine );
     }
     ++global;
      for( size_t i = 0 ; i < po_number ; ++i ){ // simulate
         cirGate[ piaig_number + 1 + i ] -> simulate();
      }
      if ( _simLog != 0 ){
        for( int i = 0 ; i < 63 ; ++i ){
          size_t a = (size_t)1 << i;
          for ( size_t m = 0 ; m < piList.size() ; ++m ){
             size_t b = a & piList[m] -> sim;
             (b == 0)?*_simLog<<0:*_simLog<<1;
          }
          *_simLog<<' ';
          for ( size_t m = 0 ; m < po_number ; ++m ){
             size_t b = a & cirGate[ piaig_number + m + 1 ] -> sim;
             (b == 0)?*_simLog<<0:*_simLog<<1;
          }
          *_simLog<<endl;
        }
     }
      if ( FecGrps.empty() ){//如果是第一次
         FecGrps.push_back( vector< int > () );
         ++global;
         if ( _dfsList.size() == 0 ){
            _dfsList.push_back( 0 );
            for ( size_t i = 0; i < po_number ; ++i ){
               cirGate[ piaig_number + i + 1 ] -> dfs( _dfsList );
            }
         }
         FecGrps[0] = _dfsList;
      }
      for ( size_t i = 0 ; i < n ; ++i ){// 每個fecgrp 都要hash 一次
         if ( FecGrps[i].size() == 2 ){
            if ( cirGate[ (FecGrps[i][0]) ] -> sim == cirGate[ abs(FecGrps[i][1]) ] -> sim ){
               FecGrps[i][1] = abs(FecGrps[i][1]);
               FecGrps.push_back( FecGrps[i] );
               continue;
            }
            else if ( cirGate[ abs(FecGrps[i][0]) ] -> sim == ~ cirGate[ abs(FecGrps[i][1]) ] -> sim){
               FecGrps[i][1] = -abs(FecGrps[i][1]);
               FecGrps.push_back( FecGrps[i] );
               continue;
            }
            else continue;
         }
         unordered_map < size_t , vector< int > > fecMap; 
         fecMap.rehash( FecGrps[i].size() );
         unordered_map < size_t , vector< int > > :: iterator iter;
    
        //------hash-----
   
           for ( size_t j = 0 ; j < FecGrps[i].size() ; ++j ){
              if ( FecGrps[i][j] < 0 ) FecGrps[i][j] = -FecGrps[i][j] ;
              iter = fecMap.find( cirGate[ FecGrps[i][j]] -> sim );
              if ( iter != fecMap.end() ) {
                 iter -> second .push_back( FecGrps[i][j] );
                 continue;
              }
              else  // if not found
                 iter = fecMap.find( ~( cirGate[ FecGrps[i][j] ] -> sim ) );
              if ( iter != fecMap.end() ) {
                 FecGrps[i][j] = -FecGrps[i][j] ;
                 iter -> second .push_back( FecGrps[i][j] );
                 continue;
              }
              else {// even the inverse is not found
                 vector < int > tem ;
                 tem.push_back( FecGrps[i][j] );
                 fecMap [ cirGate[ FecGrps[i][j] ] -> sim ] = tem; // creat new vector
              }
           }
   
        //------hash-----
        //-----建立新的vector
   
           for ( size_t k = 0 ; k < fecMap.bucket_count() ; ++k ){// for each bucket
              for ( auto iter = fecMap.begin( k ); iter!=fecMap.end( k ); ++iter ){// for each vector
                 if ( ( iter -> second ).size() < 2 ) continue;
                 else FecGrps.push_back( iter -> second );
               }
           }
      }  
        FecGrps.erase( FecGrps.begin() , FecGrps.begin() + n );
   }
   for( size_t m = 0 ; m < FecGrps.size() ; ++m ){
      sort( FecGrps[m].begin() , FecGrps[m].end() , mycompare2 );
      if ( FecGrps[m][0] < 0 ){
         for ( size_t n = 0 ; n < FecGrps[m].size() ; ++n ){
            FecGrps[m][n] = - FecGrps[m][n];
         }
      }
   }
   sort( FecGrps.begin() , FecGrps.end() , mycompare );
   cout<<'\r';
   cout<<( pattern_num + used_pattern[0].size()*64 )<<" patterns simulated."<<endl;
}

void
CirMgr::fileSim(ifstream& patternFile)
{
   size_t Pattern[ piList.size() ] ;
   string str;
   int pattern_num = 0;
   int abort = 64;
   while( patternFile.peek() != EOF ) {
      for ( size_t i = 0 ; i < piList.size() ; ++i ) Pattern[ i ] = 0;
      for( size_t j = 0 ; j < 64 ; ++j ){//64個組成一個size_t
         if ( patternFile >> str ){
            if ( str.size() != piList.size() ) {
               cerr<<'\n'<<"Error: Pattern("<<str<<") length("
               <<str.size()<<") does not match the number of inputs("
               <<piList.size()<<") in a circuit!!"<<endl;     
               for( size_t m = 0 ; m < FecGrps.size() ; ++m ){
                  sort( FecGrps[m].begin() , FecGrps[m].end() , mycompare2 );
                  if ( FecGrps[m][0] < 0 ){
                      for ( size_t n = 0 ; n < FecGrps[m].size() ; ++n ){
                          FecGrps[m][n] = - FecGrps[m][n];
                      }
                  }
               }
               sort( FecGrps.begin() , FecGrps.end() , mycompare );
               cout<<'\r';
               cout<<64*((pattern_num)/64)<<" patterns simulated."<<endl;
               return;
            }
            for ( size_t i = 0 ; i < piList.size() ; ++i ){
               if ( str[i] != '0' && str[i] != '1' ){
                  cerr<<'\n'<<"Error: Pattern("<<str
                      <<") contains a non-0/1 character('"<<str[i]<<"')."<<endl;                 
                  for( size_t m = 0 ; m < FecGrps.size() ; ++m ){
                     sort( FecGrps[m].begin() , FecGrps[m].end() , mycompare2 );
                     if ( FecGrps[m][0] < 0 ){
                        for ( size_t n = 0 ; n < FecGrps[m].size() ; ++n ){
                           FecGrps[m][n] = - FecGrps[m][n];
                        }
                     }
                  }
                  sort( FecGrps.begin() , FecGrps.end() , mycompare );
                  cout<<'\r';
                  cout<<64*((pattern_num)/64)<<" patterns simulated."<<endl;
                  return;
               }
               Pattern[ i ] |= ( (size_t)( str[i] - 48 ) ) << j ;
            }

            ++pattern_num;
            cerr<<'\r'<<"Total #FEC Group = "<<FecGrps.size()<<"          "<<'\b'<<'\b'<<'\b'<<'\b'<<'\b'<<'\b'<<'\b'<<'\b'<<'\b'<<'\b';
         }
         else {
            if ( j == 0 ){
               for( size_t m = 0 ; m < FecGrps.size() ; ++m ){
                  sort( FecGrps[m].begin() , FecGrps[m].end() , mycompare2 );
                  if ( FecGrps[m][0] < 0 ){
                     for ( size_t n = 0 ; n < FecGrps[m].size() ; ++n ){
                        FecGrps[m][n] = - FecGrps[m][n];
                     }
                  }
               }
               sort( FecGrps.begin() , FecGrps.end() , mycompare );
               cout<<'\r';
               cout<<pattern_num<<" patterns simulated."<<endl;
               return;
            }
            abort = j;
            break;
         }
      } 

      for( size_t i = 0 ; i < piList.size() ; ++i ){ // give pi sim_num
        piList[ i ] -> sim = Pattern[ i ];
      }   

      ++global;
      for( size_t i = 0 ; i < po_number ; ++i ){ // simulate
         cirGate[ piaig_number + 1 + i ] -> simulate();
      }

      if ( _simLog != 0 ){
 //       cout<<"abort = "<<abort<<endl;
        for( int i = 0 ; i < abort ; ++i ){
          size_t a = (size_t)1 << i;
 //         cout<< " a = "<<bitset<64> (a)<<endl;
          for ( size_t m = 0 ; m < piList.size() ; ++m ){
             size_t b = a & piList[m] -> sim;
             (b == 0)?*_simLog<<0:*_simLog<<1;
          }
          *_simLog<<' ';
          for ( size_t m = 0 ; m < po_number ; ++m ){
             size_t b = a & cirGate[ piaig_number + m + 1 ] -> sim;
             (b == 0)?*_simLog<<0:*_simLog<<1;
          }
          *_simLog<<endl;
        }
     }
      if ( FecGrps.empty() ){//如果是第一次
         FecGrps.push_back( vector< int > () );
         ++global;
         if ( _dfsList.size() == 0 ){
            _dfsList.push_back( 0 );
            for ( size_t i = 0; i < po_number ; ++i ){
               cirGate[ piaig_number + i + 1 ] -> dfs( _dfsList );
            }
         }
         FecGrps[0] = _dfsList;
      }
      size_t n = FecGrps.size();
       
      for ( size_t i = 0 ; i < n ; ++i ){// 每個fecgrp 都要hash 一次
         if ( FecGrps[i].size() == 2 ){
            if ( cirGate[ (FecGrps[i][0]) ] -> sim == cirGate[ abs(FecGrps[i][1]) ] -> sim ){
               FecGrps[i][1] = abs(FecGrps[i][1]);
               FecGrps.push_back( FecGrps[i] );
               continue;
            }
            else if ( cirGate[ abs(FecGrps[i][0]) ] -> sim == ~ cirGate[ abs(FecGrps[i][1]) ] -> sim){
               FecGrps[i][1] = -abs(FecGrps[i][1]);
               FecGrps.push_back( FecGrps[i] );
               continue;
            }
            else continue;
         }
         unordered_map < size_t , vector< int > > fecMap; 
         fecMap.rehash( FecGrps[i].size() );
         unordered_map < size_t , vector< int > > :: iterator iter;
    
        //------hash-----
   
           for ( size_t j = 0 ; j < FecGrps[i].size() ; ++j ){
              if ( FecGrps[i][j] < 0 ) FecGrps[i][j] = -FecGrps[i][j] ;
              iter = fecMap.find( cirGate[ FecGrps[i][j]] -> sim );
              if ( iter != fecMap.end() ) {
                 iter -> second .push_back( FecGrps[i][j] );
                 continue;
              }
              else  // if not found
                 iter = fecMap.find( ~( cirGate[ FecGrps[i][j] ] -> sim ) );
              if ( iter != fecMap.end() ) {
                 FecGrps[i][j] = -FecGrps[i][j] ;
                 iter -> second .push_back( FecGrps[i][j] );
                 continue;
              }
              else {// even the inverse is not found
                 vector < int > tem ;
                 tem.push_back( FecGrps[i][j] );
                 fecMap [ cirGate[ FecGrps[i][j] ] -> sim ] = tem; // creat new vector
              }
           }
   
        //------hash-----
        //-----建立新的vector
   
           for ( size_t k = 0 ; k < fecMap.bucket_count() ; ++k ){// for each bucket
              for ( auto iter = fecMap.begin( k ); iter!=fecMap.end( k ); ++iter ){// for each vector
                 if ( ( iter -> second ).size() < 2 ) continue;
                 else FecGrps.push_back( iter -> second );
               }
           }
      }  
        FecGrps.erase( FecGrps.begin() , FecGrps.begin() + n );
   }
   for( size_t m = 0 ; m < FecGrps.size() ; ++m ){
      sort( FecGrps[m].begin() , FecGrps[m].end() , mycompare2 );
      if ( FecGrps[m][0] < 0 ){
         for ( size_t n = 0 ; n < FecGrps[m].size() ; ++n ){
            FecGrps[m][n] = - FecGrps[m][n];
         }
      }
   }
   sort( FecGrps.begin() , FecGrps.end() , mycompare );
   cout<<'\r';
   cout<<pattern_num<<" patterns simulated."<<endl;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
