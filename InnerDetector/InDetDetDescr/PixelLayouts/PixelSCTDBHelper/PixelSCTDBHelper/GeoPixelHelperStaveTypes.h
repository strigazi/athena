/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoPixelHelperStaveTypes_H
#define GeoPixelHelperStaveTypes_H

// Class to interpret and query GeoPixelStaveType table

#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"
#include <map>


  class GeoPixelHelperStaveTypes {
    
  public :
    GeoPixelHelperStaveTypes(const OraclePixGeoAccessor* db, IRDBRecordset_ptr table);
    int getFluidType(int layer, int phiModule) const;
    int getBiStaveType(int layer, int phiModule) const;
    
  private :
    class Key 
    {
    public:
      Key(int layer_in, int phiModule_in);
      int layer;
      int phiModule;
      bool operator<(const Key &rhs) const;
    };
    
    class Datum
    {
    public:
      Datum(int fluidType_in = 0, int biStaveType_in = 0);
      int fluidType;
      int biStaveType;
    };
    
    const Datum & getData(int layer, int phiModule) const;
    
    typedef std::map<Key, Datum> MapType;
    MapType m_dataLookup;
    
    std::map<int,int> m_maxSector;
    
    static Datum s_defaultDatum;
    
  };


#endif // PixelStaveTypes_H
