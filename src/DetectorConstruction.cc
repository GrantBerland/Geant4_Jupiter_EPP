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

#include "F03FieldSetup.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"

#include "G4AutoDelete.hh"
#include "G4SDManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction(),
fLogicWorld(0),
tableSize(101)
{

}

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
  G4Material* low_density_material = new G4Material("Low_dens",
              1.0 , 1.01*g/mole, 1.0E-10*g/cm3,
              kStateGas, 2.73*kelvin, 3.0E-12*pascal );

  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;

  // World
  G4double world_sizeXY = 1000.0*km;
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


  // alt [km], O, N2, O2, Mass den [g/cm^3], Temp [K], He, Ar, H, N
  // (all species' density in [g/cm^3])
  static const G4double msisAtmosTable[101][10] = {
{0.000000,0.000000e+00,8.732600e-04,2.641520e-04,1.143000e-03,3.014000e+02,8.230200e-10,1.466520e-05,0.000000e+00,0.000000e+00},
{10.000000,0.000000e+00,3.151820e-04,9.534700e-05,4.126000e-04,2.389000e+02,2.970000e-10,5.293860e-06,0.000000e+00,0.000000e+00},
{20.000000,0.000000e+00,7.139300e-05,2.159220e-05,9.344000e-05,2.045000e+02,6.725400e-11,1.199220e-06,0.000000e+00,0.000000e+00},
{30.000000,0.000000e+00,1.344670e-05,4.067750e-06,1.760000e-05,2.277000e+02,1.267200e-11,2.258520e-07,0.000000e+00,0.000000e+00},
{40.000000,0.000000e+00,2.984970e-06,9.031200e-07,3.908000e-06,2.563000e+02,2.812920e-12,5.014020e-08,0.000000e+00,0.000000e+00},
{50.000000,0.000000e+00,7.985300e-07,2.416270e-07,1.046000e-06,2.670000e+02,7.524000e-13,1.341780e-08,0.000000e+00,0.000000e+00},
{60.000000,0.000000e+00,2.370680e-07,7.170900e-08,3.104000e-07,2.500000e+02,2.234100e-13,3.982440e-09,0.000000e+00,0.000000e+00},
{70.000000,0.000000e+00,6.467200e-08,1.906940e-08,8.414000e-08,2.203000e+02,6.132060e-14,1.084380e-09,0.000000e+00,0.000000e+00},
{80.000000,1.135420e-13,1.484260e-08,4.231520e-09,1.917000e-08,1.961000e+02,1.435500e-14,2.478960e-10,4.537390e-17,1.753750e-18},
{90.000000,8.728200e-12,2.759370e-09,7.367000e-10,3.521000e-09,1.709000e+02,2.962740e-15,4.497900e-11,1.136101e-16,3.567300e-18},
{100.000000,1.343160e-11,3.904760e-10,8.983500e-11,4.955000e-10,1.742000e+02,6.930000e-16,5.560500e-12,3.097850e-17,5.809800e-18},
{110.000000,5.447000e-12,5.588300e-11,9.820900e-12,7.119000e-11,2.413000e+02,2.987820e-16,5.370420e-13,1.267363e-17,1.021660e-17},
{120.000000,2.011880e-12,1.179700e-11,1.535940e-12,1.533000e-11,4.061000e+02,1.799820e-16,7.075200e-14,5.434180e-18,2.078740e-17},
{130.000000,1.058460e-12,4.490850e-12,4.479030e-13,5.991000e-12,5.533000e+02,1.360260e-16,1.890900e-14,2.707070e-18,5.050800e-17},
{140.000000,6.767800e-13,2.256470e-12,1.803590e-13,3.111000e-12,6.527000e+02,1.139160e-16,7.286400e-15,1.513688e-18,1.129760e-16},
{150.000000,4.739800e-13,1.287800e-12,8.649600e-14,1.848000e-12,7.245000e+02,9.979200e-17,3.350160e-15,9.323610e-19,2.091620e-16},
{160.000000,3.504800e-13,7.910100e-13,4.637500e-14,1.189000e-12,7.766000e+02,8.989200e-17,1.702140e-15,6.351010e-19,3.199300e-16},
{170.000000,2.678000e-13,5.090100e-13,2.675970e-14,8.053000e-13,8.143000e+02,8.243400e-17,9.193800e-16,4.754490e-19,4.151500e-16},
{180.000000,2.090920e-13,3.381180e-13,1.621270e-14,5.653000e-13,8.417000e+02,7.642800e-17,5.172420e-16,3.854360e-19,4.728800e-16},
{190.000000,1.658540e-13,2.293130e-13,1.015480e-14,4.073000e-13,8.616000e+02,7.147800e-17,2.990460e-16,3.321630e-19,4.882900e-16},
{200.000000,1.330160e-13,1.578730e-13,6.508400e-15,2.993000e-13,8.761000e+02,6.718800e-17,1.762860e-16,2.990970e-19,4.696600e-16},
{210.000000,1.075360e-13,1.098860e-13,4.245300e-15,2.234000e-13,8.866000e+02,6.338640e-17,1.054020e-16,2.778880e-19,4.305600e-16},
{220.000000,8.746400e-14,7.708000e-14,2.802640e-15,1.689000e-13,8.943000e+02,6.000720e-17,6.365040e-17,2.635260e-19,3.815700e-16},
{230.000000,7.147400e-14,5.437900e-14,1.867720e-15,1.291000e-13,8.999000e+02,5.692500e-17,3.875520e-17,2.535060e-19,3.312000e-16},
{240.000000,5.857800e-14,3.855880e-14,1.253450e-15,9.960000e-14,9.039000e+02,5.408040e-17,2.373360e-17,2.461580e-19,2.835900e-16},
{250.000000,4.815200e-14,2.742920e-14,8.458800e-16,7.748000e-14,9.069000e+02,5.142720e-17,1.461240e-17,2.404800e-19,2.408100e-16},
{260.000000,3.965000e-14,1.957080e-14,5.729300e-16,6.071000e-14,9.091000e+02,4.895220e-17,9.022200e-18,2.358040e-19,2.037110e-16},
{270.000000,3.270800e-14,1.399660e-14,3.897620e-16,4.788000e-14,9.107000e+02,4.662240e-17,5.593500e-18,2.317960e-19,1.718560e-16},
{280.000000,2.704000e-14,1.002980e-14,2.658480e-16,3.799000e-14,9.119000e+02,4.443120e-17,3.475560e-18,2.282890e-19,1.449000e-16},
{290.000000,2.235220e-14,7.200400e-15,1.817370e-16,3.030000e-14,9.127000e+02,4.235220e-17,2.164800e-18,2.251160e-19,1.221760e-16},
{300.000000,1.850420e-14,5.174700e-15,1.244970e-16,2.429000e-14,9.134000e+02,4.039200e-17,1.351020e-18,2.222770e-19,1.030860e-16},
{310.000000,1.533220e-14,3.725690e-15,8.543600e-17,1.956000e-14,9.138000e+02,3.853080e-17,8.448000e-19,2.194380e-19,8.705500e-17},
{320.000000,1.271400e-14,2.685580e-15,5.872400e-17,1.581000e-14,9.142000e+02,3.676200e-17,5.294520e-19,2.167660e-19,7.360000e-17},
{330.000000,1.055080e-14,1.937810e-15,4.043900e-17,1.283000e-14,9.144000e+02,3.508560e-17,3.322440e-19,2.140940e-19,6.228400e-17},
{340.000000,8.762000e-15,1.400130e-15,2.788330e-17,1.045000e-14,9.146000e+02,3.348840e-17,2.088240e-19,2.115890e-19,5.278500e-17},
{350.000000,7.280000e-15,1.012380e-15,1.924960e-17,8.535000e-15,9.147000e+02,3.197040e-17,1.314720e-19,2.090840e-19,4.475800e-17},
{360.000000,6.052800e-15,7.332000e-16,1.330300e-17,6.990000e-15,9.148000e+02,3.052500e-17,8.289600e-20,2.067460e-19,3.799600e-17},
{370.000000,5.036200e-15,5.311000e-16,9.206100e-18,5.740000e-15,9.149000e+02,2.915220e-17,5.231820e-20,2.042410e-19,3.229200e-17},
{380.000000,4.191200e-15,3.854470e-16,6.381200e-18,4.725000e-15,9.150000e+02,2.784540e-17,3.307260e-20,2.019030e-19,2.746200e-17},
{390.000000,3.491800e-15,2.798850e-16,4.425500e-18,3.898000e-15,9.150000e+02,2.660460e-17,2.094180e-20,1.997320e-19,2.336800e-17},
{400.000000,2.909400e-15,2.034630e-16,3.072940e-18,3.222000e-15,9.150000e+02,2.541660e-17,1.327920e-20,1.973940e-19,1.990420e-17},
{410.000000,2.425800e-15,1.480030e-16,2.136430e-18,2.668000e-15,9.151000e+02,2.428800e-17,8.428200e-21,1.952230e-19,1.696480e-17},
{420.000000,2.023580e-15,1.078180e-16,1.487180e-18,2.214000e-15,9.151000e+02,2.321220e-17,5.358540e-21,1.930520e-19,1.446700e-17},
{430.000000,1.689220e-15,7.858400e-17,1.036150e-18,1.839000e-15,9.151000e+02,2.218920e-17,3.411540e-21,1.908810e-19,1.234410e-17},
{440.000000,1.410760e-15,5.734000e-17,7.229200e-19,1.531000e-15,9.151000e+02,2.121240e-17,2.174700e-21,1.887100e-19,1.054090e-17},
{450.000000,1.178840e-15,4.187700e-17,5.046660e-19,1.276000e-15,9.151000e+02,2.027520e-17,1.387980e-21,1.865390e-19,9.004500e-18},
{460.000000,9.856600e-16,3.061110e-17,3.527680e-19,1.065000e-15,9.151000e+02,1.939080e-17,8.870400e-22,1.845350e-19,7.688900e-18},
{470.000000,8.244600e-16,2.240020e-17,2.468210e-19,8.902000e-16,9.151000e+02,1.854600e-17,5.677980e-22,1.823640e-19,6.578000e-18},
{480.000000,6.900400e-16,1.640300e-17,1.728860e-19,7.450000e-16,9.151000e+02,1.773420e-17,3.638580e-22,1.803600e-19,5.628100e-18},
{490.000000,5.779800e-16,1.202260e-17,1.212640e-19,6.244000e-16,9.151000e+02,1.696860e-17,2.335080e-22,1.783560e-19,4.818500e-18},
{500.000000,4.841200e-16,8.821900e-18,8.511800e-20,5.241000e-16,9.151000e+02,1.623600e-17,1.500180e-22,1.765190e-19,4.128500e-18},
{510.000000,4.058600e-16,6.481300e-18,5.978400e-20,4.404000e-16,9.151000e+02,1.552980e-17,9.649200e-23,1.745150e-19,3.537400e-18},
{520.000000,3.403400e-16,4.761100e-18,4.206610e-20,3.706000e-16,9.151000e+02,1.486320e-17,6.217200e-23,1.726780e-19,3.033700e-18},
{530.000000,2.857400e-16,3.503380e-18,2.961640e-20,3.124000e-16,9.151000e+02,1.422960e-17,4.010160e-23,1.706740e-19,2.601300e-18},
{540.000000,2.397980e-16,2.579830e-18,2.087670e-20,2.636000e-16,9.151000e+02,1.361580e-17,2.589840e-23,1.688370e-19,2.232150e-18},
{550.000000,2.014220e-16,1.901150e-18,1.472870e-20,2.229000e-16,9.151000e+02,1.303500e-17,1.675080e-23,1.670000e-19,1.916130e-18},
{560.000000,1.692600e-16,1.402480e-18,1.040390e-20,1.887000e-16,9.151000e+02,1.248060e-17,1.084380e-23,1.652465e-19,1.645650e-18},
{570.000000,1.423240e-16,1.035410e-18,7.356400e-21,1.600000e-16,9.151000e+02,1.195260e-17,7.029000e-24,1.634763e-19,1.414040e-18},
{580.000000,1.197300e-16,7.651600e-19,5.205130e-21,1.360000e-16,9.151000e+02,1.144440e-17,4.562580e-24,1.617228e-19,1.215550e-18},
{590.000000,1.007760e-16,5.658800e-19,3.687210e-21,1.158000e-16,9.151000e+02,1.096260e-17,2.965380e-24,1.599860e-19,1.045350e-18},
{600.000000,8.486400e-17,4.189110e-19,2.614490e-21,9.874000e-17,9.151000e+02,1.050060e-17,1.929840e-24,1.582659e-19,8.995300e-19},
{610.000000,7.150000e-17,3.103410e-19,1.856060e-21,8.440000e-17,9.151000e+02,1.006500e-17,1.257300e-24,1.565792e-19,7.741800e-19},
{620.000000,6.026800e-17,2.301590e-19,1.318640e-21,7.232000e-17,9.151000e+02,9.642600e-18,8.203800e-25,1.549259e-19,6.667700e-19},
{630.000000,5.083000e-17,1.707980e-19,9.375700e-22,6.212000e-17,9.151000e+02,9.240000e-18,5.355900e-25,1.532893e-19,5.743100e-19},
{640.000000,4.287400e-17,1.268530e-19,6.678000e-22,5.349000e-17,9.151000e+02,8.857200e-18,3.502620e-25,1.516694e-19,4.949600e-19},
{650.000000,3.619200e-17,9.432900e-20,4.757280e-22,4.619000e-17,9.151000e+02,8.487600e-18,2.292840e-25,1.500662e-19,4.268800e-19},
{660.000000,3.057600e-17,7.017100e-20,3.393590e-22,4.001000e-17,9.151000e+02,8.137800e-18,1.503480e-25,1.484964e-19,3.682300e-19},
{670.000000,2.583100e-17,5.226400e-20,2.422630e-22,3.476000e-17,9.151000e+02,7.801200e-18,9.867000e-26,1.469266e-19,3.176300e-19},
{680.000000,2.183740e-17,3.894890e-20,1.731510e-22,3.029000e-17,9.151000e+02,7.477800e-18,6.482520e-26,1.454069e-19,2.741600e-19},
{690.000000,1.847040e-17,2.905540e-20,1.238610e-22,2.650000e-17,9.151000e+02,7.174200e-18,4.264920e-26,1.438872e-19,2.369000e-19},
{700.000000,1.562860e-17,2.169050e-20,8.866900e-23,2.326000e-17,9.151000e+02,6.883800e-18,2.808960e-26,1.423842e-19,2.046770e-19},
{710.000000,1.323140e-17,1.620560e-20,6.354700e-23,2.049000e-17,9.151000e+02,6.600000e-18,1.852620e-26,1.409146e-19,1.769160e-19},
{720.000000,1.120600e-17,1.212130e-20,4.560120e-23,1.812000e-17,9.151000e+02,6.332040e-18,1.222980e-26,1.394617e-19,1.529960e-19},
{730.000000,9.495200e-18,9.071000e-21,3.274340e-23,1.609000e-17,9.151000e+02,6.075300e-18,8.085000e-27,1.380255e-19,1.323420e-19},
{740.000000,8.049600e-18,6.791500e-21,2.353200e-23,1.435000e-17,9.151000e+02,5.829120e-18,5.349300e-27,1.366060e-19,1.145400e-19},
{750.000000,6.827600e-18,5.094800e-21,1.692820e-23,1.284000e-17,9.151000e+02,5.594160e-18,3.544200e-27,1.352032e-19,9.917600e-20},
{760.000000,5.795400e-18,3.820630e-21,1.219000e-23,1.154000e-17,9.151000e+02,5.369100e-18,2.350920e-27,1.338338e-19,8.590500e-20},
{770.000000,4.919200e-18,2.868880e-21,8.787400e-24,1.042000e-17,9.151000e+02,5.153940e-18,1.560900e-27,1.324644e-19,7.442800e-20},
{780.000000,4.178200e-18,2.155890e-21,6.338800e-24,9.443000e-18,9.151000e+02,4.948020e-18,1.038180e-27,1.311284e-19,6.451500e-20},
{790.000000,3.551600e-18,1.621500e-21,4.576550e-24,8.593000e-18,9.151000e+02,4.750680e-18,6.910200e-28,1.297924e-19,5.595900e-20},
{800.000000,3.018600e-18,1.220120e-21,3.307730e-24,7.850000e-18,9.151000e+02,4.561260e-18,4.603500e-28,1.284898e-19,4.855300e-20},
{810.000000,2.566980e-18,9.193200e-22,2.392420e-24,7.199000e-18,9.151000e+02,4.380420e-18,3.070980e-28,1.271872e-19,4.213600e-20},
{820.000000,2.184260e-18,6.927800e-22,1.732570e-24,6.627000e-18,9.151000e+02,4.206840e-18,2.051280e-28,1.259180e-19,3.657000e-20},
{830.000000,1.859520e-18,5.226400e-22,1.255570e-24,6.122000e-18,9.151000e+02,4.041180e-18,1.371480e-28,1.246488e-19,3.176300e-20},
{840.000000,1.583660e-18,3.947060e-22,9.105400e-25,5.674000e-18,9.151000e+02,3.882120e-18,9.180600e-29,1.234130e-19,2.760000e-20},
{850.000000,1.349400e-18,2.982620e-22,6.609100e-25,5.277000e-18,9.151000e+02,3.729660e-18,6.151860e-29,1.221772e-19,2.398900e-20},
{860.000000,1.150240e-18,2.255530e-22,4.803390e-25,4.922000e-18,9.151000e+02,3.583800e-18,4.126980e-29,1.209581e-19,2.087020e-20},
{870.000000,9.809800e-19,1.707040e-22,3.493230e-25,4.605000e-18,9.151000e+02,3.443880e-18,2.772000e-29,1.197724e-19,1.815620e-20},
{880.000000,8.369400e-19,1.292970e-22,2.542940e-25,4.319000e-18,9.151000e+02,3.309900e-18,1.863840e-29,1.185867e-19,1.580100e-20},
{890.000000,7.142200e-19,9.799500e-23,1.852880e-25,4.061000e-18,9.151000e+02,3.181200e-18,1.254660e-29,1.174177e-19,1.375630e-20},
{900.000000,6.099600e-19,7.435400e-23,1.350970e-25,3.828000e-18,9.151000e+02,3.058440e-18,8.454600e-30,1.162654e-19,1.198070e-20},
{910.000000,5.210400e-19,5.644700e-23,9.858000e-26,3.616000e-18,9.151000e+02,2.940300e-18,5.702400e-30,1.151298e-19,1.043970e-20},
{920.000000,4.453800e-19,4.287340e-23,7.202700e-26,3.422000e-18,9.151000e+02,2.827440e-18,3.851100e-30,1.139942e-19,9.098800e-21},
{930.000000,3.809000e-19,3.259450e-23,5.265550e-26,3.244000e-18,9.151000e+02,2.718540e-18,2.603040e-30,1.128920e-19,7.935000e-21},
{940.000000,3.257800e-19,2.480190e-23,3.853100e-26,3.081000e-18,9.151000e+02,2.614260e-18,1.761540e-30,1.117898e-19,6.920700e-21},
{950.000000,2.787200e-19,1.888460e-23,2.821720e-26,2.931000e-18,9.151000e+02,2.514600e-18,1.193280e-30,1.107043e-19,6.039800e-21},
{960.000000,2.386800e-19,1.439140e-23,2.068060e-26,2.792000e-18,9.151000e+02,2.418900e-18,8.098200e-31,1.096355e-19,5.271600e-21},
{970.000000,2.044120e-19,1.097450e-23,1.517390e-26,2.663000e-18,9.151000e+02,2.327160e-18,5.495820e-31,1.085834e-19,4.602300e-21},
{980.000000,1.751620e-19,8.375400e-24,1.114060e-26,2.542000e-18,9.151000e+02,2.238720e-18,3.735600e-31,1.075480e-19,4.020400e-21},
{990.000000,1.501500e-19,6.396700e-24,8.188500e-27,2.430000e-18,9.151000e+02,2.154240e-18,2.541660e-31,1.065126e-19,3.514400e-21},
{1000.000000,1.287780e-19,4.888000e-24,6.020800e-27,2.325000e-18,9.151000e+02,2.073060e-18,1.731180e-31,1.054939e-19,3.072800e-21}}; 


  // Atmospheric material definitions
  G4Element* O = new G4Element("Oxygen",
		  	       "O",
		               8.,
			       16.0*g/mole);
  
  G4Element* N = new G4Element("Nitrogen",
		  	       "N",
		               7.,
			       14.0*g/mole);

  G4Element* Ar = new G4Element("Argon",
		  	       "Ar",
		               18.,
			       39.948*g/mole);
  
  G4Element* He = new G4Element("Helium",
		  	       "He",
		               2.,
			       4.0*g/mole);
  
  G4Element* H = new G4Element("Hydrogen",
		  	       "H",
		               1.,
			       1.0078*g/mole);
  

  G4double layerLocation; 
  G4double layerThickness = 10.*km;
  
  G4Tubs* atmosphereLayer = new G4Tubs("AtmosphereLayer",
		       	       		0.,
		  			world_sizeXY-1.*cm,
		       	       		0.5*layerThickness,
					0.,
					360.*deg);
  G4Material*      layerMaterial;
  G4Material*      diOxygen;
  G4Material*      diNitrogen;
  G4LogicalVolume* logicLayer;
  G4double         pressure;
  G4double         R_gas_constant_air = 287.;  // J/kg-K air
  G4double         totalAtmosMass;
  G4double         zeroThreshold = 1e-21;
  G4int            nComponents;
  for(int i=0; i<tableSize; i++)
  {
     // Ideal gas law for atmospheric pressure
     // P [Pa] = R [J/kg-K air] * (rho*1000)[kg/m^3] * T [K]
     pressure = R_gas_constant_air * 
	        (msisAtmosTable[i][2] * 1000.) * 
	         msisAtmosTable[i][4];
    

     nComponents    = 0;
     totalAtmosMass = 0;
     if(msisAtmosTable[i][1] > zeroThreshold){ 
	     nComponents++;
      	     totalAtmosMass += msisAtmosTable[i][1];}
     if(msisAtmosTable[i][2] > zeroThreshold){ 
	     nComponents++;
      	     totalAtmosMass += msisAtmosTable[i][2];
        diNitrogen = new G4Material("Dinitrogen-Layer"+std::to_string(i),
		                    msisAtmosTable[i][2]*g/cm3,
				    1,
				    kStateGas,
				    msisAtmosTable[i][4]*kelvin,
				    pressure*pascal);
             diNitrogen->AddElement(N, 2);}
     else{ diNitrogen = nullptr; }
     if(msisAtmosTable[i][3] > zeroThreshold){ 
	     nComponents++;
      	     totalAtmosMass += msisAtmosTable[i][3];
             diOxygen = new G4Material("Dioxgyen-Layer"+std::to_string(i),
		                    msisAtmosTable[i][3]*g/cm3,
				    1,
				    kStateGas,
				    msisAtmosTable[i][4]*kelvin,
				    pressure*pascal);
             diOxygen->AddElement(O, 2);}
     
     if(msisAtmosTable[i][6] > zeroThreshold){ 
	     nComponents++;
      	     totalAtmosMass += msisAtmosTable[i][6];}
     if(msisAtmosTable[i][7] > zeroThreshold){ 
	     nComponents++;
      	     totalAtmosMass += msisAtmosTable[i][7];}
     if(msisAtmosTable[i][8] > zeroThreshold){ 
	     nComponents++;
      	     totalAtmosMass += msisAtmosTable[i][8];}
     if(msisAtmosTable[i][9] > zeroThreshold){ 
	     nComponents++;
      	     totalAtmosMass += msisAtmosTable[i][9];}
     

     // test material
     //layerMaterial = low_density_material;
     
     layerMaterial = new G4Material("AirLayer"+std::to_string(i),  // name
		     totalAtmosMass*g/cm3, // density
		     nComponents,                // number of components
		     kStateGas,                  // state
		     msisAtmosTable[i][4]*kelvin,// temperature
		     pressure*pascal); 	         // pressure
    
     // alt [km], O, N2, O2, Mass den [g/cm^3], Temp [K], He, Ar, H, N
    
     // O
     if(msisAtmosTable[i][1] > zeroThreshold) 
     {
	layerMaterial->AddElement(O, 
		       msisAtmosTable[i][1]/totalAtmosMass);
     }
     // N2
     if(msisAtmosTable[i][2] > zeroThreshold) 
     {
     layerMaterial->AddMaterial(diNitrogen, // material, mass fraction
		     msisAtmosTable[i][2]/totalAtmosMass); 
     }
     // O2
     if(msisAtmosTable[i][3] > zeroThreshold) 
     {
     layerMaterial->AddMaterial(diOxygen, // material, mass fraction
		     msisAtmosTable[i][3]/totalAtmosMass); 
     }
     // He 
     if(msisAtmosTable[i][6] > zeroThreshold) 
     {
     layerMaterial->AddElement(He, // material, mass fraction
		     msisAtmosTable[i][6]/totalAtmosMass); 
     }
     // Ar 
     if(msisAtmosTable[i][7] > zeroThreshold) 
     {
     layerMaterial->AddElement(Ar, // material, mass fraction
		     msisAtmosTable[i][7]/totalAtmosMass); 
     }
     // H 
     if(msisAtmosTable[i][8] > zeroThreshold) 
     {
     layerMaterial->AddElement(H, // material, mass fraction
		     msisAtmosTable[i][8]/totalAtmosMass); 
     }
     // N
     if(msisAtmosTable[i][9] > zeroThreshold) 
     {
     layerMaterial->AddElement(N, // material, mass fraction
		     msisAtmosTable[i][9]/totalAtmosMass); 
     }

     logicLayer = new G4LogicalVolume(atmosphereLayer,
		                   layerMaterial,
				   "AtmosphereLayer"+std::to_string(i));

     
     layerLocation = (i-tableSize/2) * layerThickness + layerThickness/2.;
     
     new G4PVPlacement(0,
		       G4ThreeVector(0.,0.,layerLocation),
		       logicLayer,
		       "AtmosphereLayer"+std::to_string(i),
		       fLogicWorld,
		       false,
		       i,
		       checkOverlaps);

  }

  // always return the physical World
  return physWorld;
}

void DetectorConstruction::ConstructSDandField()
{

  if(!fEmFieldSetup.Get())
  {    
    F03FieldSetup* emFieldSetup = new F03FieldSetup();    
    fEmFieldSetup.Put(emFieldSetup);
      
    G4AutoDelete::Register(emFieldSetup);
  }
    fLogicWorld->SetFieldManager(fEmFieldSetup.Get()->
		        GetGlobalFieldManager(), true); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
