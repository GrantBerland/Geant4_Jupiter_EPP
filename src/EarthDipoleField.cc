

#include "EarthDipoleField.hh"

EarthDipoleField::EarthDipoleField()
: G4MagneticField(),
  fDipoleMoment(8.05e6),    // Tesla-km^3
  fGeomagLatitude(65.77),   // deg, Poker Flat geomagnetic latitude
  fEarthRadius(6371.)       // km
{}


EarthDipoleField::~EarthDipoleField()
{}


void EarthDipoleField::GetFieldValue(const G4double Point[4],
		                             G4double *Bfield) const
{
  // Point is a spacetime 4-vector:
  // Point[0..3] ~ (x, y, z, t)
  // Bfield is a pointer to a 6x1 array of E- and B-field components 
	
  G4double geomagLat_radians = fGeomagLatitude * 3.1415926 / 180.;

  // Radial distance from Earth center, input in kilometers
  // 1020 km / 2 = 510 km addition to account for coordinate axes in 
  // center of simulation volume
  G4double z = fEarthRadius + (Point[2]/km + 1020./2.);  // km

  G4double B_magnitude = fDipoleMoment / std::pow(z, 3) * tesla; // T


  Bfield[0] = 0; // Bx
  Bfield[1] = B_magnitude * std::cos(geomagLat_radians);       // By
  //Bfield[1] = 0;      // By
  Bfield[2] = B_magnitude * -2. * std::sin(geomagLat_radians); // Bz
  //Bfield[2] = B_magnitude * std::sqrt(1 + 
  //		  3 * std::pow(std::sin(geomagLat_radians), 2)); // Bz
  
  Bfield[3] = 0; // Ex
  Bfield[4] = 0; // Ey
  Bfield[5] = 0; // Ez

  //std::cout << Bfield[2]/tesla << std::endl;
}


