// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacePointsSeedMaker_ATLxk
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedMaker_ATLxk_H
#define SiSpacePointsSeedMaker_ATLxk_H

#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "BeamSpotConditionsData/BeamSpotData.h"
#include "SiSPSeededTrackFinderData/SiSpacePointForSeed.h"
#include "SiSPSeededTrackFinderData/SiSpacePointsSeedMakerEventData.h"
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MagField cache
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <iosfwd>
#include <list>
#include <vector>

class MsgStream;

namespace InDet {

  using EventData = SiSpacePointsSeedMakerEventData;

  /**
   * @class SiSpacePointsSeedMaker_ATLxk
   * Class for track candidates generation using space points information
   * for standard Atlas geometry
   *
   * In AthenaMT, event dependent cache inside SiSpacePointsSeedMaker_ATLxk
   * is not preferred. SiSpacePointsSeedMakerEventData = EventData class
   * holds event dependent data for SiSpacePointsSeedMaker_ATLxk.
   * Its object is instantiated in SiSPSeededTrackFinder::execute.
   */

  class SiSpacePointsSeedMaker_ATLxk : 
    public extends<AthAlgTool, ISiSpacePointsSeedMaker>
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    /// @name Standard tool methods
    ///////////////////////////////////////////////////////////////////
    //@{
    SiSpacePointsSeedMaker_ATLxk(const std::string&, const std::string&, const IInterface*);
    virtual ~SiSpacePointsSeedMaker_ATLxk() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    //@}

    ///////////////////////////////////////////////////////////////////
    /// @name Methods to initialize tool for new event or region
    ///////////////////////////////////////////////////////////////////
    //@{
    virtual void newEvent (const EventContext& ctx, EventData& data, int iteration) const override;
    virtual void newRegion(const EventContext& ctx, EventData& data,
                           const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT) const override;
    virtual void newRegion(const EventContext& ctx,SiSpacePointsSeedMakerEventData& data,
                           const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT,
                           const IRoiDescriptor& iRD) const override;
    //@}

    ///////////////////////////////////////////////////////////////////
    /// @name Methods to initilize different strategies of seeds production
    ///////////////////////////////////////////////////////////////////
    //@{

    /// With two space points with or without vertex constraint
    virtual void find2Sp(EventData& data, const std::list<Trk::Vertex>& lv) const override;

    /// with three space points with or without vertex constraint
    virtual void find3Sp(const EventContext& ctx, EventData& data, const std::list<Trk::Vertex>& lv) const override;

    /// with three space points with or without vertex constraint
    /// with information about min and max Z of the vertex
    virtual void find3Sp(const EventContext& ctx, EventData& data, const std::list<Trk::Vertex>& lv, const double* zVertex) const override;

    /// with variable number space points with or without vertex constraint
    /// Variable means (2,3,4,....) any number space points
    virtual void findVSp(const EventContext& ctx, EventData& data, const std::list<Trk::Vertex>& lv) const override;
    //@}

    ///////////////////////////////////////////////////////////////////
    /// @name Iterator through seeds pseudo collection
    /// produced accordingly methods find    
    ///////////////////////////////////////////////////////////////////
    //@{
    /** This method will update the data.seedOutput member to be the next seed pointed at by 
    * the data.i_seed_Pro iterator over the data.l_seeds_Pro list. 
    * Some poor quality PPS seeds will be skipped, this cut is implemented within the SiSpacePointsSeed::set3 method. 
    * If we run out of seeds after having previously reached a premature abort condition, 
    * seed finding will automatically be continued until all seeds have been found. 
    * @param[in] ctx: Event contex
    * @param[in,out] data Event data, updated and used to obtain the next seed to return 
    **/ 
    virtual const SiSpacePointsSeed* next(const EventContext& ctx, EventData& data) const override;
    //@}
      
    ///////////////////////////////////////////////////////////////////
    /// @name Print internal tool parameters and status
    ///////////////////////////////////////////////////////////////////
    //@{
    virtual MsgStream& dump(EventData& data, MsgStream& out) const override;
    //@}

  private:
    /// enum for array sizes
    /// Note that this stores the maximum capacities, the actual binnings 
    /// do not always use the full size. See data members below for the 
    /// actual binning paramaters, which are determined in buildFramework. 
    //@{
    enum Size {arraySizePhi=53,     ///< capacity of the 1D phi arrays 
               arraySizeZ=11,       ///< capacity of the 1D z arrays
               arraySizePhiZ=arraySizePhi*arraySizeZ,   ///< capacity for the 2D phi-z arrays 
               arraySizeNeighbourBins=9,  ///< array size to store neighbouring phi-z-regions in the seed finding
               arraySizePhiV=100,         ///< array size in phi for vertexing 
               arraySizeZV=3,             ///< array size in z for vertexing
               arraySizePhiZV=arraySizePhiV*arraySizeZV,      ///< array size in phi-Z 2D for the vertexing
               arraySizeNeighbourBinsVertex=6};       ///< array size to store neighbouring phi-z regions for the vertexing
    //@} 
    ///////////////////////////////////////////////////////////////////
    /// Private data and methods
    ///////////////////////////////////////////////////////////////////

    /// @name Data handles
    //@{
    SG::ReadHandleKey<SpacePointContainer> m_spacepointsPixel{this, "SpacePointsPixelName", "PixelSpacePoints", "Pixel space points container"};
    SG::ReadHandleKey<SpacePointContainer> m_spacepointsSCT{this, "SpacePointsSCTName", "SCT_SpacePoints", "SCT space points container"};
    SG::ReadHandleKey<SpacePointOverlapCollection> m_spacepointsOverlap{this, "SpacePointsOverlapName", "OverlapSpacePoints"};
    SG::ReadHandleKey<Trk::PRDtoTrackMap> m_prdToTrackMap{this,"PRDtoTrackMap","","option PRD-to-track association"};
    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey{this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};
    /// Read handle for conditions object to get the field cache
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj",
                                                                          "Name of the Magnetic Field conditions object key"};
    //@}

    /// @name Properties, which will not be changed after construction
    //@{
    BooleanProperty m_pixel{this, "usePixel", true};
    BooleanProperty m_sct{this, "useSCT", true};
    BooleanProperty m_dbm{this, "useDBM", false};
    BooleanProperty m_useOverlap{this, "useOverlapSpCollection", true};
    IntegerProperty m_maxsize{this, "maxSize", 50000};
    IntegerProperty m_maxsizeSP{this, "maxSizeSP", 5000};
    IntegerProperty m_maxOneSize{this, "maxSeedsForSpacePoint", 5};
    FloatProperty m_etamax{this, "etaMax", 2.7};
    FloatProperty m_r1minv{this, "minVRadius1", 0.};
    FloatProperty m_r1maxv{this, "maxVRadius1", 60.};
    FloatProperty m_r2minv{this, "minVRadius2", 70.};
    FloatProperty m_r2maxv{this, "maxVRadius2", 200.};
    FloatProperty m_drmax{this, "maxdRadius", 270.};
    FloatProperty m_zmin{this, "minZ", -250.};
    FloatProperty m_zmax{this, "maxZ", +250.};
    FloatProperty m_dzver{this, "maxdZver", 5.};
    FloatProperty m_dzdrver{this, "maxdZdRver", .02};
    //@}

    /// @name Properties, which can be updated in buildFrameWork method which is called in initialize method
    //@{
    FloatProperty m_etamin{this, "etaMin", 0.};
    FloatProperty m_r_rmax{this, "radMax", 600.};
    FloatProperty m_binSizeR{this, "radStep", 2.};
    FloatProperty m_r3max{this, "maxRadius3", 600.}; ///< This is always overwritten by m_r_rmax.
    FloatProperty m_drmin{this, "mindRadius", 5.};
    FloatProperty m_maxdImpact{this, "maxdImpact", 10.};
    FloatProperty m_maxdImpactSSS{this, "maxdImpactSSS", 50.};
    FloatProperty m_maxdImpactDecays{this, "maxdImpactForDecays", 20.};
    FloatProperty m_ptmin{this, "pTmin", 500.};
    //@}

    /// @name Properties, which can be updated in newEvent method. checketa is prepared in EventData.
    //@{
    BooleanProperty m_checketa{this, "checkEta", false};
    //@}

    /// @name Properties, which are not used in this implementation of SiSpacePointsSeedMaker_ATLxk class
    //@{
    UnsignedIntegerProperty m_maxNumberVertices{this, "maxNumberVertices", 99};
    FloatProperty m_r1min{this, "minRadius1", 0.};
    FloatProperty m_r1max{this, "maxRadius1", 600.};
    FloatProperty m_r2min{this, "minRadius2", 0.};
    FloatProperty m_r2max{this, "maxRadius2", 600.};
    FloatProperty m_r3min{this, "minRadius3", 0.};
    FloatProperty m_rapcut{this, "RapidityCut", 2.7};
    FloatProperty m_maxdImpactPPS{this, "maxdImpactPPS", 1.7};
    //@}

    /// @name Data member, which is not updated at all.
    //@{
    float m_drminv{20.};
    //@}

    /// @name Data members, which are updated only in buildFrameWork in initialize
    //@{
    /// conversion factors and cached cut values
    float m_dzdrmin0{0.};   ///< implicitly store eta cut
    float m_dzdrmax0{0.};   ///< implicitly store eta cut
    float m_ipt{0.};    ///< inverse of 90% of the ptmin cut 
    float m_ipt2{0.};   ///< inverse square of 90% of the pt min cut 
    static constexpr float m_COF{134*.05*9};    ///< conversion factor. A very magic number indeed. 

    /// @name Binning parameters 
    ///@{
    int m_nBinsR{0};              ///<  number of bins in the radial coordinate 
    int m_maxPhiBin{0};           ///<  number of bins in phi 
    int m_maxBinPhiVertex{0};     ///<  number of bins in phi for vertices 
    float m_inverseBinSizePhi{0};   ///<  cache the inverse bin size in phi which we use - needed to evaluate phi bin locations
    float m_inverseBinSizePhiVertex{0};///<  as above but for vertex
    ///@}

    /// arrays associating bins to each other for SP formation
    std::array<int,arraySizePhiZ> m_nNeighbourCellsBottom;  ///< number of neighbouring phi-z bins to consider when looking for "bottom SP" candidates for each phi-z bin
    std::array<int,arraySizePhiZ> m_nNeighbourCellsTop;  ///< number of neighbouring phi-z bins to consider when looking for "top SP" candidates for each phi-z bin
    std::array<std::array<int, arraySizeNeighbourBins>, arraySizePhiZ> m_neighbourCellsBottom; ///< mapping of neighbour cells in the 2D phi-z binning to consider  for the "bottom SP" search for central SPs in each phi-z bin. Number of valid entries stored in m_nNeighboursPhiZbottom
    std::array<std::array<int, arraySizeNeighbourBins>, arraySizePhiZ> m_neighbourCellsTop; ///< mapping of neighbour cells in the 2D phi-z binning to consider  for the "top SP" search for central SPs in each phi-z bin. Number of valid entries stored in m_nNeighboursPhiZtop

    std::array<int,arraySizePhiZV> m_nNeighboursVertexPhiZ;
    std::array<std::array<int, arraySizeNeighbourBinsVertex>, arraySizePhiZ> m_neighboursVertexPhiZ;
    //@}

    ///////////////////////////////////////////////////////////////////
    // Private methods
    ///////////////////////////////////////////////////////////////////

    /// @name Disallow default instantiation, copy, assignment
    //@{
    SiSpacePointsSeedMaker_ATLxk() = delete;
    SiSpacePointsSeedMaker_ATLxk(const SiSpacePointsSeedMaker_ATLxk&) = delete;
    SiSpacePointsSeedMaker_ATLxk &operator=(const SiSpacePointsSeedMaker_ATLxk&) = delete;
    //@}

    MsgStream& dumpConditions(EventData& data, MsgStream& out) const;
    MsgStream& dumpEvent     (EventData& data, MsgStream& out) const;

    /// prepare several data members with cached cut values,
    /// conversion factors, binnings, etc 
    void buildFrameWork();
    /* updates the beam spot information stored in the event data
    * object. 
    * @param[out] data: Event data, receives update to the x/y/zbeam members 
    **/
    void buildBeamFrameWork(EventData& data) const;

    /** Create a SiSpacePointForSeed from the space point. 
    * This will also add the point to the data object's
    * l_spforseed list and update its i_spforseed iterator 
    * to point to the entry after the new SP 
    * for further additions.
    * Returns a nullptr if the SP fails the eta cut, 
    * should we apply one 
    * @param[in,out] data: Provides beam spot location, receives updates to the l_spforseed and i_spforseed members 
    * @param[in] sp: Input space point. 
    **/
    SiSpacePointForSeed* newSpacePoint(EventData& data, const Trk::SpacePoint*const& sp) const;

    void newSeed(EventData& data, SiSpacePointForSeed*& p1, SiSpacePointForSeed*& p2, float z) const;

    /** This inserts a seed into the set of saved seeds. 
    * It internally respects the user-configured max number of seeds per central 
    * space point. Once this is exceeded, the new seed will replace worse-quality 
    * seeds if there are any, otherwise it will not insert anything. 
    * @param[in,out] data Event data - update OneSeeds_Pro and mapOneSeeds_Pro members 
    * @param[in] p1 First space point for this seed
    * @param[in] p2 Second space point for this seed
    * @param[in] p3 Third space point for this seed
    * @param[in] z z0 IP estimate
    * @param[in] q quality estimate (based on d0, plus modifiers) 
    **/ 
    void newOneSeed(EventData& data,
                    SiSpacePointForSeed*& p1, SiSpacePointForSeed*& p2,
                    SiSpacePointForSeed*& p3, float z, float q) const;

    /** This creates all possible seeds with the passed central and bottom SP, using all top SP 
    * candidates which are stored in the data.CmSp member.  Seeds are scored by a quality score 
    * seeded by abs(d0), and modified if there is a second-seed confirmation or in case of PPP/SSS 
    * topologies. Then, they are written out via the newOneSeed method.
    * @param[in,out] data Event data, used to read top SP candidates and write out found seeds (see newOneSeed). 
    * @param[in] SPb Bottom Space point for the seed creation
    * @param[in] SP0 Central Space point for the seed creation
    * @param[in] Zob z0 estimate 
    **/ 
    void newOneSeedWithCurvaturesComparison
    (EventData& data, SiSpacePointForSeed*& SPb, SiSpacePointForSeed*& SP0, float Zob) const;

    /// fills the seeds from the mapOneSeeds_Pro member into the l_seeds_Pro member of the data object, applying some more 
    /// quality requirements on the way. 
    /// @param[in,out] data Event data which is modified
    void fillSeeds(EventData& data) const;

    /** this method populates the data object's "histograms" (implemented as nested vectors). 
    * using the list of r-binned space points in the object assumed to have been previously
    * set (for example via the newEvent method of this class). 
    * @param[in,out] data: Event data which will be updated. 
    **/ 
    void fillLists(EventData& data) const;
    void erase(EventData& data) const;
    void production2Sp(EventData& data) const;


    /** Top-level method for 3-SP seed production. 
    * This method loops over each eta-Z region, and in each region 
    * calls the extended production3Sp method below to do the actual work. 
    * @param[in,out] data Event data which will be updated
    **/
    void production3Sp(EventData& data) const;

    /** \brief: Seed production from space points. 
    * 
    * This method will try to find 3-SP combinations within a 
    * local phi-z region in the detector. 
    * 
    * The central SP of the seed will be taken from this region
    * (technically via the first entry of the bottom candidate array, 
    * which always points to the phi-z bin of interest itself). 
    * 
    * The top SP is allowed to come from the same or one of several close-by
    * phi-Z bins, as is the bottom SP. 
    * 
    * All SP collections are expected to be internally sorted in the radial coordinate.
    * 
    * @param[in,out] data: Event data
    * @param[in,out] iter_bottomCands: collection of iterators over SP collections for up to 9 phi-z cells to consider for the bottom space-point search 
    * @param[in,out] iter_endBottomCands: collection of end-iterators over the 
    * SP collections  for up to 9 phi-z cells to consider for the bottom space-point search 
    * @param[in,out] iter_topCands: collection of iterators over SP collections for up to 9 phi-z cells to consider for the top space-point search 
    * @param[in,out] iter_endTopCands: collection of end-iterators over the 
    * SP collections  for up to 9 phi-z cells to consider for the top space-point search 
    * @param[in] numberBottomCells: Number of bottom cells to consider. Determines how many entries in iter_(end)bottomCands are expected to be valid. 
    * @param[in] numberTopCells: Number of top cells to consider.Determines how many entries in iter_(end)topCands are expected to be valid. 
    * @param[out] nseed: Number of seeds found 
    **/ 
    void production3Sp
    (EventData& data,
     std::array<std::vector<InDet::SiSpacePointForSeed*>::iterator, arraySizeNeighbourBins> & iter_bottomCands,
     std::array<std::vector<InDet::SiSpacePointForSeed*>::iterator, arraySizeNeighbourBins> & iter_endBottomCands,
     std::array<std::vector<InDet::SiSpacePointForSeed*>::iterator, arraySizeNeighbourBins> & iter_topCands,
     std::array<std::vector<InDet::SiSpacePointForSeed*>::iterator, arraySizeNeighbourBins> & iter_endTopCands,
     const int numberBottomCells, const int numberTopCells, int& nseed) const;

    /// as above, but for the trigger 
    void production3SpTrigger
    (EventData& data,
     std::array<std::vector<InDet::SiSpacePointForSeed*>::iterator, arraySizeNeighbourBins> & rb,
     std::array<std::vector<InDet::SiSpacePointForSeed*>::iterator, arraySizeNeighbourBins> & rbe,
     std::array<std::vector<InDet::SiSpacePointForSeed*>::iterator, arraySizeNeighbourBins> & rt,
     std::array<std::vector<InDet::SiSpacePointForSeed*>::iterator, arraySizeNeighbourBins> & rte,
     const int numberBottomCells, const int numberTopCells, int& nseed) const;
 
    /** This method updates the EventData based on the passed list of vertices. 
    * The list may be empty. 
    * Updates the isvertex, l_vertex, zminU and zmaxU members of the data object.
    * Always returns false.  
    * @param[in,out] data  Event data to update 
    * @param[in] lV possibly empty list of vertices
    **/
    bool newVertices(EventData& data, const std::list<Trk::Vertex>& lV) const;
    /** This method is called within next() when we are out of vertices. 
     * It will internally trigger a re-run of production3Sp if we are out of seeds 
     * and data.endlist is not set (indicating the search is not finished). 
    **/ 
    void findNext(EventData& data) const;
    bool isZCompatible(EventData& data, const float& Zv, const float& R, const float& T) const;

    /** This method popualtes the r array 
    * with the space point's coordinates 
    * relative to the beam spot. 
    * @param[in] data Event data 
    * @param[in] sp: Space point to take the global position from 
    * @param[out] r: 3-array, will be populated with the relative coordinates 
    **/
    void convertToBeamFrameWork(EventData& data, const Trk::SpacePoint*const& sp, std::array<float,3> & r) const;

    bool isUsed(const Trk::SpacePoint* sp, const Trk::PRDtoTrackMap &prd_to_track_map) const;

    void initializeEventData(EventData& data) const;
  };
  
} // end of name space

///////////////////////////////////////////////////////////////////
// Object-function for curvature seeds comparison
///////////////////////////////////////////////////////////////////

class comCurvature {
public:
  bool operator ()
  (const std::pair<float,InDet::SiSpacePointForSeed*>& i1, 
   const std::pair<float,InDet::SiSpacePointForSeed*>& i2)
  {
    return i1.first < i2.first;
  }
};

namespace InDet {

  inline
  bool SiSpacePointsSeedMaker_ATLxk::isUsed(const Trk::SpacePoint* sp, const Trk::PRDtoTrackMap &prd_to_track_map) const
  {
    const Trk::PrepRawData* d = sp->clusterList().first;
    if (!d || !prd_to_track_map.isUsed(*d)) return false;
    d = sp->clusterList().second;
    if (!d || prd_to_track_map.isUsed(*d)) return true;
    return false;
  }
}

#endif // SiSpacePointsSeedMaker_ATLxk_H
