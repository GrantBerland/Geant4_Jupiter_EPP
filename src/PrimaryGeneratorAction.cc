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
// $Id: PrimaryGeneratorAction.cc 94307 2015-11-11 13:42:46Z gcosmo $
//
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
//#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "Randomize.hh"
#include <math.h>

#include <fstream>


PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0)
{
  fParticleGun  = new G4ParticleGun();
  G4ParticleDefinition* electronParticle = G4ParticleTable::GetParticleTable()->FindParticle("e-");

  // Selects electron for particle type
  fParticleGun->SetParticleDefinition(electronParticle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GenerateParticles(ParticleSample* r)
{
  G4double E0 = 300*keV;
  
  G4double theta = G4UniformRand() * 2. * 3.141592; // u ~ Unif[0, 2 pi)
  G4double radialPosition = G4UniformRand();  // [0, 1)
  G4double diskRadius = 400.*km;

  r->xPos = diskRadius * std::sqrt(radialPosition) * std::cos(theta);
  r->yPos = diskRadius * std::sqrt(radialPosition) * std::sin(theta);
  r->zPos = 500.*km;

  r->xDir = 0;
  r->yDir = 0;
  r->zDir = -1;

  r->energy = -E0 * std::log(1 - G4UniformRand());
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

  ParticleSample* r = new ParticleSample();
  GenerateParticles(r);

  fParticleGun->SetParticlePosition(
		  G4ThreeVector(r->xPos, r->yPos, r->zPos));
  
  fParticleGun->SetParticleMomentumDirection(
		  G4ThreeVector(r->xDir, r->yDir, r->zDir));
  
  fParticleGun->SetParticleEnergy(r->energy);

  fParticleGun->GeneratePrimaryVertex(anEvent);

  delete r;
}


