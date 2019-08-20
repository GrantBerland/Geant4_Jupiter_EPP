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
// $Id: SteppingAction.cc 74483 2013-10-09 13:37:06Z gcosmo $
//
/// \file SteppingAction.cc
/// \brief Implementation of the SteppingAction class

#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"
// #include "DetectorAnalysis.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "RunAction.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
//#include "G4AutoLock.hh"


SteppingAction::SteppingAction(EventAction* eventAction, RunAction* RuAct)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fRunAction(RuAct),
  fEnergyThreshold_keV(0.)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{

  G4Track* track = step->GetTrack();
  G4String particleName = 
	  track->GetDynamicParticle()->GetDefinition()->GetParticleName();

  std::cout << track->GetPosition().z()/km+1020./2. << std::endl;
  if(particleName == "gamma") 
  { 
    // Gets energy delta of particle over step length
    const G4double energyDep = step->GetPreStepPoint()->GetKineticEnergy()
                           - step->GetPostStepPoint()->GetKineticEnergy();
    std::cout << energyDep/keV << " keV" << std::endl;
    if(energyDep > fEnergyThreshold_keV*keV)
    {
      // Gets altitude of particle
      G4ThreeVector position = track->GetPosition();
      G4double      alt      = position.z();
      
      // Adds photon energy to vector owned by RunAction, which is
      // written to a results file per simulation run
      fRunAction->AddPhotonEnergy(energyDep/keV);
      fRunAction->AddPhotonAltitude(alt/km);
    }
   
  }
  else if(particleName == "e-")
  {
    // Gets energy delta of particle over step length
    const G4double energyDep = step->GetPreStepPoint()->GetKineticEnergy()
                           - step->GetPostStepPoint()->GetKineticEnergy();
    std::cout << energyDep/keV << " keV" << std::endl;
    if(energyDep > fEnergyThreshold_keV*keV)
    {
      // Gets altitude of particle
      G4ThreeVector position = track->GetPosition();
      G4double      alt      = position.z();
  
      // Adds photon energy to vector owned by RunAction, which is
      // written to a results file per simulation run
      fRunAction->AddElectronEnergy(energyDep/keV);
      fRunAction->AddElectronAltitude(alt/km);
    }
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
