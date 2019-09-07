#include "PartProduce.hh"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Trd.hh"
#include "G4Box.hh"
#include "G4Transform3D.hh"
#include "G4UnionSolid.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4VisAttributes.hh"

using namespace std;

G4double smallL = 0.01*mm;
G4RotationMatrix rot_dummy(0*deg,0*deg,0*deg);
bool checkOverlap = false;
std::ostringstream Name;


PartProduce::PartProduce()
{	
    IniParam();
}

PartProduce::~PartProduce()
{
}

void PartProduce::IniParam()
{
    tofPara = BesTofGeoParameter::GetInstance();

    //All the values are in units of mm or deg
    containerOut_xdown = tofPara->Get("containerOut_xdown");
    containerOut_xup = tofPara->Get("containerOut_xup");
    containerOut_y = tofPara->Get("containerOut_y");
    containerOut_z = tofPara->Get("containerOut_z");
    container_thickness = tofPara->Get("container_thickness");
    container_lowBorder = tofPara->Get("container_lowBorder");

    containerIn_xdown = tofPara->Get("containerIn_xdown");
    containerIn_xup = tofPara->Get("containerIn_xup");
    containerIn_y = tofPara->Get("containerIn_y");
    containerIn_z = tofPara->Get("containerIn_z");

    upCover_xdown = tofPara->Get("upCover_xdown");
    upCover_xup = tofPara->Get("upCover_xup");
    upCover_y = tofPara->Get("upCover_y");
    upCover_z = tofPara->Get("upCover_z");
    upCover_posz = tofPara->Get("upCover_posz");

    lowCover1_xdown = tofPara->Get("lowCover1_xdown");
    lowCover1_xup = tofPara->Get("lowCover1_xup");
    lowCover1_y = tofPara->Get("lowCover1_y");
    lowCover1_z = tofPara->Get("lowCover1_z");
    lowCover1_addx = tofPara->Get("lowCover1_addx");
    lowCover1_addz = tofPara->Get("lowCover1_addz");
    lowCover1_posz = tofPara->Get("lowCover1_posz");

    lowCover2_xdown = tofPara->Get("lowCover2_xdown");
    lowCover2_xup = tofPara->Get("lowCover2_xup");
    lowCover2_y = tofPara->Get("lowCover2_y");
    lowCover2_z = tofPara->Get("lowCover2_z");
    lowCover2_posz = tofPara->Get("lowCover2_posz");

    upFEE_xdown = tofPara->Get("upFEE_xdown");
    upFEE_xup = tofPara->Get("upFEE_xup");
    upFEE_y = tofPara->Get("upFEE_y");
    upFEE_z = tofPara->Get("upFEE_z");
    FEE_thickness = tofPara->Get("FEE_thickness");
    upFEE_posz = tofPara->Get("upFEE_posz");

    upFEE_pcb_xdown = tofPara->Get("upFEE_pcb_xdown");
    upFEE_pcb_xup = tofPara->Get("upFEE_pcb_xup");
    upFEE_pcb_y = tofPara->Get("upFEE_pcb_y");
    upFEE_pcb_z = tofPara->Get("upFEE_pcb_z");

    sideFEE_x = tofPara->Get("sideFEE_x");
    sideFEE_y = tofPara->Get("sideFEE_y");
    sideFEE_z = tofPara->Get("sideFEE_z");
    sideFEE_posl = tofPara->Get("sideFEE_posl");

    sideFEE_pcb_x = tofPara->Get("sideFEE_pcb_x");
    sideFEE_pcb_y = tofPara->Get("sideFEE_pcb_y");
    sideFEE_pcb_z = tofPara->Get("sideFEE_pcb_z");

    castingDie_x = tofPara->Get("castingDie_x");
    castingDie_y = tofPara->Get("castingDie_y");
    castingDie_z = tofPara->Get("castingDie_z");
    castingDie_addr = tofPara->Get("castingDie_addr");
    castingDie_subtractx = tofPara->Get("castingDie_subtractx");
    castingDie_subtract_posz = tofPara->Get("castingDie_subtract_posz");
    castingDie_posz1 = tofPara->Get("castingDie_posz1");
    castingDie_posz2 = tofPara->Get("castingDie_posz2");

    chamber_in_posy = tofPara->Get("chamber_in_posy");
    chamber_out_posy = tofPara->Get("chamber_out_posy");

    bracket_x = tofPara->Get("bracket_x");
    bracket_y = tofPara->Get("bracket_y");
    bracket_z = tofPara->Get("bracket_z");
    bracket_r = tofPara->Get("bracket_r");
    bracket_posx = tofPara->Get("bracket_posx");

    upStopBlock_x = tofPara->Get("upStopBlock_x");
    upStopBlock_y = tofPara->Get("upStopBlock_y");
    upStopBlock_z = tofPara->Get("upStopBlock_z");
    upStopBlock_subtractr = tofPara->Get("upStopBlock_subtractr");
    upStopBlock_subtractposz = tofPara->Get("upStopBlock_subtractposz");

    sideStopBlock_x = tofPara->Get("sideStopBlock_x");
    sideStopBlock_y = tofPara->Get("sideStopBlock_y");
    sideStopBlock_z = tofPara->Get("sideStopBlock_z");
    sideStopBlock_thickness = tofPara->Get("sideStopBlock_thickness");
    sideStopBlock_posz0 = tofPara->Get("sideStopBlock_posz0");
    sideStopBlock_posz1 = tofPara->Get("sideStopBlock_posz1");


    honeyComb_xdown = tofPara->Get("honeyComb_xdown");
    honeyComb_xup = tofPara->Get("honeyComb_xup");
    honeyComb_y = tofPara->Get("honeyComb_y");
    honeyComb_z = tofPara->Get("honeyComb_z");
    honeyComb_surface_y = tofPara->Get("honeyComb_surface_y");
    honeyComb_posz = tofPara->Get("honeyComb_posz");

    tape_mrpc_xdown = tofPara->Get("tape_mrpc_xdown");
    tape_mrpc_xup = tofPara->Get("tape_mrpc_xup");
    tape_mrpc_y = tofPara->Get("tape_mrpc_y");
    tape_mrpc_z = tofPara->Get("tape_mrpc_z");
    tape_mrpc_posz = tofPara->Get("tape_mrpc_posz");

    pcbBoard_xdown = tofPara->Get("pcbBoard_xdown");
    pcbBoard_xup = tofPara->Get("pcbBoard_xup");
    pcbBoard_y0 = tofPara->Get("pcbBoard_y0");
    pcbBoard_y1 = tofPara->Get("pcbBoard_y1");
    pcbBoard_z = tofPara->Get("pcbBoard_z");

    strip_x[0] = tofPara->Get("strip_x[0]");
    strip_x[1] = tofPara->Get("strip_x[1]");
    strip_x[2] = tofPara->Get("strip_x[2]");
    strip_x[3] = tofPara->Get("strip_x[3]");
    strip_x[4] = tofPara->Get("strip_x[4]");
    strip_x[5] = tofPara->Get("strip_x[5]");
    strip_x[6] = tofPara->Get("strip_x[6]");
    strip_x[7] = tofPara->Get("strip_x[7]");
    strip_x[8] = tofPara->Get("strip_x[8]");
    strip_x[9] = tofPara->Get("strip_x[9]");
    strip_x[10] = tofPara->Get("strip_x[10]");
    strip_x[11] = tofPara->Get("strip_x[11]");
    strip_y = tofPara->Get("strip_y");
    strip_z = tofPara->Get("strip_z");
    strip_gap = tofPara->Get("strip_gap");
    strip_posz = tofPara->Get("strip_posz");

    mylar_xdown = tofPara->Get("mylar_xdown");
    mylar_xup = tofPara->Get("mylar_xup");
    mylar_y = tofPara->Get("mylar_y");
    mylar_z = tofPara->Get("mylar_z");
    mylar_posz = tofPara->Get("mylar_posz");

    glass0_xdown = tofPara->Get("glass0_xdown");
    glass0_xup = tofPara->Get("glass0_xup");
    glass0_y = tofPara->Get("glass0_y");
    glass0_z = tofPara->Get("glass0_z");
    glass0_posz = tofPara->Get("glass0_posz");

    glass1_xdown = tofPara->Get("glass1_xdown");
    glass1_xup = tofPara->Get("glass1_xup");
    glass1_y = tofPara->Get("glass1_y");
    glass1_z = tofPara->Get("glass1_z");
    glass1_posz = tofPara->Get("glass1_posz");

    gasLayer_xdown = tofPara->Get("gasLayer_xdown");
    gasLayer_xup = tofPara->Get("gasLayer_xup");
    gasLayer_y = tofPara->Get("gasLayer_y");
    gasLayer_z = tofPara->Get("gasLayer_z");
    gasLayer_posz = tofPara->Get("gasLayer_posz");

    endcap_r = tofPara->Get("endcap_r");
    endcap_length = tofPara->Get("endcap_length");
    layer_posz = tofPara->Get("layer_posz");
    startAngle_east = tofPara->Get("startAngle_east");
    startAngle_west = tofPara->Get("startAngle_west");
}


G4VSolid* PartProduce::sd_containerOut(double smallL, string name)
{
    G4Trd* solid_containerOut = new G4Trd(name, containerOut_xdown/2+smallL, containerOut_xup/2+smallL, containerOut_y/2+smallL, containerOut_y/2+smallL, containerOut_z/2+smallL);
    return solid_containerOut;
}

G4VSolid* PartProduce::sd_containerIn(double smallL, string name)
{
    G4Trd* solid_containerIn = new G4Trd(name, containerIn_xdown/2+smallL, containerIn_xup/2+smallL, containerIn_y/2+smallL, containerIn_y/2+smallL, containerIn_z/2+smallL);
    return solid_containerIn;
}

G4LogicalVolume* PartProduce::lg_containerFrame(string name)
{
    Name.str("");
    Name<<"solid_containerOut_"<<name;
    G4VSolid* solid_containerOut = sd_containerOut(0, Name.str());
    Name.str("");
    Name<<"solid_containerIn_"<<name;
    G4VSolid* solid_containerIn = sd_containerIn(smallL, Name.str());

    double lz = (container_lowBorder-container_thickness)/2;
    G4ThreeVector translation_containerFrame(0,0,lz);
    G4Transform3D transform_containerFrame(rot_dummy, translation_containerFrame);
    Name.str("");
    Name<<"solid_containerFrame_"<<name;
    G4SubtractionSolid* solid_containerFrame = new G4SubtractionSolid(Name.str(), solid_containerOut, solid_containerIn, transform_containerFrame);
    G4LogicalVolume* logical_containerFrame = new G4LogicalVolume(solid_containerFrame, G4Material::GetMaterial("Aluminium"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(0.5,0.5,0.6));
    logical_containerFrame->SetVisAttributes(vis);

    return logical_containerFrame;
}

G4VSolid* PartProduce::sd_upCover(double smallL, string name)
{
    G4Trd* solid_upCover = new G4Trd(name, upCover_xdown/2+smallL, upCover_xup/2+smallL, upCover_y/2+smallL, upCover_y/2+smallL, upCover_z/2+smallL);
    return solid_upCover;
}

G4VSolid* PartProduce::sd_lowCover1(double smallL, string name)
{
    Name.str("");
    Name<<name<<"_1";
    G4Trd* solid_lowCover1_1 = new G4Trd(Name.str(), lowCover1_xdown/2+smallL, lowCover1_xup/2+smallL, lowCover1_y/2+smallL, lowCover1_y/2+smallL, lowCover1_z/2+smallL);

    Name.str("");
    Name<<name<<"_2";
    G4Trd* solid_lowCover1_2 = new G4Trd(Name.str(), lowCover1_addx/2+smallL, lowCover1_addx/2+smallL, lowCover1_y/2+smallL, lowCover1_y/2+smallL, lowCover1_addz/2+smallL);

    G4Transform3D transform_lowCover1(rot_dummy, G4ThreeVector(0,0,-(lowCover1_z+lowCover1_addz)/2));
    G4UnionSolid* solid_lowCover1 = new G4UnionSolid(name, solid_lowCover1_1, solid_lowCover1_2, transform_lowCover1);
    return solid_lowCover1;
}

G4VSolid* PartProduce::sd_lowCover2(double smallL, string name)
{
    G4Trd* solid_lowCover2 = new G4Trd(name, lowCover2_xdown/2+smallL, lowCover2_xup/2+smallL, lowCover2_y/2+smallL, lowCover2_y/2+smallL, lowCover2_z/2+smallL);
    return solid_lowCover2;
}

G4LogicalVolume* PartProduce::lg_upCover(string name)
{
    Name.str("");
    Name<<"solid_upCover_"<<name;
    G4VSolid* solid_upCover = sd_upCover(0, Name.str());
    G4LogicalVolume* logical_upCover = new G4LogicalVolume(solid_upCover, G4Material::GetMaterial("Aluminium"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(0.5,0.5,0.6));
    //vis->SetForceWireframe(true);     
    logical_upCover->SetVisAttributes(vis);

    return logical_upCover;
}

G4LogicalVolume* PartProduce::lg_lowCover1(string name)
{
    Name.str("");
    Name<<"solid_lowCover1_"<<name;
    G4VSolid* solid_lowCover1 = sd_lowCover1(0, Name.str());
    G4LogicalVolume* logical_lowCover1 = new G4LogicalVolume(solid_lowCover1, G4Material::GetMaterial("Aluminium"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(0.5,0.5,0.6));
    //vis->SetForceWireframe(true);     
    logical_lowCover1->SetVisAttributes(vis);

    return logical_lowCover1;
}

G4LogicalVolume* PartProduce::lg_lowCover2(string name)
{
    Name.str("");
    Name<<"solid_lowCover2_"<<name;
    G4VSolid* solid_lowCover2 = sd_lowCover2(0, Name.str());
    G4LogicalVolume* logical_lowCover2 = new G4LogicalVolume(solid_lowCover2, G4Material::GetMaterial("Aluminium"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(0.5,0.5,0.6));
    //vis->SetForceWireframe(true);     
    logical_lowCover2->SetVisAttributes(vis);

    return logical_lowCover2;
}

G4VSolid* PartProduce::sd_upFEE(double smallL, string name)
{
    G4Trd* solid_upFEE = new G4Trd(name, upFEE_xdown/2+smallL, upFEE_xup/2+smallL, upFEE_y/2+smallL, upFEE_y/2+smallL, upFEE_z/2+smallL);
    return solid_upFEE;
}

G4LogicalVolume* PartProduce::lg_upFEE_pcb(string name)
{
    Name.str("");
    Name<<"solid_upFEE_pcb_"<<name;
    G4Trd* solid_upFEE_pcb = new G4Trd(Name.str(), upFEE_pcb_xdown/2, upFEE_pcb_xup/2, upFEE_pcb_y/2, upFEE_pcb_y/2, upFEE_pcb_z/2);
    G4LogicalVolume* logical_upFEE_pcb = new G4LogicalVolume(solid_upFEE_pcb, G4Material::GetMaterial("insulationBoard"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetForceWireframe(true);
    vis->SetColor(G4Color(0.2, 1, 0));
    logical_upFEE_pcb->SetVisAttributes(vis);

    return logical_upFEE_pcb;
}

G4LogicalVolume* PartProduce::lg_upFEE(string name)
{
    Name.str("");
    Name<<"solid_upFEE_"<<name;
    G4VSolid* solid_upFEE = sd_upFEE(0, Name.str());
    G4LogicalVolume* logical_upFEE = new G4LogicalVolume(solid_upFEE, G4Material::GetMaterial("Air"), name);

    Name.str("");
    Name<<"solid_upFEE_out_"<<name;
    G4VSolid* solid_upFEE_out = sd_upFEE(-1*smallL, Name.str());

    Name.str("");
    Name<<"solid_upFEE_in_"<<name;
    G4Trd* solid_upFEE_in = new G4Trd(Name.str(), upFEE_xdown/2-FEE_thickness, upFEE_xup/2-FEE_thickness, upFEE_y/2, upFEE_y/2, upFEE_z/2-FEE_thickness);

    Name.str("");
    Name<<"solid_upFEE_box_"<<name;
    G4Transform3D transform_upFEE(rot_dummy, G4ThreeVector(0, -1*FEE_thickness, 0));
    G4SubtractionSolid* solid_upFEE_box = new G4SubtractionSolid(Name.str(), solid_upFEE_out, solid_upFEE_in, transform_upFEE);

    Name.str("");
    Name<<"logical_upFEE_box_"<<name;
    G4LogicalVolume* logical_upFEE_box = new G4LogicalVolume(solid_upFEE_box, G4Material::GetMaterial("Aluminium"), Name.str());

    Name.str("");
    Name<<"logical_upFEE_pcb_"<<name;
    G4LogicalVolume* logical_upFEE_pcb = lg_upFEE_pcb(Name.str());

    Name.str("");
    Name<<"physical_upFEE_pcb_"<<name;
    new G4PVPlacement(0, G4ThreeVector(0,0,0), logical_upFEE_pcb, Name.str(), logical_upFEE, false, 0, checkOverlap);

    Name.str("");
    Name<<"physical_upFEE_box_"<<name;
    new G4PVPlacement(0, G4ThreeVector(0,0,0), logical_upFEE_box, Name.str(), logical_upFEE, false, 0, checkOverlap);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(0.2,0.,1));
    //vis->SetForceWireframe(true);     
    logical_upFEE_box->SetVisAttributes(vis);

    G4VisAttributes* vis1 = new G4VisAttributes();
    vis1->SetVisibility(0);
    logical_upFEE->SetVisAttributes(vis1);

    return logical_upFEE;
}


G4VSolid* PartProduce::sd_sideFEE(double smallL, string name)
{
    G4Trd* solid_sideFEE = new G4Trd(name, sideFEE_x/2+smallL, sideFEE_x/2+smallL, sideFEE_y/2+smallL, sideFEE_y/2+smallL, sideFEE_z/2+smallL);
    return solid_sideFEE;
}

G4LogicalVolume* PartProduce::lg_sideFEE_pcb(string name)
{
    Name.str("");
    Name<<"solid_sideFEE_pcb_"<<name;
    G4Trd* solid_sideFEE_pcb = new G4Trd(Name.str(), sideFEE_pcb_x/2, sideFEE_pcb_x/2, sideFEE_pcb_y/2, sideFEE_pcb_y/2, sideFEE_pcb_z/2);
    G4LogicalVolume* logical_sideFEE_pcb = new G4LogicalVolume(solid_sideFEE_pcb, G4Material::GetMaterial("insulationBoard"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(0.2, 1, 0));
    logical_sideFEE_pcb->SetVisAttributes(vis);

    return logical_sideFEE_pcb;
}

G4LogicalVolume* PartProduce::lg_sideFEE(string name)
{
    Name.str("");
    Name<<"solid_sideFEE_"<<name;
    G4VSolid* solid_sideFEE = sd_sideFEE(0, Name.str());
    G4LogicalVolume* logical_sideFEE = new G4LogicalVolume(solid_sideFEE, G4Material::GetMaterial("Air"), name);

    Name.str("");
    Name<<"solid_sideFEE_out_"<<name;
    G4VSolid* solid_sideFEE_out = sd_sideFEE(-1*smallL, Name.str());

    Name.str("");
    Name<<"solid_sideFEE_in_"<<name;
    G4Trd* solid_sideFEE_in = new G4Trd(Name.str(), sideFEE_x/2, sideFEE_x/2, sideFEE_y/2-FEE_thickness, sideFEE_y/2-FEE_thickness, sideFEE_z/2-FEE_thickness);

    Name.str("");
    Name<<"solid_sideFEE_box_"<<name;
    G4Transform3D transform_sideFEE(rot_dummy, G4ThreeVector(FEE_thickness, 0, 0));
    G4SubtractionSolid* solid_sideFEE_box = new G4SubtractionSolid(Name.str(), solid_sideFEE_out, solid_sideFEE_in, transform_sideFEE);

    Name.str("");
    Name<<"logical_sideFEE_box_"<<name;
    G4LogicalVolume* logical_sideFEE_box = new G4LogicalVolume(solid_sideFEE_box, G4Material::GetMaterial("Aluminium"), Name.str());

    Name.str("");
    Name<<"logical_sideFEE_pcb_"<<name;
    G4LogicalVolume* logical_sideFEE_pcb = lg_sideFEE_pcb(Name.str());

    Name.str("");
    Name<<"physical_sideFEE_pcb_"<<name;
    new G4PVPlacement(0, G4ThreeVector(0,0,0), logical_sideFEE_pcb, Name.str(), logical_sideFEE, false, 0, checkOverlap);

    Name.str("");
    Name<<"physical_sideFEE_box_"<<name;
    new G4PVPlacement(0, G4ThreeVector(0,0,0), logical_sideFEE_box, Name.str(), logical_sideFEE, false, 0, checkOverlap);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(0.2,0.,1));
    //vis->SetForceWireframe(true);     
    logical_sideFEE_box->SetVisAttributes(vis);

    G4VisAttributes* vis1 = new G4VisAttributes();
    vis1->SetVisibility(0);
    logical_sideFEE->SetVisAttributes(vis1);

    return logical_sideFEE;
}


G4VSolid* PartProduce::sd_castingDie_outline(double smallL, string name)
{
    Name.str("");
    Name<<name<<"_1";
    //We make Z larger by 10*smallL to make a successful connection with the cover
    G4Trd* solid_castingDie_outline_1 = new G4Trd(Name.str(), castingDie_x/2+smallL, castingDie_x/2+smallL, castingDie_y/2+smallL, castingDie_y/2+smallL, castingDie_z/2+smallL*10);
    Name.str("");
    Name<<name<<"_2";
    G4Tubs* solid_castingDie_outline_2 = new G4Tubs(Name.str(), 0, castingDie_addr+smallL, castingDie_z/2+smallL*10, 0*degree, 360*degree);

    //We make a 0.1*mm for successful connection
    G4Transform3D transform_castingDie_outline_1(rot_dummy, G4ThreeVector(castingDie_x/2-0.1*mm,0,0));
    Name.str("");
    Name<<name<<"_3";
    G4UnionSolid* solid_castingDie_outline_3 = new G4UnionSolid(Name.str(), solid_castingDie_outline_1, solid_castingDie_outline_2, transform_castingDie_outline_1);

    G4Transform3D transform_castingDie_outline_2(rot_dummy, G4ThreeVector(-1*castingDie_x/2+0.1*mm,0,0));
    G4UnionSolid* solid_castingDie_outline = new G4UnionSolid(name, solid_castingDie_outline_3, solid_castingDie_outline_2, transform_castingDie_outline_2);
    return solid_castingDie_outline;
}

G4LogicalVolume* PartProduce::lg_castingDie(string name)
{
    Name.str("");
    Name<<"solid_castingDie_outline_"<<name;
    G4VSolid* solid_castingDie_outline = sd_castingDie_outline(0, Name.str());

    Name.str("");
    Name<<"solid_castingDie_subtract_"<<name;
    G4Trd* solid_castingDie_subtract = new G4Trd(Name.str(), castingDie_subtractx/2, castingDie_subtractx/2, castingDie_y, castingDie_y, castingDie_z/2);

    G4Transform3D transform_castingDie(rot_dummy, G4ThreeVector(0,0,-1*castingDie_subtract_posz));
    Name.str("");
    Name<<"solid_castingDie_"<<name;
    G4SubtractionSolid* solid_castingDie = new G4SubtractionSolid(Name.str(), solid_castingDie_outline, solid_castingDie_subtract, transform_castingDie);
    G4LogicalVolume* logical_castingDie = new G4LogicalVolume(solid_castingDie, G4Material::GetMaterial("Aluminium"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(0.2,0.,1));
    //vis->SetForceSolid(true);
    //vis->SetForceWireframe(true);     
    logical_castingDie->SetVisAttributes(vis);

    return logical_castingDie;
}

G4LogicalVolume* PartProduce::lg_container(int flag, string name)
{
    Name.str("");
    Name<<"solid_containerOut_"<<name;
    G4VSolid* solid_containerOut = sd_containerOut(smallL, Name.str());

    //add top cover
    Name.str("");
    Name<<"solid_upCover_"<<name;
    G4VSolid* solid_upCover = sd_upCover(smallL, Name.str());
    G4ThreeVector translation_upCover(0, (containerOut_y+upCover_y)/2, (upCover_z-containerOut_z)/2-upCover_posz);
    G4Transform3D transform_upCover(rot_dummy, translation_upCover);
    Name.str("");
    Name<<"solid_container_"<<name<<"_1";
    G4UnionSolid* solid_container_1 = new G4UnionSolid(Name.str(), solid_containerOut, solid_upCover, transform_upCover);

    //add FEE on the top cover
    Name.str("");
    Name<<"solid_upFEE_"<<name;
    G4VSolid* solid_upFEE = sd_upFEE(smallL, Name.str());
    G4ThreeVector translation_upFEE(0, containerOut_y/2+upCover_y+upFEE_y/2, upCover_z-upFEE_posz-upFEE_z/2-upCover_posz-containerOut_z/2);
    G4Transform3D transform_upFEE(rot_dummy, translation_upFEE);
    Name.str("");
    Name<<"solid_container_"<<name<<"_2";
    G4UnionSolid* solid_container_2 = new G4UnionSolid(Name.str(), solid_container_1, solid_upFEE, transform_upFEE);

    //add FEE at the side
    Name.str("");
    Name<<"solid_sideFEE_"<<name;
    G4VSolid* solid_sideFEE = sd_sideFEE(smallL, Name.str());

    double angle_containerFrameSide = atan((containerOut_xup-containerOut_xdown)/2/containerOut_z) *rad;
    double l_containerFrameSide = containerOut_z/cos(angle_containerFrameSide);
    double x_sideFEE = (l_containerFrameSide/2-sideFEE_posl)*sin(angle_containerFrameSide)+(containerOut_xup+containerOut_xdown)/4+sideFEE_x/2*cos(angle_containerFrameSide);
    double z_sideFEE = (l_containerFrameSide/2-sideFEE_posl)*cos(angle_containerFrameSide)-sideFEE_x/2*sin(angle_containerFrameSide);
    G4ThreeVector translation_sideFEE_left(-x_sideFEE, 0, z_sideFEE);
    G4RotationMatrix rot_sideFEE_left;
    rot_sideFEE_left.rotateY(-angle_containerFrameSide);
    G4Transform3D transform_sideFEE_left(rot_sideFEE_left, translation_sideFEE_left);
    Name.str("");
    Name<<"solid_container_"<<name<<"_3"; 
    G4UnionSolid* solid_container_3 = new G4UnionSolid(Name.str(), solid_container_2, solid_sideFEE, transform_sideFEE_left);

    G4ThreeVector translation_sideFEE_right(x_sideFEE, 0, z_sideFEE);
    G4RotationMatrix rot_sideFEE_right;
    rot_sideFEE_right.rotateY(angle_containerFrameSide);
    G4Transform3D transform_sideFEE_right(rot_sideFEE_right, translation_sideFEE_right);
    Name.str("");
    Name<<"solid_container_"<<name<<"_4";
    G4UnionSolid* solid_container_4 = new G4UnionSolid(Name.str(), solid_container_3, solid_sideFEE, transform_sideFEE_right);

    //add casting die
    Name.str("");
    Name<<"solid_castingDie_"<<name;
    G4VSolid* solid_castingDie = sd_castingDie_outline(smallL, Name.str());

    G4RotationMatrix rot_castingDie;
    rot_castingDie.rotateX(90*deg);
    G4ThreeVector translation_castingDie_1(0, containerOut_y/2+upCover_y+castingDie_z/2, -containerOut_z/2+upCover_posz+castingDie_posz1);
    G4Transform3D transform_castingDie_1(rot_castingDie, translation_castingDie_1);
    Name.str("");
    Name<<"solid_container_"<<name<<"_5";
    G4UnionSolid* solid_container_5 = new G4UnionSolid(Name.str(), solid_container_4, solid_castingDie, transform_castingDie_1);

    G4ThreeVector translation_castingDie_2(0, containerOut_y/2+upCover_y+castingDie_z/2, -containerOut_z/2+upCover_posz+castingDie_posz2);
    G4Transform3D transform_castingDie_2(rot_castingDie, translation_castingDie_2);
    Name.str("");
    Name<<"solid_container_"<<name<<"_6";
    G4UnionSolid* solid_container_6 = new G4UnionSolid(Name.str(), solid_container_5, solid_castingDie, transform_castingDie_2);

    //add bottom cover
    G4VSolid* solid_lowCover;
    Name.str("");
    Name<<"solid_lowCover_"<<name;
    double z_lowCover;
    if(flag==0)
    {
        z_lowCover = (lowCover1_z-containerOut_z)/2-lowCover1_posz;
        solid_lowCover = sd_lowCover1(smallL, Name.str());
    }
    else if(flag==1)
    {
        z_lowCover = (lowCover2_z-containerOut_z)/2-lowCover2_posz;
        solid_lowCover = sd_lowCover2(smallL, Name.str());
    }
    else G4cout<<"!!!!!!!! Wrong lower Board !!!!!!!!!"<<G4endl;

    G4ThreeVector translation_lowCover(0, -1*(containerOut_y+upCover_y)/2, z_lowCover);
    G4Transform3D transform_lowCover(rot_dummy, translation_lowCover);
    Name.str("");
    Name<<"solid_container_"<<name;
    G4UnionSolid* solid_container = new G4UnionSolid(Name.str(), solid_container_6, solid_lowCover, transform_lowCover);
    G4LogicalVolume* logical_container = new G4LogicalVolume(solid_container, G4Material::GetMaterial("Air"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(0.5,0.5,0.6));
    vis->SetForceWireframe(true);     
    logical_container->SetVisAttributes(vis);
    //logical_container->SetVisAttributes(G4VisAttributes::Invisible);

    return logical_container;
}


G4LogicalVolume* PartProduce::lg_gasContainer(string name)
{
    Name.str("");
    Name<<"solid_gasContainer_"<<name;
    G4VSolid* solid_gasContainer = sd_containerIn(0, Name.str());
    G4LogicalVolume* logical_gasContainer = new G4LogicalVolume(solid_gasContainer, G4Material::GetMaterial("MRPCGas"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    //vis->SetVisibility(0);
    vis->SetForceWireframe(true);
    logical_gasContainer->SetVisAttributes(vis);

    return logical_gasContainer;
}


G4LogicalVolume* PartProduce::lg_bareChamber(string name)
{
    //A trapezoid to contain the bare chamber
    double xdown = pcbBoard_xdown;
    double xup = pcbBoard_xup;
    double y = honeyComb_y*2+tape_mrpc_y*2+pcbBoard_y0*2+pcbBoard_y1+mylar_y*4+glass0_y*4+glass1_y*10+gasLayer_y*12;
    double z = pcbBoard_z;
    Name.str("");
    Name<<"solid_bareChamber_"<<name;
    G4Trd* solid_bareChamber = new G4Trd(Name.str(), xdown/2.+smallL, xup/2.+smallL, y/2.+smallL, y/2.+smallL, z/2.+smallL);
    G4LogicalVolume* logical_bareChamber = new G4LogicalVolume(solid_bareChamber, G4Material::GetMaterial("MRPCGas"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    //vis->SetColor(G4Color(0.3, 0.3, 0.3));
    vis->SetVisibility(0);
    //vis->SetForceWireframe(true);
    logical_bareChamber->SetVisAttributes(vis);

    return logical_bareChamber;
}


G4LogicalVolume* PartProduce::lg_honeyComb(string name)
{
    Name.str("");
    Name<<"solid_honeyComb_"<<name;
    G4Trd* solid_honeyComb = new G4Trd(Name.str(), honeyComb_xdown/2, honeyComb_xup/2, honeyComb_y/2, honeyComb_y/2, honeyComb_z/2);
    G4LogicalVolume* logical_honeyComb = new G4LogicalVolume(solid_honeyComb, G4Material::GetMaterial("honeycombCore"), name);

    //Two surfaces
    Name.str("");
    Name<<"solid_honeyComb_surface_"<<name;
    G4Trd* solid_honeyComb_surface = new G4Trd(Name.str(), honeyComb_xdown/2, honeyComb_xup/2, honeyComb_surface_y/2, honeyComb_surface_y/2, honeyComb_z/2);

    Name.str("");
    Name<<"logical_honeyComb_surface_"<<name;
    G4LogicalVolume* logical_honeyComb_surface = new G4LogicalVolume(solid_honeyComb_surface, G4Material::GetMaterial("insulationBoard"), Name.str());

    Name.str("");
    Name<<"physical_honeyComb_surface_"<<name<<"_0";
    new G4PVPlacement(0, G4ThreeVector(0,(honeyComb_y-honeyComb_surface_y)/2.-smallL,0), logical_honeyComb_surface, Name.str(), logical_honeyComb, false, 0, checkOverlap);

    Name.str("");
    Name<<"physical_honeyComb_surface_"<<name<<"_1";
    new G4PVPlacement(0, G4ThreeVector(0,-(honeyComb_y-honeyComb_surface_y)/2.+smallL,0), logical_honeyComb_surface, Name.str(), logical_honeyComb, false, 0, checkOverlap);

    G4VisAttributes* vis1 = new G4VisAttributes();
    vis1->SetColor(G4Color::Green());
    //vis1->SetForceWireframe(true);
    logical_honeyComb_surface->SetVisAttributes(vis1);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(0.55, 0.3, 0.3));
    //vis->SetForceWireframe(true);     
    logical_honeyComb->SetVisAttributes(vis);

    return logical_honeyComb;
}

G4LogicalVolume* PartProduce::lg_tape_mrpc(string name)
{
    Name.str("");
    Name<<"solid_tape_mrpc_"<<name;
    G4Trd* solid_tape_mrpc= new  G4Trd(Name.str(),tape_mrpc_xdown/2,tape_mrpc_xup/2,tape_mrpc_y/2,tape_mrpc_y/2,tape_mrpc_z/2);
    G4LogicalVolume* logical_tape_mrpc = new G4LogicalVolume(solid_tape_mrpc,G4Material::GetMaterial("PET"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(1, 1, 0));
    //vis->SetForceWireframe(true);     
    //vis->SetForceSolid(true); 
    logical_tape_mrpc->SetVisAttributes(vis);

    return logical_tape_mrpc;
}

G4LogicalVolume* PartProduce::lg_pcbBoard(G4int flag, string name) //flag:0:out; 1:inside
{
    double pcbBoard_y;
    if(flag==0) pcbBoard_y=pcbBoard_y0;
    else pcbBoard_y=pcbBoard_y1;

    Name.str("");
    Name<<"solid_pcbBoard_"<<name;
    G4Trd* solid_pcbBoard = new G4Trd(Name.str(),pcbBoard_xdown/2., pcbBoard_xup/2., pcbBoard_y/2., pcbBoard_y/2.,pcbBoard_z/2.);
    G4LogicalVolume* logical_pcbBoard = new G4LogicalVolume(solid_pcbBoard,G4Material::GetMaterial("insulationBoard"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(0.2, 1, 0));
    //vis->SetForceSolid(true); 
    //vis->SetForceWireframe(true); 
    logical_pcbBoard->SetVisAttributes(vis);

    return logical_pcbBoard;
}

G4AssemblyVolume* PartProduce::lg_strip(string name)
{
    const G4int no=12;
    G4Trd* solid_strip[no];
    G4LogicalVolume* logical_strip[no];
    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(1, 0, 0));
    vis->SetForceSolid(1);
    for(G4int i=0; i<no; i++)
    {
        Name.str("");
        Name<<"solid_strip_"<<name<<"_"<<i;
        solid_strip[i] = new G4Trd(Name.str(), strip_x[i]/2., strip_x[i]/2., strip_y/2., strip_y/2., strip_z/2.);
        Name.str("");
        Name<<name<<"_"<<i;
        logical_strip[i] = new G4LogicalVolume(solid_strip[i], G4Material::GetMaterial("Copper"), Name.str());
        logical_strip[i]->SetVisAttributes(vis);
    }

    G4AssemblyVolume* logical_strip_unified = new G4AssemblyVolume();
    G4ThreeVector translation_strip;
    G4Transform3D transform_strip;
    for(G4int i=0; i<no; i++)
    {
        translation_strip.set(0, 0, (i-5.5)*(strip_z+strip_gap));
        transform_strip = G4Transform3D(rot_dummy, translation_strip);
        logical_strip_unified->AddPlacedVolume(logical_strip[i], transform_strip);
    }

    return logical_strip_unified;
}

G4LogicalVolume* PartProduce::lg_mylar(string name)
{
    Name.str("");
    Name<<"solid_mylar_"<<name;
    G4Trd* solid_mylar = new G4Trd(Name.str(),mylar_xdown/2.,mylar_xup/2.,mylar_y/2.,mylar_y/2.,mylar_z/2.);
    G4LogicalVolume* logical_mylar = new G4LogicalVolume(solid_mylar,G4Material::GetMaterial("PET"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(1, 0., 0.7));
    //vis->SetForceSolid(true); 
    logical_mylar->SetVisAttributes(vis);

    return logical_mylar;
}

G4LogicalVolume* PartProduce::lg_glass(G4int flag, string name) //flag:0:out; 1:inside
{
    double glass_xdown;
    double glass_xup;
    double glass_y;
    double glass_z;
    if(flag==0)
    {
        glass_xdown = glass0_xdown;
        glass_xup = glass0_xup;
        glass_y = glass0_y;
        glass_z = glass0_z;
    }
    else
    {
        glass_xdown = glass1_xdown;
        glass_xup = glass1_xup;
        glass_y = glass1_y;
        glass_z = glass1_z;
    }

    Name.str("");
    Name<<"solid_glass_"<<name;
    G4Trd* solid_glass = new G4Trd(Name.str(), glass_xdown/2.,glass_xup/2. ,glass_y/2.,glass_y/2.,glass_z/2.);
    G4LogicalVolume* logical_glass = new G4LogicalVolume(solid_glass,G4Material::GetMaterial("SiO2"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(0.2, 0.4, 1));
    //vis->SetForceSolid(true); 
    logical_glass->SetVisAttributes(vis);

    return logical_glass;
}

G4LogicalVolume* PartProduce::lg_gasLayer(string name)
{
    Name.str("");
    Name<<"solid_gasLayer_"<<name;
    G4Trd* solid_gasLayer = new G4Trd(Name.str(),gasLayer_xdown/2.,gasLayer_xup/2.,gasLayer_y/2.,gasLayer_y/2.,gasLayer_z/2.);
    G4LogicalVolume* logical_gasLayer = new G4LogicalVolume(solid_gasLayer,G4Material::GetMaterial("MRPCGas"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color::Blue());
    //vis->SetForceSolid(true); 
    //logical_gasLayer->SetVisAttributes(vis);

    return logical_gasLayer;
}

G4LogicalVolume* PartProduce::lg_bracket(string name)
{
    Name.str("");
    Name<<"solid_bracket_"<<name<<"_1";
    G4Tubs* solid_bracket_1 = new G4Tubs(Name.str(), 0, bracket_r, bracket_z/2, 0*degree, 360*degree);

    Name.str("");
    Name<<"solid_bracket_"<<name<<"_2";
    G4Trd* solid_bracket_2 = new G4Trd(Name.str(), bracket_x/2, bracket_x/2, bracket_x/2, bracket_x/2, bracket_y/2);

    G4Transform3D transform_bracket(rot_dummy, G4ThreeVector(0, 0, (bracket_y+bracket_z)/2));
    Name.str("");
    Name<<"solid_bracket_"<<name;
    G4UnionSolid* solid_bracket = new G4UnionSolid(Name.str(), solid_bracket_1, solid_bracket_2, transform_bracket);
    G4LogicalVolume* logical_bracket = new G4LogicalVolume(solid_bracket, G4Material::GetMaterial("Nylon"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(0.3, 0.3, 0.3));
    //vis->SetForceSolid(true); 
    logical_bracket->SetVisAttributes(vis);

    return logical_bracket;
}

G4LogicalVolume* PartProduce::lg_upStopBlock(string name)
{
    Name.str("");
    Name<<"solid_upStopBlock_"<<name<<"_1";
    G4Trd* solid_upStopBlock_1 = new G4Trd(Name.str(), upStopBlock_x/2, upStopBlock_x/2, upStopBlock_y/2, upStopBlock_y/2, upStopBlock_z/2);

    Name.str("");
    Name<<"solid_upStopBlock_"<<name<<"_2";
    G4Tubs* solid_upStopBlock_2 = new G4Tubs(Name.str(), 0, upStopBlock_subtractr+smallL, upStopBlock_x/2+smallL, 0*degree, 360*degree);

    G4RotationMatrix rot_upStopBlock;
    //Rotation outside of the transform3D. In transform3D the 3 angles are Euler
    rot_upStopBlock.rotateY(90*degree);
    G4Transform3D transform_upStopBlock(rot_upStopBlock, G4ThreeVector(0,0,(upStopBlock_z-upStopBlock_subtractr)/2-upStopBlock_subtractposz));

    Name.str("");
    Name<<"solid_upStopBlock_"<<name;
    G4SubtractionSolid* solid_upStopBlock = new G4SubtractionSolid(Name.str(), solid_upStopBlock_1, solid_upStopBlock_2, transform_upStopBlock);
    G4LogicalVolume* logical_upStopBlock = new G4LogicalVolume(solid_upStopBlock, G4Material::GetMaterial("Nylon"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(0.3, 0.3, 0.3));
    //vis->SetForceSolid(true); 
    logical_upStopBlock->SetVisAttributes(vis);

    return logical_upStopBlock;
}

G4LogicalVolume* PartProduce::lg_sideStopBlock(string name)
{
    Name.str("");
    Name<<"solid_sideStopBlock_"<<name<<"_1";
    G4Trd* solid_sideStopBlock_1 = new G4Trd(Name.str(), sideStopBlock_x/2, sideStopBlock_x/2, sideStopBlock_y/2, sideStopBlock_y/2, sideStopBlock_z/2);

    Name.str("");
    Name<<"solid_sideStopBlock_"<<name<<"_2";
    G4Trd* solid_sideStopBlock_2 = new G4Trd(Name.str(), (sideStopBlock_x-sideStopBlock_thickness)/2+smallL, (sideStopBlock_x-sideStopBlock_thickness)/2+smallL, sideStopBlock_y/2+smallL, sideStopBlock_y/2+smallL, sideStopBlock_z/2-sideStopBlock_thickness);

    G4Transform3D transform_upStopBlock(rot_dummy, G4ThreeVector(sideStopBlock_thickness/2, 0, 0));

    Name.str("");
    Name<<"solid_sideStopBlock_"<<name;
    G4SubtractionSolid* solid_sideStopBlock = new G4SubtractionSolid(Name.str(), solid_sideStopBlock_1, solid_sideStopBlock_2, transform_upStopBlock);
    G4LogicalVolume* logical_sideStopBlock = new G4LogicalVolume(solid_sideStopBlock, G4Material::GetMaterial("Nylon"), name);

    G4VisAttributes* vis = new G4VisAttributes();
    vis->SetColor(G4Color(0.3, 0.3, 0.3));
    //vis->SetForceSolid(true); 
    logical_sideStopBlock->SetVisAttributes(vis);

    return logical_sideStopBlock;
}

