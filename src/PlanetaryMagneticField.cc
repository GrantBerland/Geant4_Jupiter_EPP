

#include "PlanetaryMagneticField.hh"
#include <fstream>

/* The following class calculates the Earth's magnetic field strength 
 * and direction according to a tilted dipole model. This class inherits
 * from G4MagneticField. GetFieldValue() is a Geant virtual method that 
 * is called to obtain magnetic and electric field values for particle 
 * propagation purposes.
 * 
 * Future work: 
 *  - implement IGRF magnetic field (and others)
 *  - read from file 
 * Grant Berland
 */


PlanetaryMagneticField::PlanetaryMagneticField()
: G4MagneticField(),
  fDipoleMoment(8.05e6),    // Tesla-km^3
  fGeomagLatitude(65.77),   // deg, Poker Flat geomagnetic latitude
  fEarthRadius(6371.),      // km,
  fJupiterRadius(71492.),   // km,
  fWhichPlanet(1),
  fMagData()
{

  switch(fWhichPlanet)
    {	
        case(0): // Earth
	  // Do Earth dipole method
	  break;
	
	case(1): // Jupiter
  	
	  ReadMagneticFieldFile("Jupiter_magneticField.csv", fMagData);
	  break;
	
	default:
	  throw std::runtime_error("Chose a planetary magnetic field!");

  }  

}


PlanetaryMagneticField::~PlanetaryMagneticField()
{
}


void PlanetaryMagneticField::GetFieldValue(const G4double Point[4],
		                             G4double *Bfield) const
{
  

  // Point is a spacetime 4-vector:
  // Point[0..3] ~ (x, y, z, t)
  // Bfield is a pointer to a 6x1 array of E- and B-field components 
	

  // Bfield[0] ~ West direction
  // Bfield[1] ~ North direction
  // Bfield[2] ~ Up direction, or radially out from Earth 
  

  G4double geomagLat_radians = fGeomagLatitude * 3.1415926 / 180.;

  // Radial distance from Earth center, input in kilometers
  // 1000 km / 2 = 500 km addition to account for coordinate axes in 
  // center of simulation volume
  G4double z = fEarthRadius + (Point[2]/km + 1000./2.);  // km

  // Magnitude of B-field, units assigned here
  G4double B_magnitude = fDipoleMoment / std::pow(z, 3) * tesla; // T


  switch(fWhichPlanet)
  {
    case 0:
  	  Bfield[0] = 0; // Bx
  	  Bfield[1] = B_magnitude * std::cos(geomagLat_radians);       // By
  	  Bfield[2] = B_magnitude * -2. * std::sin(geomagLat_radians); // Bz
  	  Bfield[3] = 0; // Ex
  	  Bfield[4] = 0; // Ey
  	  Bfield[5] = 0; // Ez
  	  break;
	
    case 1:
  	  Bfield[0] = 0; // Bx
  	  Bfield[1] = B_magnitude * std::cos(geomagLat_radians);       // By
  	  Bfield[2] = B_magnitude * -2. * std::sin(geomagLat_radians); // Bz
  	  Bfield[3] = 0; // Ex
  	  Bfield[4] = 0; // Ey
  	  Bfield[5] = 0; // Ez
	  break;
	
    default:
 	  throw std::runtime_error("Chose a planetary magnetic field!");
  }

  // Debugging print statement
  //std::cout << "Bz = " << (Bfield[2]/tesla)*1e9 << " nT" << std::endl;
}

void PlanetaryMagneticField::ReadMagneticFieldFile(G4String filename, G4double datatable[][3])
{

  std::ifstream filePtr;
  std::string line;
  G4int word_counter = 0;


  // OPEN
  filePtr.open(filename, std::ios::in);

  // Check that file is open/accesible, throw error if not
  if(!filePtr.is_open())
  { 
    G4String errorMessage = "Could not open file ";
    errorMessage += filename;
    errorMessage += " !\n";
    throw std::runtime_error(errorMessage);
  }

  int lineIndex = 0;
  while(getline(filePtr, line))
  {
    if(filePtr.good()){
    // Get line
    filePtr >> line;
	
    //std::cout << line << ',' << lineIndex << std::endl;

    // Instantiate stringstream from line
    std::stringstream s_ptr(line); 
    
    word_counter = 0;
    //while(s_ptr.good())
    for(unsigned int i = 0; i < 3; i++)
    {
      if(s_ptr.good()){
      // Parse line into words delimited by commas
      G4String word;
      getline(s_ptr, word, ',');

      // Convert to double and assign table entry
      datatable[lineIndex][i] = std::stod(word);
    
      word_counter++;
    }
      }
    }
    lineIndex++;
    std::cout << lineIndex << std::endl;
  }


  // CLOSE
  filePtr.close();
  std::cout << "Got to here!" << std::endl;

}
