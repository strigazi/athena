/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <algorithm>
#include <sstream>

// Local
#include "TrigMonitoringEvent/TrigConfAlg.h"

using namespace std;

//--------------------------------------------------------------------------------------  
TrigConfAlg::TrigConfAlg() 
  :m_index(0),
   m_position(0),
   m_name_id(0),
   m_type_id(0),
   m_name(),
   m_type()
{
}

//--------------------------------------------------------------------------------------  
TrigConfAlg::TrigConfAlg(const uint32_t index,
		       const uint32_t position,
		       const std::string name,
		       const std::string type, 
		       const uint32_t name_id,
		       const uint32_t type_id)
  :m_index(index),
   m_position(position),
   m_name_id(name_id),
   m_type_id(type_id),
   m_name(name),
   m_type(type)
{
  if(position >= 128) {
    cerr << "TrigConfAlg ctor error! Position is too large: " << position << endl;
  }
  if(position >= 65535) {
    cerr << "TrigConfAlg ctor error! Index is too large: " << index << endl;
  }
}

//--------------------------------------------------------------------------------------  
void TrigConfAlg::clearStrings()
{
  //
  // Clear all string variables
  //
  m_name.clear();
  m_type.clear();
}

//--------------------------------------------------------------------------------------  
void TrigConfAlg::print(std::ostream &os) const
{
  os << str(*this) << endl;
}

//--------------------------------------------------------------------------------------  
std::string str(const TrigConfAlg &o)
{
  std::stringstream s;
  s << "TrigConfAlg: " << o.getPosition() << " " 
      << o.getName()   << "/" << o.getType() << " = "
      << o.getNameId() << "/" << o.getTypeId();

  return s.str();
}
