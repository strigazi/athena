#include "GeomACTS/ACTSTrackingGeometry.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GeomACTS/TrackingGeometrySvc.h"
#include "GeomACTS/ExtrapolationTool.h"
#include "GeomACTS/ObjWriterTool.h"
#include "GeomACTS/ExCellWriterSvc.h"

DECLARE_COMPONENT( ACTSTrackingGeometry )
DECLARE_COMPONENT( Acts::TrackingGeometrySvc )
DECLARE_COMPONENT( Acts::ExtrapolationTool )
DECLARE_COMPONENT( Acts::ObjWriterTool )
DECLARE_COMPONENT( Acts::ExCellWriterSvc )
