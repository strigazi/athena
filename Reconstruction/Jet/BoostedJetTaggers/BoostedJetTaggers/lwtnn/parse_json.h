// MIT License
// Copyright (c) 2017 Daniel Hay Guest and lwtnn contributors

// WARNING: this code was copied automatically from
// https://github.com/lwtnn/lwtnn.git (rev v2.0)
// Please don't edit it! To get the latest version, run
// > ./update-lwtnn.sh
// from BoostedJetTaggers/share

#ifndef PARSE_JSON_HH
#define PARSE_JSON_HH

#include "NNLayerConfig.h"

#include <istream>
#include <map>

namespace lwt {
  struct JSONConfig
  {
    std::vector<LayerConfig> layers;
    std::vector<Input> inputs;
    std::vector<std::string> outputs;
    std::map<std::string, double> defaults;
    std::map<std::string, std::string> miscellaneous;
  };
  JSONConfig parse_json(std::istream& json);
}


#endif
