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
// $Id: DetectorConstruction.cc 94307 2015-11-11 13:42:46Z gcosmo $
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
//#include "G4Cons.hh"
//#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"


#include <fstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction(),
fLogicWorld(0)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  //G4NistManager* nist = G4NistManager::Instance();


  // Material: Vacuum
  G4Material* vacuum_material = new G4Material("Vacuum",
              1.0 , 1.01*g/mole, 1.0E-25*g/cm3,
              kStateGas, 2.73*kelvin, 3.0E-18*pascal );

  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;

  // World
  G4double world_sizeXY = 500.0*km;
  G4double world_sizeZ  = 1020.*km;
  // G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

  G4Tubs* solidWorld = new G4Tubs("World",     //its name
       0.,  			// inner radius
       world_sizeXY, 	// outer radius
       0.5*world_sizeZ,         // z half length
       0.,			// starting phi
       360.*deg);		// segment angle

  fLogicWorld =
    new G4LogicalVolume(solidWorld,          //its solid
                        vacuum_material,           //its material
                        "World");            //its name

  G4VPhysicalVolume* physWorld =
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      fLogicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking


  // altitude [km], N2 [g/cm^3], O2 [g/cm^3], Mass density [g/cm^3]
  static const G4double msisAtmosTable[101][5] = {
{0.000000,8.732600e-04,2.641520e-04,1.143000e-03,3.014000e+02},
{10.000000,3.151820e-04,9.534700e-05,4.126000e-04,2.389000e+02},
{20.000000,7.139300e-05,2.159220e-05,9.344000e-05,2.045000e+02},
{30.000000,1.344670e-05,4.067750e-06,1.760000e-05,2.277000e+02},
{40.000000,2.984970e-06,9.031200e-07,3.908000e-06,2.563000e+02},
{50.000000,7.985300e-07,2.416270e-07,1.046000e-06,2.670000e+02},
{60.000000,2.370680e-07,7.170900e-08,3.104000e-07,2.500000e+02},
{70.000000,6.467200e-08,1.906940e-08,8.414000e-08,2.203000e+02},
{80.000000,1.484260e-08,4.231520e-09,1.917000e-08,1.961000e+02},
{90.000000,2.759370e-09,7.367000e-10,3.521000e-09,1.709000e+02},
{100.000000,3.904760e-10,8.983500e-11,4.955000e-10,1.742000e+02},
{110.000000,5.588300e-11,9.820900e-12,7.119000e-11,2.413000e+02},
{120.000000,1.179700e-11,1.535940e-12,1.533000e-11,4.061000e+02},
{130.000000,4.490850e-12,4.479030e-13,5.991000e-12,5.533000e+02},
{140.000000,2.256470e-12,1.803590e-13,3.111000e-12,6.527000e+02},
{150.000000,1.287800e-12,8.649600e-14,1.848000e-12,7.245000e+02},
{160.000000,7.910100e-13,4.637500e-14,1.189000e-12,7.766000e+02},
{170.000000,5.090100e-13,2.675970e-14,8.053000e-13,8.143000e+02},
{180.000000,3.381180e-13,1.621270e-14,5.653000e-13,8.417000e+02},
{190.000000,2.293130e-13,1.015480e-14,4.073000e-13,8.616000e+02},
{200.000000,1.578730e-13,6.508400e-15,2.993000e-13,8.761000e+02},
{210.000000,1.098860e-13,4.245300e-15,2.234000e-13,8.866000e+02},
{220.000000,7.708000e-14,2.802640e-15,1.689000e-13,8.943000e+02},
{230.000000,5.437900e-14,1.867720e-15,1.291000e-13,8.999000e+02},
{240.000000,3.855880e-14,1.253450e-15,9.960000e-14,9.039000e+02},
{250.000000,2.742920e-14,8.458800e-16,7.748000e-14,9.069000e+02},
{260.000000,1.957080e-14,5.729300e-16,6.071000e-14,9.091000e+02},
{270.000000,1.399660e-14,3.897620e-16,4.788000e-14,9.107000e+02},
{280.000000,1.002980e-14,2.658480e-16,3.799000e-14,9.119000e+02},
{290.000000,7.200400e-15,1.817370e-16,3.030000e-14,9.127000e+02},
{300.000000,5.174700e-15,1.244970e-16,2.429000e-14,9.134000e+02},
{310.000000,3.725690e-15,8.543600e-17,1.956000e-14,9.138000e+02},
{320.000000,2.685580e-15,5.872400e-17,1.581000e-14,9.142000e+02},
{330.000000,1.937810e-15,4.043900e-17,1.283000e-14,9.144000e+02},
{340.000000,1.400130e-15,2.788330e-17,1.045000e-14,9.146000e+02},
{350.000000,1.012380e-15,1.924960e-17,8.535000e-15,9.147000e+02},
{360.000000,7.332000e-16,1.330300e-17,6.990000e-15,9.148000e+02},
{370.000000,5.311000e-16,9.206100e-18,5.740000e-15,9.149000e+02},
{380.000000,3.854470e-16,6.381200e-18,4.725000e-15,9.150000e+02},
{390.000000,2.798850e-16,4.425500e-18,3.898000e-15,9.150000e+02},
{400.000000,2.034630e-16,3.072940e-18,3.222000e-15,9.150000e+02},
{410.000000,1.480030e-16,2.136430e-18,2.668000e-15,9.151000e+02},
{420.000000,1.078180e-16,1.487180e-18,2.214000e-15,9.151000e+02},
{430.000000,7.858400e-17,1.036150e-18,1.839000e-15,9.151000e+02},
{440.000000,5.734000e-17,7.229200e-19,1.531000e-15,9.151000e+02},
{450.000000,4.187700e-17,5.046660e-19,1.276000e-15,9.151000e+02},
{460.000000,3.061110e-17,3.527680e-19,1.065000e-15,9.151000e+02},
{470.000000,2.240020e-17,2.468210e-19,8.902000e-16,9.151000e+02},
{480.000000,1.640300e-17,1.728860e-19,7.450000e-16,9.151000e+02},
{490.000000,1.202260e-17,1.212640e-19,6.244000e-16,9.151000e+02},
{500.000000,8.821900e-18,8.511800e-20,5.241000e-16,9.151000e+02},
{510.000000,6.481300e-18,5.978400e-20,4.404000e-16,9.151000e+02},
{520.000000,4.761100e-18,4.206610e-20,3.706000e-16,9.151000e+02},
{530.000000,3.503380e-18,2.961640e-20,3.124000e-16,9.151000e+02},
{540.000000,2.579830e-18,2.087670e-20,2.636000e-16,9.151000e+02},
{550.000000,1.901150e-18,1.472870e-20,2.229000e-16,9.151000e+02},
{560.000000,1.402480e-18,1.040390e-20,1.887000e-16,9.151000e+02},
{570.000000,1.035410e-18,7.356400e-21,1.600000e-16,9.151000e+02},
{580.000000,7.651600e-19,5.205130e-21,1.360000e-16,9.151000e+02},
{590.000000,5.658800e-19,3.687210e-21,1.158000e-16,9.151000e+02},
{600.000000,4.189110e-19,2.614490e-21,9.874000e-17,9.151000e+02},
{610.000000,3.103410e-19,1.856060e-21,8.440000e-17,9.151000e+02},
{620.000000,2.301590e-19,1.318640e-21,7.232000e-17,9.151000e+02},
{630.000000,1.707980e-19,9.375700e-22,6.212000e-17,9.151000e+02},
{640.000000,1.268530e-19,6.678000e-22,5.349000e-17,9.151000e+02},
{650.000000,9.432900e-20,4.757280e-22,4.619000e-17,9.151000e+02},
{660.000000,7.017100e-20,3.393590e-22,4.001000e-17,9.151000e+02},
{670.000000,5.226400e-20,2.422630e-22,3.476000e-17,9.151000e+02},
{680.000000,3.894890e-20,1.731510e-22,3.029000e-17,9.151000e+02},
{690.000000,2.905540e-20,1.238610e-22,2.650000e-17,9.151000e+02},
{700.000000,2.169050e-20,8.866900e-23,2.326000e-17,9.151000e+02},
{710.000000,1.620560e-20,6.354700e-23,2.049000e-17,9.151000e+02},
{720.000000,1.212130e-20,4.560120e-23,1.812000e-17,9.151000e+02},
{730.000000,9.071000e-21,3.274340e-23,1.609000e-17,9.151000e+02},
{740.000000,6.791500e-21,2.353200e-23,1.435000e-17,9.151000e+02},
{750.000000,5.094800e-21,1.692820e-23,1.284000e-17,9.151000e+02},
{760.000000,3.820630e-21,1.219000e-23,1.154000e-17,9.151000e+02},
{770.000000,2.868880e-21,8.787400e-24,1.042000e-17,9.151000e+02},
{780.000000,2.155890e-21,6.338800e-24,9.443000e-18,9.151000e+02},
{790.000000,1.621500e-21,4.576550e-24,8.593000e-18,9.151000e+02},
{800.000000,1.220120e-21,3.307730e-24,7.850000e-18,9.151000e+02},
{810.000000,9.193200e-22,2.392420e-24,7.199000e-18,9.151000e+02},
{820.000000,6.927800e-22,1.732570e-24,6.627000e-18,9.151000e+02},
{830.000000,5.226400e-22,1.255570e-24,6.122000e-18,9.151000e+02},
{840.000000,3.947060e-22,9.105400e-25,5.674000e-18,9.151000e+02},
{850.000000,2.982620e-22,6.609100e-25,5.277000e-18,9.151000e+02},
{860.000000,2.255530e-22,4.803390e-25,4.922000e-18,9.151000e+02},
{870.000000,1.707040e-22,3.493230e-25,4.605000e-18,9.151000e+02},
{880.000000,1.292970e-22,2.542940e-25,4.319000e-18,9.151000e+02},
{890.000000,9.799500e-23,1.852880e-25,4.061000e-18,9.151000e+02},
{900.000000,7.435400e-23,1.350970e-25,3.828000e-18,9.151000e+02},
{910.000000,5.644700e-23,9.858000e-26,3.616000e-18,9.151000e+02},
{920.000000,4.287340e-23,7.202700e-26,3.422000e-18,9.151000e+02},
{930.000000,3.259450e-23,5.265550e-26,3.244000e-18,9.151000e+02},
{940.000000,2.480190e-23,3.853100e-26,3.081000e-18,9.151000e+02},
{950.000000,1.888460e-23,2.821720e-26,2.931000e-18,9.151000e+02},
{960.000000,1.439140e-23,2.068060e-26,2.792000e-18,9.151000e+02},
{970.000000,1.097450e-23,1.517390e-26,2.663000e-18,9.151000e+02},
{980.000000,8.375400e-24,1.114060e-26,2.542000e-18,9.151000e+02},
{990.000000,6.396700e-24,8.188500e-27,2.430000e-18,9.151000e+02},
{1000.000000,4.888000e-24,6.020800e-27,2.325000e-18,9.151000e+02}};
  
  const int tableSize = 101;
 
  // Material definitions
  G4Element* O = new G4Element("Oxygen",
		  	       "O",
		               8.,
			       16.0*g/mole);

  G4Element* N = new G4Element("Nitrogen",
		  	       "N",
		               7.,
			       14.0*g/mole);

  

  G4double layerLocation; 
  G4double layerThickness = 10.*km;
  
  G4Tubs* atmosphereLayer = new G4Tubs("AtmosphereLayer",
		       	       		0.,
		  			world_sizeXY-1.*cm,
		       	       		0.5*layerThickness,
					0.,
					360.*deg);
  G4Material*      layerMaterial;
  G4LogicalVolume* logicLayer;
  G4double         pressure;
  G4double         R_gas_constant_air = 287.;  // J/kg-K air
  for(int i=0; i<tableSize; i++)
  {
     
     // P [Pa] = R [J/kg-K air] * (rho*1000)[kg/m^3] * T [K]
     pressure = R_gas_constant_air * 
	        (msisAtmosTable[i][2] * 1000.) * 
	        msisAtmosTable[i][4];
     
     layerMaterial = new G4Material("AirLayer"+std::to_string(i),  // name
		     msisAtmosTable[i][3]*g/cm3, // density
		     2,                          // number of components
		     kStateGas,                  // state
		     msisAtmosTable[i][4]*kelvin,// temperature
		     pressure*pascal); 	 // pressure
    
     layerMaterial->AddElement(N, // material, mass fraction
		     2);//msisAtmosTable[i][1]/msisAtmosTable[i][3]);

     layerMaterial->AddElement(O, // material, mass fraction
		     2);//msisAtmosTable[i][2]/msisAtmosTable[i][3]); 
     
     logicLayer = new G4LogicalVolume(atmosphereLayer,
		                      layerMaterial,
				      "AtmosphereLayer");


     
     layerLocation = (i - tableSize/2) * layerThickness + layerThickness/2.;
     new G4PVPlacement(0,
		G4ThreeVector(0.,0.,layerLocation),
		       logicLayer,
		       "AtmosphereLayer",
		       fLogicWorld,
		       false,
		       i,
		       checkOverlaps);

  }

  // always return the physical World
  return physWorld;
}

void DetectorConstruction::ConstructSDandFields()
{

  G4MagneticField* magField= new G4UniformMagField( 
		  G4ThreeVector(0.0, 0.0, 1.0e-5*tesla) ); 

  G4FieldManager* globalFieldMgr = 
  G4TransportationManager::GetTransportationManager()->GetFieldManager();

  fLogicWorld->SetFieldManager(globalFieldMgr, true);

  globalFieldMgr->SetDetectorField(magField);
  globalFieldMgr->CreateChordFinder(magField);

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
