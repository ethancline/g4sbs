#ifndef G4SBSDetectorConstruction_h
#define G4SBSDetectorConstruction_h 1

#include "G4LogicalVolume.hh"
#include "G4VUserDetectorConstruction.hh"
//#include "G4SBSIO.hh"
#include "sbstypes.hh"
#include "globals.hh"

#include "G4Material.hh"
#include "G4OpticalSurface.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include <map>
#include <set>

using namespace std;

class G4SBSGlobalField;
class G4SBSMagneticField;
class G4SBSBigBiteField;
class G4SBS48D48Field;

class G4SBSBeamlineBuilder;
class G4SBSTargetBuilder;
class G4SBSEArmBuilder;
class G4SBSHArmBuilder;


class G4SBSDetectorMessenger;

class G4SBSDetectorConstruction : public G4VUserDetectorConstruction //The : means that G4SBSD.. inherits all methods and member variables from calss G4VUser.. **Notes 
{
public:
  G4SBSDetectorConstruction();
  ~G4SBSDetectorConstruction();

public:
  G4VPhysicalVolume* Construct();
  G4VPhysicalVolume* ConstructAll();

  void ConstructMaterials();  //Construct materials and optical surfaces
  G4Material *GetMaterial( G4String );
  G4OpticalSurface *GetOpticalSurface( G4String );

  //Build the various subsystems (BeamLine, Target, SBS Magnet, HCAL, BigCal, RICH, GEMs, etc.):
  void ConstructBeamline(G4LogicalVolume*);
  void ConstructTarget(G4LogicalVolume*);

  G4SBSMagneticField *GetBBField(){ return fbbfield; }
  G4SBSMagneticField *Get48D48Field(){ return f48d48field; }
  G4SBSGlobalField *GetGlobalField(){ return fGlobalField; }

  void SetBigBiteField(int n, G4String fname="map_696A.dat");
  void Set48D48Field(int n);

  // D. Flay (7/28/20).  GEn 3He target Helmholtz coil configuration
  void SetGEnTargetHelmholtzConfig(int n) { fGEnTgtHHConfig =n; }  
  int GetGEnTargetHelmholtzConfig() const { return fGEnTgtHHConfig; } 
 
  // D. Flay (9/29/20).  GEn 3He target angular misalignment
  void SetGEnTargetDRX(double rx) { fGEnTgtDRX = rx; } 
  void SetGEnTargetDRY(double ry) { fGEnTgtDRY = ry; } 
  void SetGEnTargetDRZ(double rz) { fGEnTgtDRZ = rz; }
  G4double GetGEnTargetDRX() const { return fGEnTgtDRX; } 
  G4double GetGEnTargetDRY() const { return fGEnTgtDRY; } 
  G4double GetGEnTargetDRZ() const { return fGEnTgtDRZ; }

  // D. Flay (10/9/20).  GEn 3He target collimators 
  void SetGEnTargetCollimatorEnable(bool v)    { fGEnTgtCollimatorEnable  = v; }  
  void SetGEnTargetCollimatorAEnable(bool v)   { fGEnTgtCollimatorAEnable = v; }  
  void SetGEnTargetCollimatorBEnable(bool v)   { fGEnTgtCollimatorBEnable = v; }  
  void SetGEnTargetCollimatorCEnable(bool v)   { fGEnTgtCollimatorCEnable = v; }  

  G4bool GetGEnTargetCollimatorEnable()  const { return fGEnTgtCollimatorEnable;  }  
  G4bool GetGEnTargetCollimatorAEnable() const { return fGEnTgtCollimatorAEnable; }  
  G4bool GetGEnTargetCollimatorBEnable() const { return fGEnTgtCollimatorBEnable; }  
  G4bool GetGEnTargetCollimatorCEnable() const { return fGEnTgtCollimatorCEnable; }  
 
  void SetBeamDumpEnable(bool v)   { fBeamDumpEnable = v;    } 
  G4bool GetBeamDumpEnable() const { return fBeamDumpEnable; } 
  // D. Flay (8/25/20).  Beam Diffuser enable 
  void SetBeamDiffuserEnable(bool v)   { fBeamDiffuserEnable = v;    } 
  G4bool GetBeamDiffuserEnable() const { return fBeamDiffuserEnable; } 

  void SetTotalAbs(bool b){ fTotalAbs= b; }
  void SetCheckOverlap(bool b){ fCheckOverlap = b; }

  void SetExpType( G4SBS::Exp_t et ){ fExpType = et; }
  void SetTarget( G4SBS::Targ_t tg ){ fTargType = tg; }

  void SetUniformMagneticField48D48( double B );

  int  fBeamlineConf;
  int  fLeadOption;
  bool fBLneutronDet;
  
  G4SBS::Exp_t fExpType;
  G4SBS::Targ_t fTargType;

  //map<G4String, G4VSensitiveDetector*> SDlist; //List of all sensitive detectors in the run. This is redundant with G4SDManager; just use that instead.
  set<G4String> SDlist;
  map<G4String, G4SBS::SDet_t> SDtype; //Mapping of sensitive detector names to sensitive detector types
  //map<G4String, G4SBS::Arm_t> SDarm; //Mapping of sensitive detector names to spectrometer arms
  set<G4String> StepLimiterList; //List of sensitive detectors for which G4UserLimits are defined to stop all particles entering (only allowed for calorimeters!)
  
  map<G4String, G4double> SDgatewidth; //Time window for accumulating hit signal
  map<G4String, G4double> SDthreshold; //threshold (energy deposition or photoelectrons) for recording a hit
  //map<G4String, G4int>    SDntimebins; //Time bins for "pulse shape" histogram

  void InsertSDboundaryVolume( G4String bvname, G4String sdname );
  
  map<G4String, set<G4String> > SDboundaryVolumes; //mapping between sensitive detector names and entrance boundary volumes. Here the key is the "boundary" or mother volume name, the mapped value is the set of unique sensitive detectors associated with this "boundary volume" (In many cases we have ONE boundary volume containing multiple SDs)
  
  G4SDManager *fSDman; 
  bool fTotalAbs;
  bool fCheckOverlap; //< Check if volumes overlap

  G4SBSBeamlineBuilder *fBeamlineBuilder;
  G4SBSTargetBuilder   *fTargetBuilder;
  G4SBSEArmBuilder     *fEArmBuilder;
  G4SBSHArmBuilder     *fHArmBuilder;

  void SetBBDist( double);
  void SetBBAng( double);
  void Set48D48Dist( double);
  void Set48D48Ang( double);

  void AddToscaField(const char *);

  bool fUseGlobalField;

  G4SBSGlobalField *fGlobalField;

  void SetECALmapfilename( G4String );
  G4String GetECALmapfilename(){ return fECALmapfilename; }

  // int TrackerIDnumber;
  // map<int,G4SBS::Arm_t> TrackerArm; //Is tracker in E arm or P arm?

  void SetCDetconfig( int );
  int GetCDetConfigOption() { return fCDetOption; }

  void SetC16Segmentation( int );
  int GetC16Segmentation() { return fSegmentC16; }

  void SetFieldScale_SBS( G4double );
  G4double GetFieldScale_SBS() { return fFieldScale_SBS; }

  void SetFieldScale_BB( G4double );
  G4double GetFieldScale_BB() { return fFieldScale_BB; }

  void SetSegmentThickC16( G4double );
  G4double GetSegmentThickC16(){ return fSegmentThickC16; }

  void SetDoseRateC16( G4double );
  G4double GetDoseRateC16(){ return fDoseRateC16; }

  void SetFlipGEM( G4bool );
  G4bool GetFlipGEM(){ return fGEMflip; }

  void SetOpticalPhotonDisabled(G4String material){ fMaterialsListOpticalPhotonDisabled.insert( material ); }

  void SetTimeWindowAndThreshold( G4String SDname, G4double Ethresh=0.0*MeV, G4double Twindow=1000.0*ns ); //utility function to set time window and threshold by sensitive detector name

  inline set<G4String> GetTargetVolumes() const { return fTargetVolumes; }
  inline set<G4String> GetAnalyzerVolumes() const { return fAnalyzerVolumes; }

  inline void InsertTargetVolume( G4String vname ){ fTargetVolumes.insert(vname); }
  inline void InsertAnalyzerVolume( G4String vname ){ fAnalyzerVolumes.insert(vname); }
  
private:

  map<G4String, G4Material*> fMaterialsMap;
  map<G4String, G4OpticalSurface*> fOpticalSurfacesMap;
  set<G4String> fMaterialsListOpticalPhotonDisabled; //Allows us to disable definition of refractive index and
  //scintillation parameter definition for individual materials, to prevent optical photon production and tracking

  set<G4String> fTargetVolumes; //list of logical volume names to be flagged as "TARGET"
  set<G4String> fAnalyzerVolumes; //list of logical volume names to be flagged as "ANALYZER"
  
  G4SBSMagneticField *fbbfield;
  G4SBSMagneticField *f48d48field;
  
  G4String fECALmapfilename;

  //Let's define some additional configurable properties of 48D48:
  G4double f48D48_uniform_bfield; //set magnitude (and polarity) of SBS magnetic field (direction is fixed)
  G4double fFieldScale_SBS;
  G4double fFieldScale_BB;
  
  G4int fCDetOption;
  G4int fSegmentC16;

  G4double fSegmentThickC16;
  G4double fDoseRateC16; //Dose rate at z = 0 of lead-glass.

  G4bool fGEMflip;

  G4int fGEnTgtHHConfig; // D. Flay (7/28/20).  GEn 3He target Helmholtz coil configuration
  
  G4double fGEnTgtDRX,fGEnTgtDRY,fGEnTgtDRZ; // D. Flay (9/29/20).  GEn 3He target angular misalignment 

  // for GEn 3He target collimators 
  G4bool fGEnTgtCollimatorEnable; 
  G4bool fGEnTgtCollimatorAEnable,fGEnTgtCollimatorBEnable,fGEnTgtCollimatorCEnable; 

  // D. Flay (8/25/20).  Beam diffuser enable and beam pointing
  G4bool fBeamDumpEnable;       // enable the beam diffuser 
  G4bool fBeamDiffuserEnable;       // enable the beam diffuser 
  G4double fBeamPointX,fBeamPointY; // beam pointing   

};


#endif


