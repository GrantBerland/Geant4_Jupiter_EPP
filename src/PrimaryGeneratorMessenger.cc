

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
  fPrimDir->SetGuidance("Select beam energy and distribution.");

  fcmd = new G4UIcmdWithAnInteger("/energy/setDistributionType",this);
  fcmd->SetParameterName("0 - Exponential, 1 - Monoenergetic",true);
  fcmd->SetDefaultValue(0);
  fcmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fDcmd = new G4UIcmdWithADouble("/energy/setFoldingEnergy",this);
  fDcmd->SetParameterName("Folding or Mono Energy [keV]",true);
  fDcmd->SetDefaultValue(100.);
  fDcmd->AvailableForStates(G4State_PreInit, G4State_Idle);

}

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  delete fPrimDir;
  delete fcmd;
  delete fDcmd;
}
void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, 
					    G4String newValue)
{

  if(command == fcmd){
    fPrimaryGenerator->SetDistribution(std::stoi(newValue));
  }    	  

  if(command == fDcmd){
    fPrimaryGenerator->SetEnergy(std::stod(newValue));
  }

}
