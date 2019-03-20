/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;
extern int global;

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
   fecgrpexist = true;
   // dfs simulate
   if ( _dfsList.size() == 0 ){
      _dfsList.push_back( 0 );
      ++global;
      for ( size_t i = 0; i < po_number ; ++i ){
         cirGate[ piaig_number + i + 1 ] -> dfs( _dfsList );
      }
   }
   if ( FecGrps.empty() ){//如果是第一次
      FecGrps.push_back( vector< int > () );
      FecGrps[0] = _dfsList;
   }
   size_t pattern_num = 0;
   int abort = 64;
   for ( size_t xx = 0 ; xx < used_pattern[ 0 ].size() ; ++ xx ){
      cerr<<'\r'<<"Total #FEC Group = "<<FecGrps.size()<<" "<<'\b';
      for( size_t i = 0 ; i < piList.size() ; ++i ){ // give pi sim_num
         piList[ i ] -> sim = used_pattern[ i ][xx];
      }

      ++global;
      for( size_t i = 0 ; i < po_number ; ++i ){ // simulate
         cirGate[ piaig_number + 1 + i ] -> simulate();
      }

      if ( _simLog != 0 ){
        for( int i = 0 ; i < abort ; ++i ){
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
      fecgrps_hash();   
   }
   for ( size_t i = 0 ; i < piList.size() ; ++i ) used_pattern[ i ].clear();
   int limit = 0 ;
   for ( size_t xx = 0 ; xx < cirGate.size()/64 ; ++xx ){ //xx is the number to sim
      cerr<<'\r'<<"Total #FEC Group = "<<FecGrps.size()<<" "<<'\b';
      pattern_num += 64;
      for( size_t i = 0 ; i < piList.size() ; ++i ){ // give pi sim_num
         piList[ i ] -> sim = rngen( engine );
      }
      
      ++global;
      for( size_t i = 0 ; i < po_number ; ++i ){ // simulate
         cirGate[ piaig_number + 1 + i ] -> simulate();
      }

      if ( _simLog != 0 ){
        for( int i = 0 ; i < abort ; ++i ){
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
      limit += fecgrps_hash();  
      if( xx % 5 == 4 ){
         if( limit <= lim ) break;
         else limit = 0;
      }
   }
   for( size_t m = 0 ; m < _dfsList.size() ; ++m ){
      int id = _dfsList[ m ];
      cirGate[ id ] -> fecgrpnum = -1;
   }
   for( size_t m = 0 ; m < FecGrps.size() ; ++m ){
      for ( size_t n = 0 ; n < FecGrps[ m ].size() ; ++n ){
         cirGate[ FecGrps[ m ][ n ] ] -> fecgrpnum = m;
      }
   }

   cout<<'\r';
   cout<<( pattern_num + used_pattern[0].size()*64 )<<" patterns simulated."<<endl;
}

void
CirMgr::fileSim(ifstream& patternFile)
{
   ++global;
   if ( _dfsList.size() == 0 ){
      _dfsList.push_back( 0 );
      for ( size_t i = 0; i < po_number ; ++i ){
         cirGate[ piaig_number + i + 1 ] -> dfs( _dfsList );
      }
   }

   if ( FecGrps.empty() ){//如果是第一次
      FecGrps.push_back( vector< int > () );
      FecGrps[0] = _dfsList;
   }
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
                  for ( size_t n = 0 ; n < FecGrps[m].size() ; ++n ){
                     cirGate[ FecGrps[m][n] ] -> fecgrpnum = m;
                  }
               }
               cout<<'\r';
               cout<<64*((pattern_num)/64)<<" patterns simulated."<<endl;
               return;
            }
            for ( size_t i = 0 ; i < piList.size() ; ++i ){
               if ( str[i] != '0' && str[i] != '1' ){
                  cerr<<'\n'<<"Error: Pattern("<<str
                      <<") contains a non-0/1 character('"<<str[i]<<"')."<<endl;     
                  for( size_t m = 0 ; m < FecGrps.size() ; ++m ){
                     for ( size_t n = 0 ; n < FecGrps[m].size() ; ++n ){
                        cirGate[ FecGrps[m][n] ] -> fecgrpnum = m;
                     }
                  }
                  cout<<'\r';
                  cout<<64*((pattern_num)/64)<<" patterns simulated."<<endl;
                  return;
               }
               Pattern[ i ] |= ( (size_t)( str[i] - 48 ) ) << j ;
            }
            ++pattern_num;
            cerr<<'\r'<<"Total #FEC Group = "<<FecGrps.size()<<" "<<'\b';
         }
         else {
            if ( j == 0 ){
               for( size_t m = 0 ; m < FecGrps.size() ; ++m ){
                  for ( size_t n = 0 ; n < FecGrps[m].size() ; ++n ){
                     cirGate[ FecGrps[m][n] ] -> fecgrpnum = m;
                  }
               }
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
        for( int i = 0 ; i < abort ; ++i ){
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
      fecgrps_hash();  
   }
   for( size_t m = 0 ; m < _dfsList.size() ; ++m ){
      int id = _dfsList[ m ];
      cirGate[ id ] -> fecgrpnum = -1;
   }
   for( size_t m = 0 ; m < FecGrps.size() ; ++m ){
      for ( size_t n = 0 ; n < FecGrps[ m ].size() ; ++n ){
         cirGate[ FecGrps[ m ][ n ] ] -> fecgrpnum = m;
      }
   }

   cout<<'\r';
   cout<<pattern_num<<" patterns simulated."<<endl;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/

size_t 
CirMgr::fecgrps_hash(){
   size_t n = FecGrps.size();
   size_t found = 0 ;
   for ( size_t i = 0 ; i < n ; ++i ){// 每個fecgrp 都要hash 一次
      if ( FecGrps[ i ].size() <= 1 ) continue;
      if ( FecGrps[ i ].size() == 2 ){
         size_t member_1 = FecGrps[i][0];
         size_t member_2 = FecGrps[i][1];
         if ( cirGate[ member_1 ] == 0 ) continue;
         if ( cirGate[ member_2 ] == 0 ) continue;
         if ( cirGate[ member_1 ] -> sim ==  cirGate[ member_2 ] -> sim ||
              cirGate[ member_1 ] -> sim == ~cirGate[ member_2 ] -> sim ){
            FecGrps.push_back( FecGrps[i] );
            continue;
         }
         else{
            found += 2;
            continue;
         }
      }
      unordered_map < size_t , vector< int > > fecMap; 
      fecMap.rehash( FecGrps[i].size() );
      unordered_map < size_t , vector< int > > :: iterator iter;
    
      //------hash-----
      for ( size_t j = 0 ; j < FecGrps[i].size() ; ++j ){
         size_t id = FecGrps[i][j];
         if ( cirGate[ id ] == 0 )continue;
         iter = fecMap.find( cirGate[ id ] -> sim );
         if ( iter != fecMap.end() ) {
            iter -> second.push_back( id );
            continue;
         }
         else  // if not found
            iter = fecMap.find( ~( cirGate[ id ] -> sim ) );
         if ( iter != fecMap.end() ) {
            iter -> second.push_back( id );
            continue;
         }
         else {// even the inverse is not found
            vector < int > tem ;
            tem.push_back( id );
            fecMap [ cirGate[ id ] -> sim ] = tem; // creat new vector
         }
      }
      //------hash-----
      //-----建立新的vector
      for ( size_t k = 0 ; k < fecMap.bucket_count() ; ++k ){// for each bucket
         for ( auto iter = fecMap.begin( k ); iter!=fecMap.end( k ); ++iter ){// for each vector
            if ( ( iter -> second ).size() < 2 ) ++found;
            else FecGrps.push_back( iter -> second );
         }
      }
   }  
   FecGrps.erase( FecGrps.begin() , FecGrps.begin() + n );
   return found;
}
