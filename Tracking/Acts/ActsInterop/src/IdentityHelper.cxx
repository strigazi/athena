#include "ActsInterop/IdentityHelper.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"


IdentityHelper::IdentityHelper(const InDetDD::SiDetectorElement *elem)
  : m_elem(elem)
{}
  

const PixelID* 
IdentityHelper::getPixelIDHelper() const 
{
  return dynamic_cast<const PixelID*>(m_elem->getIdHelper());
}
const SCT_ID* 
IdentityHelper::getSCTIDHelper() const
{
  return dynamic_cast<const SCT_ID*>(m_elem->getIdHelper());
}

int 
IdentityHelper::bec() const 
{
  auto id = m_elem->identify();
  if (m_elem->isPixel()) {
    return getPixelIDHelper()->barrel_ec(id);
  }
  else {
    return getSCTIDHelper()->barrel_ec(id);
  }
}

int 
IdentityHelper::layer_disk() const
{
  auto id = m_elem->identify();
  if (m_elem->isPixel()) {
    return getPixelIDHelper()->layer_disk(id);
  }
  else {
    return getSCTIDHelper()->layer_disk(id);
  }
}

int 
IdentityHelper::phi_module() const
{
  auto id = m_elem->identify();
  if (m_elem->isPixel()) {
    return getPixelIDHelper()->phi_module(id);
  }
  else {
    return getSCTIDHelper()->phi_module(id);
  }
}

int 
IdentityHelper::eta_module() const
{
  auto id = m_elem->identify();
  if (m_elem->isPixel()) {
    return getPixelIDHelper()->eta_module(id);
  }
  else {
    return getSCTIDHelper()->eta_module(id);
  }
}

int 
IdentityHelper::side() const
{
  auto id = m_elem->identify();
  if (m_elem->isPixel()) {
    return 0;
  }
  else {
    return getSCTIDHelper()->side(id);
  }
}

int 
IdentityHelper::phi_module_max() const
{
  auto id = m_elem->identify();
  if (m_elem->isPixel()) {
    return getPixelIDHelper()->phi_module_max(id);
  }
  else {
    return getSCTIDHelper()->phi_module_max(id);
  }
}

int 
IdentityHelper::eta_module_max() const
{
  auto id = m_elem->identify();
  if (m_elem->isPixel()) {
    return getPixelIDHelper()->eta_module_max(id);
  }
  else {
    return getSCTIDHelper()->eta_module_max(id);
  }
}
