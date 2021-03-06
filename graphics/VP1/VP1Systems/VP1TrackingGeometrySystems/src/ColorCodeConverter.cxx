/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class ColorCodeConverter                         //
//                                                                     //
//  Author: Joerg Mechnich    <Joerg.Mechnich@cern.ch> (primary)       //
//          Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial VP1 version: September 2007                                //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1TrackingGeometrySystems/ColorCodeConverter.h"

#include <Inventor/nodes/SoMaterial.h>

// The code for the conversion table can be generated from ROOT with
// the following script:

/*
{
  unsigned int indexMax = 200;
  TColor* color;
  cout << "const double SurfaceToSoNode::s_colorConversionTable[][3] = " << endl
       << "{" << endl;
  for( unsigned int i=0; i < indexMax; ++i)
  {
    color = gROOT->GetColor(i);
    cout << "  { "
         << color->GetRed() << ", "
         << color->GetGreen() << ", "
         << color->GetBlue() << " }," << endl;
  }
  cout << "}" << endl;

  cout << "const unsigned int SurfaceToSoNode::s_colorConversionTableSize = "
       << indexMax << ";" << endl;
}
*/
  
const double
ColorCodeConverter::s_colorConversionTable[][3] =
{
  { 1, 1, 1 },
  { 0, 0, 0 },
  { 1, 0, 0 },
  { 0, 1, 0 },
  { 0, 0, 1 },
  { 1, 1, 0 },
  { 1, 0, 1 },
  { 0, 1, 1 },
  { 0.35, 0.83, 0.33 },
  { 0.35, 0.33, 0.85 },
  { 0.999, 0.999, 0.999 },
  { 0.754, 0.715, 0.676 },
  { 0.3, 0.3, 0.3 },
  { 0.4, 0.4, 0.4 },
  { 0.5, 0.5, 0.5 },
  { 0.6, 0.6, 0.6 },
  { 0.7, 0.7, 0.7 },
  { 0.8, 0.8, 0.8 },
  { 0.9, 0.9, 0.9 },
  { 0.95, 0.95, 0.95 },
  { 0.8, 0.78, 0.67 },
  { 0.8, 0.78, 0.67 },
  { 0.76, 0.75, 0.66 },
  { 0.73, 0.71, 0.64 },
  { 0.7, 0.65, 0.59 },
  { 0.72, 0.64, 0.61 },
  { 0.68, 0.6, 0.55 },
  { 0.61, 0.56, 0.51 },
  { 0.53, 0.4, 0.34 },
  { 0.69, 0.81, 0.78 },
  { 0.52, 0.76, 0.64 },
  { 0.54, 0.66, 0.63 },
  { 0.51, 0.62, 0.55 },
  { 0.68, 0.74, 0.78 },
  { 0.48, 0.56, 0.6 },
  { 0.46, 0.54, 0.57 },
  { 0.41, 0.51, 0.59 },
  { 0.43, 0.48, 0.52 },
  { 0.49, 0.6, 0.82 },
  { 0.5, 0.5, 0.61 },
  { 0.67, 0.65, 0.75 },
  { 0.83, 0.81, 0.53 },
  { 0.87, 0.73, 0.53 },
  { 0.74, 0.62, 0.51 },
  { 0.78, 0.6, 0.49 },
  { 0.75, 0.51, 0.47 },
  { 0.81, 0.37, 0.38 },
  { 0.67, 0.56, 0.58 },
  { 0.65, 0.47, 0.48 },
  { 0.58, 0.41, 0.44 },
  { 0.83, 0.35, 0.33 },
  { 0.573333, 0, 1 },
  { 0.48, 0, 1 },
  { 0.386667, 0, 1 },
  { 0.293333, 0, 1 },
  { 0.2, 0, 1 },
  { 0.106667, 0, 1 },
  { 0.0133331, 0, 1 },
  { 0, 0.0800001, 1 },
  { 0, 0.173333, 1 },
  { 0, 0.266667, 1 },
  { 0, 0.36, 1 },
  { 0, 0.453333, 1 },
  { 0, 0.546667, 1 },
  { 0, 0.64, 1 },
  { 0, 0.733333, 1 },
  { 0, 0.826667, 1 },
  { 0, 0.92, 1 },
  { 0, 1, 0.986667 },
  { 0, 1, 0.893333 },
  { 0, 1, 0.8 },
  { 0, 1, 0.706667 },
  { 0, 1, 0.613333 },
  { 0, 1, 0.52 },
  { 0, 1, 0.426667 },
  { 0, 1, 0.333333 },
  { 0, 1, 0.24 },
  { 0, 1, 0.146667 },
  { 0, 1, 0.0533333 },
  { 0.0399999, 1, 0 },
  { 0.133333, 1, 0 },
  { 0.226667, 1, 0 },
  { 0.32, 1, 0 },
  { 0.413333, 1, 0 },
  { 0.506667, 1, 0 },
  { 0.6, 1, 0 },
  { 0.693333, 1, 0 },
  { 0.786667, 1, 0 },
  { 0.88, 1, 0 },
  { 0.973333, 1, 0 },
  { 1, 0.933333, 0 },
  { 1, 0.84, 0 },
  { 1, 0.746667, 0 },
  { 1, 0.653333, 0 },
  { 1, 0.56, 0 },
  { 1, 0.466667, 0 },
  { 1, 0.373333, 0 },
  { 1, 0.28, 0 },
  { 1, 0.186667, 0 },
  { 1, 0.0933333, 0 },
  { 1, 0, 0 },
  { 0, 0, 0 },
  { 0.7, 0, 0 },
  { 0, 0.7, 0 },
  { 0, 0, 0.7 },
  { 0.7, 0.7, 0 },
  { 0.7, 0, 0.7 },
  { 0, 0.7, 0.7 },
  { 0.183667, 0.647667, 0.164333 },
  { 0.171244, 0.151098, 0.674902 },
  { 0.999, 0.999, 0.999 },
  { 0.568853, 0.5005, 0.432147 },
  { 0.21, 0.21, 0.21 },
  { 0.28, 0.28, 0.28 },
  { 0.35, 0.35, 0.35 },
  { 0.42, 0.42, 0.42 },
  { 0.49, 0.49, 0.49 },
  { 0.56, 0.56, 0.56 },
  { 0.63, 0.63, 0.63 },
  { 0.665, 0.665, 0.665 },
  { 0.633585, 0.596943, 0.395415 },
  { 0.633585, 0.596943, 0.395415 },
  { 0.58269, 0.565552, 0.41131 },
  { 0.548, 0.517556, 0.411 },
  { 0.521451, 0.457859, 0.381549 },
  { 0.541925, 0.430761, 0.389075 },
  { 0.503182, 0.413727, 0.357818 },
  { 0.436545, 0.392, 0.347455 },
  { 0.371, 0.28, 0.238 },
  { 0.411, 0.639, 0.582 },
  { 0.298667, 0.597333, 0.448 },
  { 0.357, 0.483, 0.4515 },
  { 0.345494, 0.445506, 0.381862 },
  { 0.420445, 0.529111, 0.601556 },
  { 0.328696, 0.394435, 0.427304 },
  { 0.319619, 0.379082, 0.401381 },
  { 0.287, 0.357, 0.413 },
  { 0.301, 0.336, 0.364 },
  { 0.239217, 0.385406, 0.677783 },
  { 0.340483, 0.340483, 0.436517 },
  { 0.441, 0.408333, 0.571667 },
  { 0.699125, 0.669375, 0.252875 },
  { 0.767667, 0.539, 0.212333 },
  { 0.571667, 0.431667, 0.303333 },
  { 0.621082, 0.401877, 0.267918 },
  { 0.580282, 0.317513, 0.273718 },
  { 0.63461, 0.19139, 0.201463 },
  { 0.492, 0.369, 0.391364 },
  { 0.472182, 0.311818, 0.320727 },
  { 0.406, 0.287, 0.308 },
  { 0.647667, 0.183667, 0.164333 },
  { 0, 0, 0 },
  { 1, 0.2, 0.2 },
  { 0.2, 1, 0.2 },
  { 0.2, 0.2, 1 },
  { 1, 1, 0.2 },
  { 1, 0.2, 1 },
  { 0.2, 1, 1 },
  { 0.529524, 0.876952, 0.515048 },
  { 0.537073, 0.522829, 0.893171 },
  { 1, 1, 1 },
  { 0.877432, 0.858, 0.838568 },
  { 0.36, 0.36, 0.36 },
  { 0.48, 0.48, 0.48 },
  { 0.6, 0.6, 0.6 },
  { 0.72, 0.72, 0.72 },
  { 0.84, 0.84, 0.84 },
  { 0.96, 0.96, 0.96 },
  { 1, 1, 1 },
  { 1, 1, 1 },
  { 0.910943, 0.902038, 0.853057 },
  { 0.910943, 0.902038, 0.853057 },
  { 0.877517, 0.872414, 0.826483 },
  { 0.847429, 0.836127, 0.796571 },
  { 0.809014, 0.777183, 0.738986 },
  { 0.831164, 0.782925, 0.764836 },
  { 0.782234, 0.727792, 0.693766 },
  { 0.709273, 0.672, 0.634727 },
  { 0.626391, 0.48354, 0.417609 },
  { 0.876, 0.924, 0.912 },
  { 0.690667, 0.845333, 0.768 },
  { 0.678, 0.762, 0.741 },
  { 0.637287, 0.718713, 0.666897 },
  { 0.853037, 0.880593, 0.898963 },
  { 0.602087, 0.663304, 0.693913 },
  { 0.57468, 0.637691, 0.66132 },
  { 0.528, 0.608, 0.672 },
  { 0.529263, 0.574526, 0.610737 },
  { 0.683652, 0.751884, 0.888348 },
  { 0.624719, 0.624719, 0.707281 },
  { 0.824, 0.813333, 0.866667 },
  { 0.90225, 0.89075, 0.72975 },
  { 0.930667, 0.856, 0.749333 },
  { 0.826667, 0.746667, 0.673333 },
  { 0.856548, 0.739178, 0.667452 },
  { 0.828205, 0.663282, 0.635795 },
  { 0.864683, 0.551317, 0.558439 },
  { 0.775429, 0.700571, 0.714182 },
  { 0.739091, 0.604909, 0.612364 },
  { 0.663717, 0.524283, 0.548889 },
};

const unsigned int
ColorCodeConverter::s_colorConversionTableSize = 200;

ColorCodeConverter::~ColorCodeConverter()
{
  std::map<unsigned int, SoMaterial*>::const_iterator it;
  for( it = m_materialCache.begin(); it != m_materialCache.end(); ++it)
  {
    it->second->unref();
  }
}

SoMaterial*
ColorCodeConverter::lookup( unsigned int colorCode)
{
  SoMaterial* ret = 0;
  //sroe: coverity 16515, original was "if (colorCode > s_colorConversionTableSize)"
  //which still allows colorCode = 200, which is out-of-bounds.
  if( colorCode >= s_colorConversionTableSize)
  {
    // if color code is out of bounds choose red as default color
    colorCode = red; //red is defined as =2 in the header 
  }
  
  std::map<unsigned int, SoMaterial*>::const_iterator it = m_materialCache.find( colorCode);
  if( it != m_materialCache.end())
  {
    ret = it->second;
  }
  else
  {
    ret = new SoMaterial;
    ret->diffuseColor.setValue( s_colorConversionTable[colorCode][0],
                                s_colorConversionTable[colorCode][1],
                                s_colorConversionTable[colorCode][2]);
    ret->ref();
    m_materialCache[colorCode] = ret;
  }
  
  return ret;
}
