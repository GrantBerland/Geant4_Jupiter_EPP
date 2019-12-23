

#include "SteppingActionMessenger.hh"

#include "SteppingAction.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIdirectory.hh"


SteppingActionMessenger::SteppingActionMessenger(SteppingAction* step)
  :G4UImessenger(),
   fSteppingAction(step) 
{
  fSteppingActionDir = new G4UIdirectory("/dataCollection/");
  fSteppingActionDir->SetGuidance("Select data collection type.");

  fcmd = new G4UIcmdWithAnInteger("/dataCollection/setDataCollection",this);
  fcmd->SetParameterName("0 - Energy deposition with altitude, 1 - Particle trajectory (warning, lots of data!!!!)",true);
  fcmd->SetDefaultValue(0);
  fcmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

SteppingActionMessenger::~SteppingActionMessenger()
{
  delete fSteppingActionDir;
  delete fcmd;
}
void SteppingActionMessenger::SetNewValue(G4UIcommand* command, 
					    G4String newValue)
{
  if(command == fcmd){
    fSteppingAction->SetDataCollection(std::stoi(newValue));
  }    	  

}
