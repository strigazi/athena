from PowhegControl.PowhegUtils import PowhegConfig_WZ

# Use the Powheg_bb configuration
if 'runArgs' in dir() : 
  PowhegConfig = PowhegConfig_WZ(runArgs)
else :
  PowhegConfig = PowhegConfig_WZ()
