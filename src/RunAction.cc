//
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
// $Id: RunAction.cc 99560 2016-09-27 07:03:29Z gcosmo $
//
/// \file RunAction.cc
/// \brief Implementation of the RunAction class

#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
// #include "Run.hh"
// #include "DetectorAnalysis.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
// #include "HistoManager.hh"

#include <fstream>

RunAction::RunAction()
: G4UserRunAction()
{
  fPhotonEnergyVector = new std::vector<G4double>();
  fPhotonAltitudeVector = new std::vector<G4double>();
  fElectronEnergyVector = new std::vector<G4double>();
  fElectronAltitudeVector = new std::vector<G4double>();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{
  delete fPhotonEnergyVector;
  delete fPhotonAltitudeVector;
  delete fElectronEnergyVector;
  delete fElectronAltitudeVector;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run*)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RunAction::EndOfRunAction(const G4Run*)
{
  // Write photon energies to file 
  if(!fPhotonEnergyVector->empty())
  {
    // Photon data
    std::ofstream outputFile1("photon.txt", std::ios_base::app);
    for (unsigned int i=0;i<fPhotonEnergyVector->size(); i++) 
    {
      outputFile1 << (*fPhotonAltitudeVector)[i] << ","
	          << (*fPhotonEnergyVector)[i] << "\n";
    }
  }
  if(!fElectronEnergyVector->empty())
  {
    // Electron data
    std::ofstream outputFile2("electron.txt", std::ios_base::app);
    for (unsigned int i=0;i<fElectronEnergyVector->size(); i++) 
    {
      outputFile2 << (*fElectronAltitudeVector)[i] << ","
	          << (*fElectronEnergyVector)[i] << "\n";
    }
  }

}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
