

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

  void InitializeHistogram(){ initializedFlag = 1; };
  
  // Method to fill histogram with data
  void AddCountToBin(unsigned int, double);
  
  // Writes vector to file name provided
  void WriteHistogramToFile(std::string);

  // Writes arbitrary data line directly to a filename specified
  void WriteDirectlyToFile(std::string, double*);


private:
  
  int initializedFlag = 0;   
  
  // Array initialized to zeros
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
    std::cout << "Histogram not initialized" << std::endl;
  }

}
  
inline void myHistogram::WriteDirectlyToFile(std::string filename, 
		                             double *data)
{
  std::ofstream outputFile;
  
  // OPEN
  outputFile.open(filename, std::ios_base::app);

  size_t data_length = sizeof(data)/sizeof(*data);
  data_length = 3; 
  // Writes entire line in 'data[]' to file, comma delimited
  for(unsigned int i=0; i<data_length; i++)
  {
    outputFile << data[i] << ',';
  }

  // Ends output with newline character
  outputFile << '\n';
  
  // CLOSE
  outputFile.close();

}
#endif
