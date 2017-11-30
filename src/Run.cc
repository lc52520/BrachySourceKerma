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
//    *    Run.cc                  *
//    *                             *
//    *******************************
//    Saves an output.csv file

#include "Run.hh"
#include <assert.h>
#include "G4MultiFunctionalDetector.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VPrimitiveScorer.hh"

Run::Run(const G4String& detectorName) {
    // Get the sensitive detector manager
    G4SDManager* manager = G4SDManager::GetSDMpointer();

    // Get the sensitive detector
    G4MultiFunctionalDetector* detector =
        dynamic_cast<G4MultiFunctionalDetector*>(
            manager->FindSensitiveDetector(detectorName));

    // Expect the detector to exist
    assert(0 != detector);

    G4int i(0);

    // Loop over primitive scorers registered with the detector
    for (i = 0; i < detector->GetNumberOfPrimitives(); i++) {
        // Get scorer
        G4VPrimitiveScorer* scorer = detector->GetPrimitive(i);

        // Need to form the full collection name = detector name + "/"+ scorer name
        // to get the collection id number
        G4String fullCollectionName = detectorName + "/" + scorer->GetName();
        G4int id = manager->GetCollectionID(fullCollectionName);

        // Expect the collection to have been added to the sensitive detector
        // manager
        // when scorer was registered with G4MultiFunctionalDetector
        assert(id >= 0);

        fMap[id] = new G4THitsMap<G4double>(detectorName, scorer->GetName());
    }
}

Run::~Run() {
    // Cleanup
    std::map<G4int, G4THitsMap<G4double>*>::iterator iter = fMap.begin();

    while (iter != fMap.end()) {
        delete iter->second;
        iter++;
    }
}

void Run::RecordEvent(const G4Event* anEvent) {
    numberOfEvent++;

    // HandsOn3: Merging event data
    // Get the hits collection
    G4HCofThisEvent* eventHitCollection = anEvent->GetHCofThisEvent();

    if (!eventHitCollection) return;

    // Update our private fMap
    std::map<G4int, G4THitsMap<G4double>*>::iterator iter = fMap.begin();

    while (iter != fMap.end()) {
        G4int id = iter->first;

        // Get the hit collection corresponding to "id"
        G4THitsMap<G4double>* eventHitsMap =
            dynamic_cast<G4THitsMap<G4double>*>(eventHitCollection->GetHC(id));

        // Expect this to exist
        assert(0 != eventHitsMap);

        // Accumulate event data into our G4THitsMap<G4double> map
        *(iter->second) += *eventHitsMap;

        iter++;
    }
}

void Run::DumpData() const {
    // Titles
    std::vector<G4String> title;
    title.push_back("Theta");

    // Output map - energy binning on x axis, theta on y
    std::map<G4int, std::vector<G4double> > output;

    // Standard 10 bins in theta
    // TODO replace hardcoded number of bins by number of primitive scorers
    // TODO implement ROOT scoring classes
    G4int nThetaBins = 10;

    // Energy bins depends on the number of scorers
    G4int nEnergyBins = fMap.size();

    G4int i(0), j(0);

    // Initialise current to 0 in all bins
    for (i = 0; i < 10; i++) {
        for (j = 0; j < nEnergyBins; j++) {
            output[i].push_back(0);
        }
    }

    i = 0;

    // Fill the output map
    std::map<G4int, G4THitsMap<G4double>*>::const_iterator iter = fMap.begin();

    while (iter != fMap.end()) {
        G4THitsMap<G4double>* hitMap = iter->second;

        title.push_back(hitMap->GetName());

        std::map<G4int, G4double*>* myMap = hitMap->GetMap();

        for (j = 0; j < nThetaBins; j++) {
            G4double* current = (*myMap)[j];
            if (0 != current) output[j][i] = (*current);
        }

        i++;
        iter++;
    }

    Print(title, output);
}

void Run::Print(const std::vector<G4String>& title,
                const std::map<G4int, std::vector<G4double> >& myMap) const {
    // Print to G4cout and an output file
    std::ofstream outFile("output.csv");

    // Print title vector
    std::vector<G4String>::const_iterator titleIter = title.begin();

    while (titleIter != title.end()) {
        G4cout << std::setw(8) << *titleIter << " ";
        titleIter++;
    }

    G4cout << G4endl;

    // Print current data
    std::map<G4int, std::vector<G4double> >::const_iterator iter = myMap.begin();

    while (iter != myMap.end()) {
        G4cout << std::setw(8) << std::setprecision(3) << iter->first << " ";

        std::vector<G4double>::const_iterator energyBinIter = iter->second.begin();

        while (energyBinIter != iter->second.end()) {
            G4double value = *energyBinIter;
            G4cout << std::setw(8) << std::setprecision(3) << value * mm * mm << " ";
            outFile << value * mm * mm << ",";
            energyBinIter++;
        }
        outFile << G4endl;
        G4cout << G4endl;
        iter++;
    }
}
