/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MonitoredScalar_h
#define MonitoredScalar_h

#include <vector>

#include "AthenaMonitoring/IMonitoredVariable.h"
#include "AthenaMonitoring/MonitoredHelpers.h"

namespace Monitored {
    namespace MonitoredScalar {
        template<class T>
        class MonitoredScalar;
        
        template<class T>
        MonitoredScalar<T> declare(std::string name, const T& defaultValue = {});
        
        template<class T>
        class MonitoredScalar : public IMonitoredVariable {
        public:
            static_assert(MonitoredHelpers::has_double_operator<T>::value, "Value must be convertable to double");
            friend MonitoredScalar<T> declare<T>(std::string name, const T& defaultValue);
            
            MonitoredScalar(MonitoredScalar&&) = default;
            
            void operator=(T value) { mValue = value; }
            operator T() const { return mValue; }
            
            const std::vector<double> getVectorRepresentation() const override { return { double(mValue) }; }
        private:
            T mValue;
            
            MonitoredScalar(std::string name, const T& defaultValue = {})
              : IMonitoredVariable(std::move(name)), mValue(defaultValue) { }
            MonitoredScalar(MonitoredScalar const&) = delete;
            MonitoredScalar& operator=(MonitoredScalar const&) = delete;
        };
        
        template<class T>
        MonitoredScalar<T> declare(std::string name, const T& defaultValue) {
            return MonitoredScalar<T>(std::move(name), defaultValue);
        }
    }
}

#endif /* MonitoredScalar_h */
