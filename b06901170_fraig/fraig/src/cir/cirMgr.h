/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions

class CirMgr
{

public:
   friend class CirGate;
   CirMgr(){}
   ~CirMgr() ;

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.

   CirGate* getGate(unsigned gid) const {
      if ( gid >= cirGate.size() )return 0;
      return cirGate[gid];
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   int  countAig()const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;
   void printFECPairs() const;
   void genProofModel(SatSolver& s);
   void reportResult(const SatSolver& solver, bool result);
   CirGate* ptrplus ( CirGate* a ){
      size_t tem = (size_t)a;
      ++tem;
      CirGate* b = (CirGate*)tem;
      return b;
   }

private:
   vector< vector< int  > > FecGrps;
   vector< vector< size_t > > used_pattern;
   vector<CirGate*>         cirGate; //dynamic array 
   vector<CirGate*>         piList; 
   vector< int >            _dfsList;
   size_t                   piaig_number;
   size_t                   po_number;
   ofstream                 *_simLog;
};


#endif // CIR_MGR_H
