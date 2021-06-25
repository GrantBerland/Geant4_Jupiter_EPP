

#ifndef myHistogram_h
#define myHistogram_h 1

#include <string>
#include <fstream>

// Written by Grant Berland
//
// Header-only histogramming class to record energy deposition
// per altitude bin


class myHistogram
{
public:

  myHistogram();
  
  ~myHistogram();

  // Tells program whether or not to write to histogram if other 
  // data collections methods are selected
  void InitializeHistogram(){ initializedFlag = 1; };
  
  // Method to fill histogram with data
  void AddCountToBin(unsigned int, double);
  
  // Writes vector to file name provided
  void WriteHistogramToFile(std::string);

  // Writes arbitrary data line directly to a filename specified
  void WriteDirectlyToFile(std::string, double*, size_t);

  int GetInitializationFlag();

private:
  // Need to run InitializeHistogram() method to write particle energy 
  // depostion to histogram
  int initializedFlag = 0;   
  
  // Array initialized to zeros (with fixed resolution)
  double histogramArray[1000] = {};
};

// Inline constructor and destructor methods
inline myHistogram::myHistogram()
  : histogramArray()
{}

inline myHistogram::~myHistogram()
{}

inline void myHistogram::AddCountToBin(unsigned int binAddress, 
			               double       energyToAdd)
{
  histogramArray[binAddress] += energyToAdd;
}

inline void myHistogram::WriteHistogramToFile(std::string filename)
{
  // overwrite since this method wasn't working
  initializedFlag = 1;
  if(initializedFlag == 1)
  {
    std::ofstream outputFile;
  
    // OPEN
    outputFile.open(filename, std::ios_base::app);

    for(unsigned int i=0; i<1000; i++)
    {
      outputFile << histogramArray[i] << "\n";
    }
  
    // CLOSE
    outputFile.close();
    }
  
  else if(initializedFlag == 0)
  {
    //std::cout << "Histogram not initialized" << std::endl;
    throw std::runtime_error("Histogram not initialized");
  }
}
  
inline void myHistogram::WriteDirectlyToFile(std::string filename, 
		                             double *data,
					     size_t  arraySize)
{
  std::ofstream outputFile;
  
  // OPEN
  outputFile.open(filename, std::ios_base::app);

  // Writes entire line in 'data[]' to file, comma delimited
  for(unsigned int i=0; i<arraySize-1; i++)
  {
    outputFile << data[i] << ',';
  }
  
  // Ends output with last entry and newline character
  outputFile << data[arraySize-1] << '\n';

  // CLOSE
  outputFile.close();
}

inline int myHistogram::GetInitializationFlag()
{
  return initializedFlag;
}


#endif
