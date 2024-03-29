

#include "SteppingActionMessenger.hh"

#include "SteppingAction.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
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

  fcmd2 = new G4UIcmdWithADouble("/dataCollection/SetPhotonWindow",this);
  fcmd2->SetParameterName("Maximum photon tracking altitude [km]", true);
  fcmd2->SetDefaultValue(250.);
  fcmd2->AvailableForStates(G4State_PreInit, G4State_Idle);


  fcmd3 = new G4UIcmdWithAString("/dataCollection/setPhotonFileName",this);
  fcmd3->SetParameterName("Enter file name.",true);
  fcmd3->SetDefaultValue("photon_traj.csv");
  fcmd3->AvailableForStates(G4State_PreInit, G4State_Idle);

}

SteppingActionMessenger::~SteppingActionMessenger()
{
  delete fSteppingActionDir;
  delete fcmd;
  delete fcmd2;
  delete fcmd3;
}
void SteppingActionMessenger::SetNewValue(G4UIcommand* command, 
					    G4String newValue)
{
  if(command == fcmd){
    fSteppingAction->SetDataCollection(std::stoi(newValue));
  }    	 
  if(command == fcmd2){
    fSteppingAction->SetPhotonWindowAlt(std::stod(newValue));
  }    	 
  if(command == fcmd3){
    fSteppingAction->SetPhotonFilename(newValue);
  }    	 

}
