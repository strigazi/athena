/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include "PathResolver/PathResolver.h"
#include <fstream>


TRTParameters* TRTParameters::s_pParameters = NULL;


  // Called by GetPointer

TRTParameters::TRTParameters() : m_msg("TRTParameters")
{
  ReadInputFile("TRT_G4Utilities_management.txt");

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParameters" << endmsg;

  ReadInputFile("TRT_G4Utilities_geometry.txt");

  if (GetInteger("PrintListOfParameters"))
    PrintListOfParameters();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParameters done" << endmsg;
}


  // It was called by TRTRunAction::BeginOfRunAction...

TRTParameters::~TRTParameters()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParameters" << endmsg;

  s_pParameters = NULL;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParameters done" << endmsg;
}


  // Called by TRTParameters

void TRTParameters::ReadInputFile ATLAS_NOT_THREAD_SAFE (std::string fileName) // Thread unsafe exit function is used.
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParameters::ReadInputFile" << endmsg;

  std::string file = PathResolver::find_file (fileName, "DATAPATH");
  std::ifstream inputFile(file.c_str(), std::ios::in);

  if (!inputFile)
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTParameters::ReadInputFile *****" << std::endl;
    std::cerr << "  Cannot open input file '" << fileName << "'." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }

  int inputState = 0;
  std::string inputString;
  std::string parameterName;
  double parameterValue;
  bool parameterIsRead = false;

  while (inputFile >> inputString)
  {
    if (inputState == 0)
    {
      if (inputString == "<")
        inputState = 1;
    }
    else if (inputState == 1)
    {
      parameterName = inputString;
      inputState = 2;
    }
    else if (inputState == 2)
    {
      if (inputString == "=")
        inputState = 3;
      else
      {
        std::cerr << std::endl;
        std::cerr << "***** TRTParameters::ReadInputFile *****" << std::endl;
        std::cerr << "  Input file '" << fileName << "'." << std::endl;
	std::cerr << "  Last parameter '" << parameterName << "'." << std::endl;
        std::cerr << "  Cannot find symbol '='." << std::endl;
        std::cerr << "  Exit!" << std::endl << std::endl;
        exit(0);
      }
    }
    else if (inputState == 3)
    {
      if (inputString == ">")
      {
        if (parameterIsRead)
        {
          inputState = 0;
          parameterIsRead = false;
        }
        else
        {
          std::cerr << std::endl;
          std::cerr << "***** TRTParameters::ReadInputFile *****" << std::endl;
          std::cerr << "  Input file '" << fileName << "'." << std::endl;
          std::cerr << "  Last parameter '" << parameterName << "'." << std::endl;
          std::cerr << "  Unexpected symbol '>'." << std::endl;
          std::cerr << "  Exit!" << std::endl << std::endl;
          exit(0);
        }
      }
      else if (inputString == "<")
      {
        std::cerr << std::endl;
        std::cerr << "***** TRTParameters::ReadInputFile *****" << std::endl;
        std::cerr << "  Input file '" << fileName << "'." << std::endl;
        std::cerr << "  Last parameter '" << parameterName << "'." << std::endl;
        std::cerr << "  Unexpected symbol '<'." << std::endl;
        std::cerr << "  Exit!" << std::endl << std::endl;
        exit(0);
      }
      else
      {
        parameterValue = atof(inputString.c_str());
        m_multimapOfParameters.insert(make_pair(parameterName, parameterValue));
        parameterIsRead = true;
      }
    }
  }

  if (inputState != 0)
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTParameters::ReadInputFile *****" << std::endl;
    std::cerr << "  Input file '" << fileName << "'." << std::endl;
    std::cerr << "  Last parameter '" << parameterName << "'." << std::endl;
    std::cerr << "  Cannot find symbol '>' at the end of file." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }

  inputFile.close();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParameters::ReadInputFile done" << endmsg;
}


  // Called by TRTParameters

void TRTParameters::PrintListOfParameters ATLAS_NOT_THREAD_SAFE () const // Thread unsafe TRTOutputFile class is used.
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParameters::PrintListOfParameters" << endmsg;

  TRTOutputFile* pOutputFile = TRTOutputFile::GetPointer();

  std::ofstream& output = pOutputFile->GetReference();

  output << "***** TRTParameters::PrintListOfParameters *****" << std::endl;
  output << "List of parameters:" << std::endl;

  for (multimapIterator i = m_multimapOfParameters.begin();
    i != m_multimapOfParameters.end(); ++i)
    output << "  " << (*i).first << "=" << (*i).second << std::endl;
  output << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParameters::PrintListOfParameters done" << endmsg;
}


  // Called on demand

int TRTParameters::GetInteger ATLAS_NOT_THREAD_SAFE (std::string parameterName) const // Thread unsafe exit function is used.
{
  int numberOfItems = m_multimapOfParameters.count(parameterName);

  if (numberOfItems == 1)
  {
    multimapIterator i = m_multimapOfParameters.find(parameterName);
    int parameterValue = (int) (*i).second;
    return parameterValue;
  }
  else if (numberOfItems == 0)
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTParameters::GetInteger *****" << std::endl;
    std::cerr << "  Cannot find parameter '" << parameterName << "'." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }
  else
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTParameters::GetInteger *****" << std::endl;
    std::cerr << "  Parameter '" << parameterName << "' has " << numberOfItems
           << " copies." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }
}


  // Called on demand

double TRTParameters::GetDouble ATLAS_NOT_THREAD_SAFE (std::string parameterName) const // Thread unsafe exit function is used.
{
  int numberOfItems = m_multimapOfParameters.count(parameterName);

  if (numberOfItems == 1)
  {
    multimapIterator i = m_multimapOfParameters.find(parameterName);
    double parameterValue = (*i).second;
    return parameterValue;
  }
  else if (numberOfItems == 0)
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTParameters::GetDouble *****" << std::endl;
    std::cerr << "  Cannot find parameter '" << parameterName << "'." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }
  else
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTParameters::GetDouble *****" << std::endl;
    std::cerr << "  Parameter '" << parameterName << "' has " << numberOfItems
           << " copies." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }
}


  // Called on demand

void TRTParameters::GetIntegerArray ATLAS_NOT_THREAD_SAFE (std::string arrayName, int arraySize, // Thread unsafe exit function is used.
  int* array) const
{
  int numberOfItems = m_multimapOfParameters.count(arrayName);

  if (numberOfItems == arraySize)
  {
    multimapIterator i = m_multimapOfParameters.find(arrayName);
    for (int j = 0; j < arraySize; ++j, ++i)
      array[j] = (int) ((*i).second);
  }
  else if (numberOfItems == 0)
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTParameters::GetIntegerArray *****" << std::endl;
    std::cerr << "  Cannot find array '" << arrayName << "'." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }
  else
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTParameters::GetIntegerArray *****" << std::endl;
    std::cerr << "  Size of array '" << arrayName << "' is " << numberOfItems
           << "." << std::endl;
    std::cerr << "  Demanded size is " << arraySize << "." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }
}


  // Called on demand

void TRTParameters::GetDoubleArray ATLAS_NOT_THREAD_SAFE (std::string arrayName, int arraySize, // Thread unsafe exit function is used.
  double* array) const
{
  int numberOfItems = m_multimapOfParameters.count(arrayName);

  if (numberOfItems == arraySize)
  {
    multimapIterator i = m_multimapOfParameters.find(arrayName);
    for (int j = 0; j < arraySize; ++j, ++i)
      array[j] = (*i).second;
  }
  else if (numberOfItems == 0)
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTParameters::GetDoubleArray *****" << std::endl;
    std::cerr << "  Cannot find array '" << arrayName << "'." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }
  else
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTParameters::GetDoubleArray *****" << std::endl;
    std::cerr << "  Size of array '" << arrayName << "' is " << numberOfItems
           << "." << std::endl;
    std::cerr << "  Demanded size is " << arraySize << "." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }
}


  // Called on demand

void TRTParameters::GetPartOfIntegerArray ATLAS_NOT_THREAD_SAFE (std::string arrayName, // Thread unsafe exit function is used.
  int numberOfDemandedElements, int* array) const
{
  int numberOfItems = m_multimapOfParameters.count(arrayName);

  if (numberOfItems > numberOfDemandedElements)
  {
    multimapIterator i = m_multimapOfParameters.find(arrayName);
    for (int j = 0; j < numberOfDemandedElements; ++j, ++i)
      array[j] = (int) ((*i).second);
  }
  else if (numberOfItems == 0)
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTParameters::GetPartOfIntegerArray *****" << std::endl;
    std::cerr << "  Cannot find array '" << arrayName << "'." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }
  else
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTParameters::GetPartOfIntegerArray *****" << std::endl;
    std::cerr << "  Size of array '" << arrayName << "' is " << numberOfItems
           << "." << std::endl;
    std::cerr << "  Number of demanded elements " << numberOfDemandedElements
           << "." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }
}


  // Called on demand

void TRTParameters::GetPartOfDoubleArray ATLAS_NOT_THREAD_SAFE (std::string arrayName, // Thread unsafe exit function is used.
  int numberOfDemandedElements, double* array) const
{
  int numberOfItems = m_multimapOfParameters.count(arrayName);

  if (numberOfItems > numberOfDemandedElements)
  {
    multimapIterator i = m_multimapOfParameters.find(arrayName);
    for (int j = 0; j < numberOfDemandedElements; ++j, ++i)
      array[j] = (*i).second;
  }
  else if (numberOfItems == 0)
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTParameters::GetPartOfDoubleArray *****" << std::endl;
    std::cerr << "  Cannot find array '" << arrayName << "'." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }
  else
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTParameters::GetPartOfDoubleArray *****" << std::endl;
    std::cerr << "  Size of array '" << arrayName << "' is " << numberOfItems
           << "." << std::endl;
    std::cerr << "  Number of demanded elements " << numberOfDemandedElements
           << "." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }
}


  // Called on demand

int TRTParameters::GetElementOfIntegerArray ATLAS_NOT_THREAD_SAFE (std::string arrayName, // Thread unsafe exit function is used.
  int elementIndex) const
{
  int numberOfItems = m_multimapOfParameters.count(arrayName);

  if (elementIndex >= 0 && elementIndex < numberOfItems)
  {
    multimapIterator i = m_multimapOfParameters.find(arrayName);
    for (int j = 0; j < elementIndex; ++j, ++i) ;
    return (int) (*i).second;
  }
  else if (numberOfItems == 0)
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTParameters::GetElementOfIntegerArray *****" << std::endl;
    std::cerr << "  Cannot find array '" << arrayName << "'." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }
  else
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTParameters::GetElementOfIntegerArray *****" << std::endl;
    std::cerr << "  Invalid element index " << elementIndex << " for array '"
           << arrayName << "'." << std::endl;
    std::cerr << "  Array size " << numberOfItems << "." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }
}
