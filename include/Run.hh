// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// --------------------------------------------------------------
//                 GEANT 4 - BrachySourceKerma
// --------------------------------------------------------------
//
// Code developed by:  Victor Gabriel Leandro Alves
// Copyright 2008-2017
//    *******************************
//    *                             *
//    *    Run.hh                *
//    *                             *
//    *******************************
#ifndef Run_HH
#define Run_HH

#include <map>
#include "G4Event.hh"
#include "G4Run.hh"
#include "G4THitsMap.hh"

class G4Event;

class Run : public G4Run {
public:
    // Constructor
    Run(const G4String& detectorName);

    // Destructor
    virtual ~Run();

public:
    // Override this method in G4Run
    virtual void RecordEvent(const G4Event*);

    // Dump all data
    void DumpData() const;

private:
    // Helper function
    void Print(const std::vector<G4String>& title,
               const std::map<G4int, std::vector<G4double> >& out) const;

    // Data member
    std::map<G4int, G4THitsMap<G4double>*> fMap;
};

#endif
