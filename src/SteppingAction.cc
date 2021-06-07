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

// Initialize autolock for multiple threads writing into a single file
//namespace{ G4Mutex aMutex=G4MUTEX_INITIALIZER;} 

SteppingAction::SteppingAction(EventAction* eventAction, RunAction* RuAct)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fRunAction(RuAct),
  fEnergyThreshold_keV(0.),
  fPhotonWindowMin(250.),
  fPhotonWindowMax(275.), 
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
    
    case(2):
      G4cout << "Particle backscatter flux being recorded..." << G4endl;
      break;

    default:
      throw std::invalid_argument("No data being recorded, exiting...");
      break;
  }

}

SteppingAction::~SteppingAction()
{
  delete fSteppingMessenger;
}

namespace{G4Mutex aMutex=G4MUTEX_INITIALIZER;}

void SteppingAction::UserSteppingAction(const G4Step* step)
{

  G4Track* track = step->GetTrack();
  
  if( std::isnan(step->GetPostStepPoint()->GetKineticEnergy()) )
  {
    G4cout << "Particle killed at: " << step->GetPreStepPoint()->GetKineticEnergy()/keV << 
	    " keV , Process: " << step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() << G4endl;
    
    // kill it
    track->SetTrackStatus(fStopAndKill);
  }



  switch(fDataCollectionType)
  {
    
    case(0):  // Collects electron energy deposition per altitude
    { 
    	// Gets energy delta of particle over step length
    	const G4double energyDep = step->GetPreStepPoint()->GetKineticEnergy() - 
		step->GetPostStepPoint()->GetKineticEnergy();
    	
	if(energyDep > fEnergyThreshold_keV*keV)
    	{
	  // Gets altitude of particle
      	  G4ThreeVector position = track->GetPosition();
      	  G4double      zPos     = position.z();
      
          // Adds energy deposition to vector owned by RunAction, which is
          // written to a results file per simulation run
      	  G4int altitudeAddress = std::floor(500. + zPos/km);
      
	  // Thread lock this so only one thread can deposit energy into
	  // the histogram at a time. Unlocks when l goes out of scope.
	  if(altitudeAddress > 0 && altitudeAddress < 1000) 
	  {
	    LogEnergy(altitudeAddress, energyDep/keV);
	  }
        }
      break;
    }
    case(1):  // Collects particle (electron and photon) trajectory (warning, lots of data!)
    {
      G4String particleName = 
	  track->GetDynamicParticle()->GetDefinition()->GetParticleName();

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
    }
    case(2):
    {  
      G4String particleName = 
	  track->GetDynamicParticle()->GetDefinition()->GetParticleName();
      
      if(particleName == "e-" || particleName == "gamma")
      {
        
	G4ThreeVector momentumDirection = track->GetMomentumDirection();

	if(momentumDirection.z() > 0) // backscattered towards space
	{
	  // do tracking in some way
	}

      }
      
      break;
    }
    case(3): // collects electron energy histograms, photon trajectory
    {
      
      G4String particleName = 
	  track->GetDynamicParticle()->GetDefinition()->GetParticleName();
	  
      G4ThreeVector position = track->GetPosition();

      if(particleName == "e-")
      {
    	// Gets energy delta of particle over step length
    	const G4double energyDep = step->GetPreStepPoint()->GetKineticEnergy() - 
		step->GetPostStepPoint()->GetKineticEnergy();
    	
	if(energyDep > fEnergyThreshold_keV*keV)
    	{
      
          // Adds energy deposition to vector owned by RunAction, which is
          // written to a results file per simulation run
      	  G4int altitudeAddress = std::floor(500. + position.z()/km);
      
	  // Thread lock this so only one thread can deposit energy into
	  // the histogram at a time. Unlocks when l goes out of scope.
	  if(altitudeAddress > 0 && altitudeAddress < 1000) 
	  {
	    LogEnergy(altitudeAddress, energyDep/keV);
	  }
 
        }
      }
      
      // Records data if photon is in a box within [250, 275] km
      else if (particleName == "gamma" && 
	       position.z()/km > -500. + fPhotonWindowMin && 
	       position.z()/km < -500. + fPhotonWindowMax)
      {
        
          const G4double partEnergy = 
		step->GetPreStepPoint()->GetKineticEnergy();	
          
	  G4ThreeVector momentumDirection = track->GetMomentumDirection();
	  
	  G4double pos_array[7] = {position.x()/m, 
	      		           position.y()/m, 
			           position.z()/m,
				   momentumDirection.x(),
				   momentumDirection.y(),
				   momentumDirection.z(),
				   partEnergy/keV};

          // Writes 3D position vector to results file
	  // owned by RunAction

	  // Lock so threads don't overwrite into opened file
	  G4AutoLock lock(&aMutex);
          
	  fRunAction->fEnergyHist->WriteDirectlyToFile("photon_traj_Jupiter.txt", 
			                             pos_array,
				sizeof(pos_array)/sizeof(*pos_array));

	}
      break;
    }
    default: 
      throw std::runtime_error("Enter a valid data collection type!");
      break;
  
  }

}

void SteppingAction::LogEnergy(G4int histogramAddress, G4double energy)
{

  //G4AutoLock lock(&aMutex);

  fRunAction->fEnergyHist->AddCountToBin(histogramAddress, energy/keV);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
