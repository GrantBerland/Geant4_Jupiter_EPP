

#include "PrimaryGeneratorMessenger.hh"

#include "PrimaryGeneratorAction.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIdirectory.hh"


PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* prim)
  :G4UImessenger(),
   fPrimaryGenerator(prim) 
{
  fPrimDir = new G4UIdirectory("/energy/");
  fPrimDir->SetGuidance("Select particle distributions.");

  fcmd = new G4UIcmdWithAnInteger("/energy/setEnergyDistributionType",this);
  fcmd->SetParameterName("0 - Exponential, 1 - Monoenergetic",true);
  fcmd->SetDefaultValue(0);
  fcmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  fcmd2 = new G4UIcmdWithAnInteger("/energy/setPitchAngleDistributionType",this);
  fcmd2->SetParameterName("TODO: write options in",true);
  fcmd2->SetDefaultValue(0);
  fcmd2->AvailableForStates(G4State_PreInit, G4State_Idle);

  fDcmd = new G4UIcmdWithADouble("/energy/setFoldingEnergy",this);
  fDcmd->SetParameterName("Folding or Mono Energy [keV]",true);
  fDcmd->SetDefaultValue(100.);
  fDcmd->AvailableForStates(G4State_PreInit, G4State_Idle);

}

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  delete fPrimDir;
  delete fcmd;
  delete fcmd2;
  delete fDcmd;
}
void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, 
					    G4String newValue)
{

  if(command == fcmd){
    fPrimaryGenerator->SetEnergyDistribution(std::stoi(newValue));
  }    	  
  
  if(command == fcmd2){
    fPrimaryGenerator->SetPitchAngleDistribution(std::stoi(newValue));
  }    	  

  if(command == fDcmd){
    fPrimaryGenerator->SetEnergy(std::stod(newValue));
  }

}
