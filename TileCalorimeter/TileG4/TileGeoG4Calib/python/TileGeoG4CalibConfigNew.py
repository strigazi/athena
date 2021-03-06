# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
TileGeoG4CalibSDTool=CompFactory.TileGeoG4CalibSDTool


def TileGeoG4CalibSDCfg(ConfigFlags, name="TileGeoG4CalibSD", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("LogicalVolumeNames", [ 'Tile::Scintillator', 'Tile::Tile', 'Tile::TileCentralBarrel', 'Tile::TileEndcapPos', 'Tile::TileEndcapNeg', 'Tile::Barrel', 'Tile::BarrelModule', 'Tile::FrontPlate', 'Tile::EndPlate1', 'Tile::EPHole1', 'Tile::EndPlate2', 'Tile::EPHole2', 'Tile::GirderMother', 'Tile::GirderIron', 'Tile::GirderAluminium', 'Tile::GirderElectronics', 'Tile::Absorber', 'Tile::AbsorberChild', 'Tile::Period', 'Tile::Glue', 'Tile::Wrapper', 'Tile::EBarrel', 'Tile::EBarrelModule', 'Tile::EndPlateSh', 'Tile::ITC', 'Tile::ITCModule', 'Tile::Plug1Module', 'Tile::FrontPlateSh', 'Tile::Plug2Module', 'Tile::Gap', 'Tile::GapModule', 'Tile::IrUp', 'Tile::IrDw', 'Tile::Iron4', 'Tile::Iron3', 'Tile::Iron2', 'Tile::Iron1', 'Tile::IrBox', 'Tile::SaddleModule', 'Tile::LArService', 'Tile::LArCables', 'Tile::ExtBarrelSaddleSupport', 'Tile::Crack', 'Tile::CrackModule', 'Tile::FingerModule', 'Tile::FingerIron', 'Tile::FingerAluminum', 'Tile::FingerElectronics', 'Tile::EFinger', 'Tile::EFingerModule', 'Tile::FingerPos', 'Tile::FingerNeg', 'Tile::SaddlePos', 'Tile::SaddleNeg', 'Tile::ESaddlePos', 'Tile::ESaddleNeg' ] )
    kwargs.setdefault("OutputCollectionNames", ["TileHitVec", "TileCalibHitActiveCell", "TileCalibHitInactiveCell", "TileCalibHitDeadMaterial"] )
    
    result.setPrivateTools( TileGeoG4CalibSDTool(name, **kwargs) ) # is this private..?
    return result


def TileCTBGeoG4CalibSDCfg(ConfigFlags, name="TileCTBGeoG4CalibSD", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("LogicalVolumeNames", [ 'CTB::CTB', 'BEAMPIPE1::BEAMPIPE1', 'BEAMPIPE2::BEAMPIPE2', 'MYLAREQUIV::MYLAREQUIV', 'S1::S1', 'S2::S2', 'S3::S3', 'CALO::CALO', 'MuonWall::MuonWall', 'MuonWall::MuScintillatorLayer', 'Tile::TileTBEnv', 'Tile::Scintillator', 'Tile::Tile', 'Tile::TileCentralBarrel', 'Tile::TileEndcapPos', 'Tile::TileEndcapNeg', 'Tile::Barrel', 'Tile::BarrelModule', 'Tile::FrontPlate', 'Tile::EndPlate1', 'Tile::EPHole1', 'Tile::EndPlate2', 'Tile::EPHole2', 'Tile::GirderMother', 'Tile::GirderIron', 'Tile::GirderAluminium', 'Tile::GirderElectronics', 'Tile::Absorber', 'Tile::AbsorberChild', 'Tile::Period', 'Tile::Glue', 'Tile::Wrapper', 'Tile::EBarrel', 'Tile::EBarrelModule', 'Tile::EndPlateSh', 'Tile::ITC', 'Tile::ITCModule', 'Tile::Plug1Module', 'Tile::FrontPlateSh', 'Tile::Plug2Module', 'Tile::Gap', 'Tile::GapModule', 'Tile::IrUp', 'Tile::IrDw', 'Tile::Iron4', 'Tile::Iron3', 'Tile::Iron2', 'Tile::Iron1', 'Tile::IrBox', 'Tile::SaddleModule', 'Tile::LArService', 'Tile::LArCables', 'Tile::ExtBarrelSaddleSupport', 'Tile::Crack', 'Tile::CrackModule', 'Tile::FingerModule', 'Tile::FingerIron', 'Tile::FingerAluminum', 'Tile::FingerElectronics', 'Tile::EFinger', 'Tile::EFingerModule', 'Tile::FingerPos', 'Tile::FingerNeg', 'Tile::SaddlePos', 'Tile::SaddleNeg', 'Tile::ESaddlePos', 'Tile::ESaddleNeg' ] )
    kwargs.setdefault("OutputCollectionNames", ["TileHitVec", "TileCalibHitActiveCell", "TileCalibHitInactiveCell", "TileCalibHitDeadMaterial"] )
    kwargs.setdefault("TileCalculator", "TileCTBGeoG4SDCalc")
    
    result.setPrivateTools( TileGeoG4CalibSDTool(name, **kwargs) )
    return result
