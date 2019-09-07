//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Dengzy
//Created: Mar, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//$ID: BesTofConstruction.hh

#ifndef BesTofConstruction_h
#define BesTofConstruction_h 1

#include "BesSubdetector.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "BesTofGeoParameter.hh"


class BesTofConstruction : public BesSubdetector
{
    public:
        BesTofConstruction();
        virtual ~BesTofConstruction();

    public:
        void Construct(G4LogicalVolume* logicalBes);
        void ConstructBr1Tof();
        void ConstructBr2Tof();
        void ConstructEcTof();
        void ConstructEcTof_mrpc();
        void DefineMaterial(); 
        void getXYZ( const G4RotationMatrix r, double& a, double& b, double& c) const;


    private:
        void IniParam_mrpc();
        BesTofGeoParameter* tofPara;

        G4LogicalVolume*   logicalTof;
        G4VPhysicalVolume* physicalTof;

        G4LogicalVolume*   logicalBrTof;
        G4LogicalVolume*   logicalEcTofWest;
        G4LogicalVolume*   logicalEcTofEast;

        //scintillator
        G4LogicalVolume*   logicalScinBr1;
        G4LogicalVolume*   logicalScinBr2;
        G4VPhysicalVolume* physicalScinBr1;
        G4VPhysicalVolume* physicalScinBr2;

        //casing material
        G4LogicalVolume*   logicalAlBr1;
        G4LogicalVolume*   logicalAlBr2;
        G4VPhysicalVolume* physicalAlBr1;
        G4VPhysicalVolume* physicalAlBr2;

        G4LogicalVolume*   logicalPVFBr1;
        G4LogicalVolume*   logicalPVFBr2;
        G4VPhysicalVolume* physicalPVFBr1;
        G4VPhysicalVolume* physicalPVFBr2;

        //bucket
        G4LogicalVolume*   logicalBucketBr1;
        G4LogicalVolume*   logicalBucketBr2;
        G4VPhysicalVolume* physicalBucket1Br1;
        G4VPhysicalVolume* physicalBucket2Br1;
        G4VPhysicalVolume* physicalBucket1Br2;
        G4VPhysicalVolume* physicalBucket2Br2;

        G4LogicalVolume*   logicalScinEcWest;
        G4LogicalVolume*   logicalScinEcEast;
        G4VPhysicalVolume* physicalScinEcWest;
        G4VPhysicalVolume* physicalScinEcEast;

        G4LogicalVolume*   logicalAlEcWest;
        G4LogicalVolume*   logicalAlEcEast;
        G4VPhysicalVolume*  physicalAlEcWest;
        G4VPhysicalVolume*  physicalAlEcEast;

        G4LogicalVolume*   logicalPVFEcWest;
        G4LogicalVolume*  logicalPVFEcEast;
        G4VPhysicalVolume*  physicalPVFEcWest;
        G4VPhysicalVolume*  physicalPVFEcEast;

        G4LogicalVolume*   logicalBucketEc;
        G4VPhysicalVolume* physicalBucketEc;

        G4Material* BC404;
        G4Material* BC408;
        G4Material* PVF;
        G4Material* PMTmaterial;


        //MRPC
        G4LogicalVolume* logical_gasLayer;

        G4double containerOut_xdown;
        G4double containerOut_xup;
        G4double containerOut_y;
        G4double containerOut_z;
        G4double container_thickness;
        G4double container_lowBorder;

        G4double containerIn_xdown;
        G4double containerIn_xup;
        G4double containerIn_y;
        G4double containerIn_z;

        G4double upCover_xdown;
        G4double upCover_xup;
        G4double upCover_y;
        G4double upCover_z;
        G4double upCover_posz;

        G4double lowCover1_xdown;
        G4double lowCover1_xup;
        G4double lowCover1_y;
        G4double lowCover1_z;
        G4double lowCover1_addx;
        G4double lowCover1_addz;
        G4double lowCover1_posz;

        G4double lowCover2_xdown;
        G4double lowCover2_xup;
        G4double lowCover2_y;
        G4double lowCover2_z;
        G4double lowCover2_posz;

        G4double upFEE_xdown;
        G4double upFEE_xup;
        G4double upFEE_y;
        G4double upFEE_z;
        G4double FEE_thickness;
        G4double upFEE_posz;

        G4double upFEE_pcb_xdown;
        G4double upFEE_pcb_xup;
        G4double upFEE_pcb_y;
        G4double upFEE_pcb_z;

        G4double sideFEE_x;
        G4double sideFEE_y;
        G4double sideFEE_z;
        G4double sideFEE_posl;

        G4double sideFEE_pcb_x;
        G4double sideFEE_pcb_y;
        G4double sideFEE_pcb_z;

        G4double castingDie_x;
        G4double castingDie_y;
        G4double castingDie_z;
        G4double castingDie_addr;
        G4double castingDie_subtractx;
        G4double castingDie_subtract_posz;
        G4double castingDie_posz1;
        G4double castingDie_posz2;
        G4double chamber_in_posy;
        G4double chamber_out_posy;

        G4double bracket_x;
        G4double bracket_y;
        G4double bracket_z;
        G4double bracket_r;
        G4double bracket_posx;

        G4double upStopBlock_x;
        G4double upStopBlock_y;
        G4double upStopBlock_z;
        G4double upStopBlock_subtractr;
        G4double upStopBlock_subtractposz;

        G4double sideStopBlock_x;
        G4double sideStopBlock_y;
        G4double sideStopBlock_z;
        G4double sideStopBlock_thickness;
        G4double sideStopBlock_posz0;
        G4double sideStopBlock_posz1;

        G4double honeyComb_xdown;
        G4double honeyComb_xup;
        G4double honeyComb_y;
        G4double honeyComb_z;
        G4double honeyComb_surface_y;
        G4double honeyComb_posz;

        G4double tape_mrpc_xdown;
        G4double tape_mrpc_xup;
        G4double tape_mrpc_y;
        G4double tape_mrpc_z;
        G4double tape_mrpc_posz;

        G4double pcbBoard_xdown;
        G4double pcbBoard_xup;
        G4double pcbBoard_y0;
        G4double pcbBoard_y1;
        G4double pcbBoard_z;

        G4double strip_x[12];
        G4double strip_y;
        G4double strip_z;
        G4double strip_gap;
        G4double strip_posz;

        G4double mylar_xdown;
        G4double mylar_xup;
        G4double mylar_y;
        G4double mylar_z;
        G4double mylar_posz;

        G4double glass0_xdown;
        G4double glass0_xup;
        G4double glass0_y;
        G4double glass0_z;
        G4double glass0_posz;

        G4double glass1_xdown;
        G4double glass1_xup;
        G4double glass1_y;
        G4double glass1_z;
        G4double glass1_posz;

        G4double gasLayer_xdown;
        G4double gasLayer_xup;
        G4double gasLayer_y;
        G4double gasLayer_z;
        G4double gasLayer_posz;

        G4double endcap_r;
        G4double endcap_length;
        G4double layer_posz;
        G4double startAngle_east;
        G4double startAngle_west;

        double m_userLimits;
        double m_ionE;

        //Adjusting parameters
        VEC rOffset_east;
        VEC angle_east;
        VEC angleOffset_east;
        VEC rOffset_west;
        VEC angle_west;
        VEC angleOffset_west;
};

#endif
