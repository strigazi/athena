/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HltOstreams_H
#define HltOstreams_H

#include <iostream>
#include <set>
#include <vector>
#include <stdint.h>
#include "eformat/eformat.h"

std::ostream& operator<<(std::ostream& os, const std::vector<eformat::helper::StreamTag>& rhs);
std::ostream& operator<<(std::ostream& os, const std::vector<uint32_t>& rhs);
std::ostream& operator<<(std::ostream& os, const std::vector<eformat::SubDetector>& rhs);
std::ostream& operator<<(std::ostream& os, const std::set<uint32_t>& rhs);
std::ostream& operator<<(std::ostream& os, const std::set<eformat::SubDetector>& rhs);

#endif // HltOstreams_H
