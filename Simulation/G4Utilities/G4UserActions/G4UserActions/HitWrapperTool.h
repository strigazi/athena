/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__HITWRAPPERTOOL_H
#define G4USERACTIONS_G4UA__HITWRAPPERTOOL_H

#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include  "G4AtlasTools/ActionToolBase.h"
#include "G4UserActions/HitWrapper.h"

namespace G4UA
{

  /// @brief A tool which manages the HitWrapper user action.
  class HitWrapperTool : public ActionToolBase<HitWrapper>,
                         public IEndEventActionTool
  {
    public:
      /// Standard constructor
      HitWrapperTool(const std::string& type, const std::string& name,
                     const IInterface* parent);

      virtual IEndEventAction* getEndEventAction() override final
      { return static_cast<IEndEventAction*>( getAction() ); }

    protected:
      virtual std::unique_ptr<HitWrapper> makeAction() override final;

    private:
      HitWrapper::Config m_config;

  }; // class HitWrapperTool

} // namespace G4UA

#endif
