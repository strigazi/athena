/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TGC_Digitization_page Short Description of TGC Digitization Package

@author Yoji.Hasegawa@cern.ch

@section TGC_Digitization_TGC Digitization Introduction

This is a short description on the package. A detailed description is
available in the note on the Muon Digitization.

This package has the following functionalities:

  - conversion TGC hits generated by the simulation to TGC digits and
  - simulation of TGC detector response.

@section TGC_Digitization_Conversion of Hits to Digits

Hit positions of R and phi coordinates in TGC can be read by gangs of
wires and strips, respectively. The parameters needed for digitization,
such as number of ganged wires and boundary positions between strips,
are gotten from MuonGeoModel which reads those parameters from
amdb_simrec database. TGC digitizer calculates the id number of the
wire gang or strip to which a hit belongs based on the parameters.

As the result of the digitization, TGC digits have offline ID which
describes stationName/statonPhi/stationEta/gasGap/is_strip/channel for
the digits.

@section TGC_Digitization_Detector Response Simulation

TGC digitization simulates the following detector responses:

- Multi hits:
  Multi hits in wire gangs and strips are due to ionization along track's
  path extending several wire gangs. These tracks potentially gives multi
  hits in wire gangs and strips. In addition, induced charge spread on
  cathode plane by avalanche around wire may make hits in several strips.

- Intrinsic time response:
  Variation of strength of electric field in a sensitive layer depends on
  position, this results in the dependence of time response on injection
  angle of a charged track which is defines the path of the track in the
  sensitive layer. The time response is parametrized as a function of
  injection angle by a field simulation.
  Signal propagation time along wires or strips are parametrized by the
  data taken at test beams.

- Detection efficiency (sensitivity):
  This can be also parametrized by the data taken at test beams. At the
  moment, a unique efficiency value is applied to wire gangs or strips
  independently. Eventually, the efficiency map for each sensitive layer,
  which has been taken at the test station with cosmic rays, will be used.

*/