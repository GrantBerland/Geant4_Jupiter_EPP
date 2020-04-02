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
#include "SteppingActionMessenger.hh"
#include "G4AutoLock.hh"

SteppingAction::SteppingAction(EventAction* eventAction, RunAction* RuAct)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fRunAction(RuAct),
  fEnergyThreshold_keV(0.),
  fDataCollectionType(0),
  fSteppingMessenger()
{

  fSteppingMessenger = new SteppingActionMessenger(this);

  switch(fDataCollectionType)
  {
    
    case(0):
      G4cout << "Energy deposition being recorded...";
      fRunAction->fEnergyHist->InitializeHistogram();
      G4cout << "Histogram initialized!" << G4endl;
      break;
    
    case(1):
      G4cout << "Particle trajectory being recorded..." << G4endl;
      break;
    
    default:
      throw std::invalid_argument("No data being recorded, exiting...");
      break;
  }

}

SteppingAction::~SteppingAction()
{}

namespace{G4Mutex aMutex=G4MUTEX_INITIALIZER;}

void SteppingAction::UserSteppingAction(const G4Step* step)
{

  G4AutoLock l(&aMutex);

  G4Track* track = step->GetTrack();
  G4String particleName = 
	  track->GetDynamicParticle()->GetDefinition()->GetParticleName();
  
  switch(fDataCollectionType)
  {
    
    case(0):  // Collects energy deposition per altitude
  
      if(particleName == "e-")
      {

    	// Gets energy delta of particle over step length
    	const G4double energyDep = step->GetPreStepPoint()->GetKineticEnergy() - 
		step->GetPostStepPoint()->GetKineticEnergy();
    	
	if(energyDep > fEnergyThreshold_keV*keV)
    	{
	  // Gets altitude of particle
      	  G4ThreeVector position = track->GetPosition();
      	  G4double      alt      = position.z();
      
          // Adds energy deposition to vector owned by RunAction, which is
          // written to a results file per simulation run
      	  G4int altitudeAddress = std::floor(1020./2. + alt/km);
      
	  // Thread lock this so only one thread can deposit energy into
	  // the histogram at a time. Unlocks when l goes out of scope.
	  l.lock();
	  if(altitudeAddress > 0 && altitudeAddress < 1000) 
	  {
	  
	  fRunAction->fEnergyHist->AddCountToBin(altitudeAddress, 
		      			    energyDep/keV);
	  }
        }
    }
      break;
    
    case(1):  // Collects particle trajectory (warning, lots of data!)
      if(particleName == "e-")
      {

        const G4double partEnergy = 
		step->GetPreStepPoint()->GetKineticEnergy();	
        G4ThreeVector position = track->GetPosition();
        G4double pos_array[4] = { position.x()/m, 
	      		          position.y()/m, 
			          position.z()/m,
				  partEnergy/keV};
        // Writes 3D position vector to results file
	// owned by RunAction
        fRunAction->fEnergyHist->WriteDirectlyToFile("part_traj.txt", 
			                             pos_array,
				sizeof(pos_array)/sizeof(*pos_array));
      
      }
      if(particleName == "gamma")
      {

        const G4double partEnergy = 
		step->GetPreStepPoint()->GetKineticEnergy();	
        G4ThreeVector position = track->GetPosition();
        G4double pos_array[4] = { position.x()/m, 
	      		          position.y()/m, 
			          position.z()/m,
				  partEnergy/keV};
        // Writes 3D position vector to results file
	// owned by RunAction
        fRunAction->fEnergyHist->WriteDirectlyToFile("photon_part_traj.txt", 
			                             pos_array,
				sizeof(pos_array)/sizeof(*pos_array));
      
      }
      break;
    
    default: 
      throw std::runtime_error("Enter a valid data collection type!");
      break;
  
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
