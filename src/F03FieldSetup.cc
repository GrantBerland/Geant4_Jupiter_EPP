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
/// \file field/field03/src/F03FieldSetup.cc
/// \brief Implementation of the F03FieldSetup class
//
//
// $Id: F03FieldSetup.cc 104351 2017-05-26 07:23:04Z gcosmo $
//
//
//   Field Setup class implementation.
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "F03FieldSetup.hh"
#include "F03FieldMessenger.hh"

#include "G4MagneticField.hh"
#include "G4UniformMagField.hh"
#include "PlanetaryMagneticField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4Transportation.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ChordFinder.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"

#include "G4ExplicitEuler.hh"
#include "G4ImplicitEuler.hh"
#include "G4SimpleRunge.hh"
#include "G4SimpleHeum.hh"
#include "G4ClassicalRK4.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4CashKarpRKF45.hh"
#include "G4RKG3_Stepper.hh"
#include "G4DormandPrince745.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

F03FieldSetup::F03FieldSetup()
 : fFieldManager(0),
   fChordFinder(0),
   fEquation(0),
   fMagneticField(0),
   fStepper(0),
   fFieldMessenger(0)
{
  fMagneticField      = new PlanetaryMagneticField(); 
  fFieldMessenger     = new F03FieldMessenger(this);
  fEquation           = new G4Mag_UsualEqRhs(fMagneticField);


  // Default values
  fMinStep     = 10.*cm ; 
  //fStepperType = 10;

  fFieldManager = GetGlobalFieldManager();

  UpdateField();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

F03FieldSetup::~F03FieldSetup()
{
  delete fMagneticField;
  delete fChordFinder;
  delete fStepper;
  delete fFieldMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void F03FieldSetup::UpdateField()
{
  // It must be possible to call 'again' - e.g. to choose an alternative stepper
  //   has been chosen, or in case other changes have been made.

  // 1. First clean up previous state.
  delete fChordFinder;
  fChordFinder= nullptr;


  G4cout<<"F03FieldSetup::UpdateField > The minimal step is equal to "
        << fMinStep/mm <<" mm"<<G4endl;
  G4cout<<"                            Stepper Type chosen = " << fStepperType
        << G4endl;

  // 2. Create the steppers ( Note: this also deletes the previous ones. )
  SetStepper();

  // 3. Create the chord finder(s)
  fChordFinder = new G4ChordFinder(fMagneticField, fMinStep, fStepper);

  fFieldManager->SetChordFinder(fChordFinder);

  // 4. Ensure that the field is updated (in Field manager & equation)
  fFieldManager->SetDetectorField(fMagneticField);
  
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void F03FieldSetup::SetStepper()
{
  delete fStepper;
  fStepper= nullptr;

  switch ( fStepperType )
  {
    case 0:
      fStepper = new G4ExplicitEuler( fEquation );
      G4cout<<"G4ExplicitEuler is called"<<G4endl;
      break;
    case 1:
      fStepper = new G4ImplicitEuler( fEquation );
      G4cout<<"G4ImplicitEuler is called"<<G4endl;
      break;
    case 2:
      fStepper = new G4SimpleRunge( fEquation );
      G4cout<<"G4SimpleRunge is called"<<G4endl;
      break;
    case 3:
      fStepper = new G4SimpleHeum( fEquation );
      G4cout<<"G4SimpleHeum is called"<<G4endl;
      break;
    case 4:
      fStepper = new G4ClassicalRK4( fEquation );
      G4cout<<"G4ClassicalRK4 (default) is called"<<G4endl;
      break;
    case 5:
      fStepper = new G4HelixExplicitEuler( fEquation );
      G4cout<<"G4HelixExplicitEuler is called"<<G4endl;
      break;
    case 6:
      fStepper = new G4HelixImplicitEuler( fEquation );
      G4cout<<"G4HelixImplicitEuler is called"<<G4endl;
      break;
    case 7:
      fStepper = new G4HelixSimpleRunge( fEquation );
      G4cout<<"G4HelixSimpleRunge is called"<<G4endl;
      break;
    case 8:
      fStepper = new G4CashKarpRKF45( fEquation );
      G4cout<<"G4CashKarpRKF45 is called"<<G4endl;
      break;
    case 9:
      fStepper = new G4RKG3_Stepper( fEquation );
      G4cout<<"G4RKG3_Stepper is called"<<G4endl;
      break;
    case 10:
      fStepper = new G4DormandPrince745( fEquation );
      G4cout<<"ode45 is called"<<G4endl;
      break;
    default: fStepper = 0;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void F03FieldSetup::SetFieldValue(G4double fieldStrength)
{
  G4ThreeVector fieldSetVec(0.0, 0.0, fieldStrength);
  SetFieldValue( fieldSetVec );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void F03FieldSetup::SetFieldValue(G4ThreeVector fieldVector)
{
  if(fMagneticField) delete fMagneticField;

  if(fieldVector != G4ThreeVector(0.,0.,0.))
  {
    //fMagneticField = new  G4UniformMagField(fieldVector);
    fMagneticField = new PlanetaryMagneticField();
  }
  else
  {
    // If the new field's value is Zero, then
    // setting the pointer to zero ensures
    // that it is not used for propagation.
    fMagneticField = 0;
  }
  

  // Either
  //   - UpdateField() to reset all (ChordFinder, Equation);
  // UpdateField();
  //     or simply update the field manager & equation of motion
  //     with pointer to new field
  GetGlobalFieldManager()->SetDetectorField(fMagneticField);
  fEquation->SetFieldObj( fMagneticField );

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4FieldManager* F03FieldSetup::GetGlobalFieldManager(){
  return G4TransportationManager::GetTransportationManager()
                                  ->GetFieldManager();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
