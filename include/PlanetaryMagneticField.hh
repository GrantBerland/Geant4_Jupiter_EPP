

#ifndef PLANETARYMAGNETICFIELD_h
#define PLANETARYMAGNETICFIELD_h 1

#include "G4MagneticField.hh"
#include "G4SystemOfUnits.hh"

class PlanetaryMagneticField : public G4MagneticField
{
public:

  PlanetaryMagneticField();
  virtual ~PlanetaryMagneticField() override;

  virtual void GetFieldValue(const G4double Point[4], 
		                   G4double *Bfield) const override;
  
  void ReadMagneticFieldFile(G4String, G4double(*)[3]);

private:
  G4double fDipoleMoment;
  G4double fGeomagLatitude;
  G4double fEarthRadius;
  G4double fJupiterRadius;
  G4int    fWhichPlanet;
  G4double fMagData[1000][3];

};

#endif
