

#ifndef myHistogram_h
#define myHistogram_h 1

#include <string>
#include <fstream>

class myHistogram
{
public:
  myHistogram();
  ~myHistogram();

  void AddCountToBin(unsigned int, double);
  void WriteHistogramToFile(std::string);

private:
  // Array initialized to zeros
  double histogramArray[1000] = {};
};

// Inline constructor and destructor
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
  std::ofstream outputFile;
  outputFile.open(filename, std::ios_base::app);

  for(unsigned int i=0; i<1000; i++)
  {
	  outputFile << histogramArray[i] << "\n";
  }

  outputFile.close();
}

#endif
