#ifndef PartProduce_h
#define PartProduce_h 1

#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4AssemblyVolume.hh"
#include "BesTofGeoParameter.hh"

using namespace std;

class PartProduce
{
    public:
        PartProduce();
        ~PartProduce();

        void IniParam();

        G4VSolid* sd_containerOut(double smallL=0, string name="");
        G4VSolid* sd_containerIn(double smallL=0, string name="");
        G4LogicalVolume* lg_containerFrame(string name="");

        G4VSolid* sd_upCover(double smallL=0, string name="");
        G4VSolid* sd_lowCover1(double smallL=0, string name="");
        G4VSolid* sd_lowCover2(double smallL=0, string name="");
        G4LogicalVolume* lg_upCover(string name="");
        G4LogicalVolume* lg_lowCover1(string name="");
        G4LogicalVolume* lg_lowCover2(string name="");

        G4VSolid* sd_upFEE(double smallL=0, string name="");
        G4LogicalVolume* lg_upFEE_pcb(string name="");
        G4LogicalVolume* lg_upFEE(string name="");
        G4VSolid* sd_sideFEE(double smallL=0, string name="");
        G4LogicalVolume* lg_sideFEE_pcb(string name="");
        G4LogicalVolume* lg_sideFEE(string name="");

        G4VSolid* sd_castingDie_outline(double smallL=0, string name="");
        G4LogicalVolume* lg_castingDie(string name="");

        G4LogicalVolume* lg_container(int flag, string name="");

        G4LogicalVolume* lg_gasContainer(string name="");
        G4LogicalVolume* lg_bareChamber(string name="");
        G4LogicalVolume* lg_honeyComb(string name="");
        G4LogicalVolume* lg_tape_mrpc(string name="");
        G4LogicalVolume* lg_pcbBoard(G4int flag, string name="");
        G4AssemblyVolume* lg_strip(string name="");
        G4LogicalVolume* lg_mylar(string name="");
        G4LogicalVolume* lg_glass(G4int flag, string name="");
        G4LogicalVolume* lg_gasLayer(string name="");
        G4LogicalVolume* lg_bracket(string name="");
        G4LogicalVolume* lg_upStopBlock(string name="");
        G4LogicalVolume* lg_sideStopBlock(string name="");


    private:
        BesTofGeoParameter* tofPara;

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
};

#endif
