/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

extern int global;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed

void 
CirMgr::sweep(){
   ++global;
   vector < bool > sweepingGate ( cirGate.size() , false );
   for ( size_t i = 1 ; i < cirGate.size() ; ++i ){
      if ( cirGate[i] != 0 && cirGate[i] -> existparent()==false ) {//flt Aig
         cirGate[i] -> sweeping( sweepingGate  );        
      }
   }  
   for ( size_t i = 0 ; i < sweepingGate.size() ; ++i ){
      if ( sweepingGate[i] ) {
        cout<<"Sweeping: AIG("<<i<<')'<<" removed..."<<endl;
        delete cirGate[i];
        cirGate[i] = 0;
      }
   }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
   ++global;
   vector<bool> optimizingGate ( cirGate.size() , false );
   for ( size_t i = 0 ; i <po_number ; ++i ){
      if ( cirGate[ piaig_number + i + 1 ] != 0 ) {
         if ( cirGate[ piaig_number + i + 1 ] -> simplify( cirGate[0] ) ) cirGate[i] = 0;
      }
   }
   _dfsList.clear();
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
