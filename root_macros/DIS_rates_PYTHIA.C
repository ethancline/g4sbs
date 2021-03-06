#include "g4sbs_a1n_tree.C"
#include "TChain.h"
#include "TTree.h"
#include "TFile.h"
#include "TChainElement.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TGraphErrors.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "G4SBSRunData.hh"

const double Mp = 0.938272;

void DIS_rates_PYTHIA(const char *configfilename, const char *outfilename){
  
  
  TChain *C = new TChain("T");

  TString currentline;

  ifstream infile(configfilename);

  set<TString> proton_files;
  
  while( currentline.ReadLine( infile ) && !currentline.BeginsWith("endlist_proton") ){
    if( !currentline.BeginsWith("#") ){
      
      C->Add( currentline );
      
      proton_files.insert(currentline);
    }
  }

  set<TString> neutron_files;
  
  while( currentline.ReadLine( infile ) && !currentline.BeginsWith("endlist_neutron") ){
    if( !currentline.BeginsWith("#") ){
      
      C->Add( currentline );
      
      neutron_files.insert(currentline);
    }
  }

  TObjArray *FileList = C->GetListOfFiles();
  TIter next(FileList);

  TChainElement *chEl = 0;

  set<TString> bad_file_list;

  long ngen_p=0, ngen_n=0;

  G4SBSRunData *rd;
  
  while( (chEl=(TChainElement*)next() )){
    TFile newfile(chEl->GetTitle());
    newfile.GetObject("run_data",rd);
    if( rd ){
      if( proton_files.find( chEl->GetTitle() ) != proton_files.end() ){
	ngen_p += rd->fNtries;
      }
      if( neutron_files.find( chEl->GetTitle() ) != neutron_files.end() ){
	ngen_n += rd->fNtries;
      }
      //nfiles++;
    } else {
      bad_file_list.insert( chEl->GetTitle());
    }
  }

  cout << "Total number of generated events, proton = " << ngen_p << endl;
  cout << "Total number of generated events, neutron = " << ngen_n << endl;
  
  TH1D::SetDefaultSumw2();
  TH2D::SetDefaultSumw2();
  
  double Emin_SBS = 1.5; //GeV
  double Emin_BB = 0.8;

  int binflag = 0, nbins;
  vector<double> bins;
  infile >> binflag >> nbins;
  bins.resize(nbins+1);
  if( binflag == 0 ){ //fixed-width bins in xbj:
    double min, max;
    infile >> min >> max;
    for( int bin=0; bin<=nbins; bin++ ){
      bins[bin] = min + bin*(max-min)/double(nbins);
    }
  } else { //variable-width bins in xbj:
    bins.resize(nbins+1);
    for( int bin=0; bin<=nbins; bin++ ){
      infile >> bins[bin];
    }
  }

  double Ibeam=50e-6; //Amps
  double rho_tgt=1.3e-3, Ltgt=55.0; //g/cm^3 and cm
  double N_A = 6.022e23; //avogadro's number
  double e = 1.602e-19; //electron charge
  double Mmol_3He = 3.016; //g/mol

  infile >> Ibeam >> Ltgt;
  Ibeam *= 1.e-6; //assumed to be given in muA.
  //             (e-/s) *     (g/cm^2)   / (g/mol)  * (atoms/mol) = e- * atoms /cm^2/s 
  double Lumi = Ibeam/e * rho_tgt * Ltgt / Mmol_3He * N_A;

  cout << "For an assumed beam current of " << Ibeam*1e6 << " muA and Ltgt = " << Ltgt << " cm, electron-nucleus luminosity = " << Lumi
       << endl << " or equivalently, " << 3.*Lumi << " electron-nucleon luminosity" << endl << endl;

  double Pbeam = 0.85;
  double Ptgt = 0.6;
  infile >> Pbeam >> Ptgt;
  infile >> Emin_SBS >> Emin_BB;
  
  TFile *fout = new TFile(outfilename,"RECREATE");
  
  TH1D *hrate_p_xbj_BB = new TH1D("hrate_p_xbj_BB","",nbins,&(bins[0]));
  TH1D *hrate_n_xbj_BB = new TH1D("hrate_n_xbj_BB","",nbins,&(bins[0]));
  TH1D *hrate_3He_xbj_BB = new TH1D("hrate_3He_xbj_BB","",nbins,&(bins[0]));

  TProfile *hQ2_xbj_BB_prof = new TProfile("hQ2_xbj_BB_prof","",nbins,&(bins[0]));
  TProfile *hEprime_xbj_BB_prof = new TProfile("hEprime_xbj_BB_prof","",nbins,&(bins[0]));
  TProfile *hxmean_xbj_BB_prof = new TProfile("hxmean_xbj_BB_prof","",nbins,&(bins[0]));
  TProfile *hW_xbj_BB_prof = new TProfile("hW_xbj_BB_prof","",nbins,&(bins[0]));
  TProfile *hy_xbj_BB_prof = new TProfile("hy_xbj_BB_prof","",nbins,&(bins[0]));
  TProfile *heps_xbj_BB_prof = new TProfile("heps_xbj_BB_prof","",nbins,&(bins[0]));
  
  TH2D *hQ2_xbj_BB = new TH2D("hQ2_xbj_BB","",100,0,1,100,1,12);
  TH2D *hEprime_xbj_BB = new TH2D("hEprime_xbj_BB","",100,0,1,100,0,11);
  
  TH1D *hrate_p_xbj_SBS = new TH1D("hrate_p_xbj_SBS","",nbins,&(bins[0]));
  TH1D *hrate_n_xbj_SBS = new TH1D("hrate_n_xbj_SBS","",nbins,&(bins[0]));
  TH1D *hrate_3He_xbj_SBS = new TH1D("hrate_3He_xbj_SBS","",nbins,&(bins[0]));

  // TH1D *hdA1n_500h_BB = new TH1D("hdA1n_500h_BB", "", nbins, &(bins[0]) );
  // TH1D *hdA1n_500h_SBS = new TH1D("hdA1n_500h_SBS", "", nbins, &(bins[0]) );
  
  TProfile *hQ2_xbj_SBS_prof = new TProfile("hQ2_xbj_SBS_prof","",nbins,&(bins[0]));
  TProfile *hEprime_xbj_SBS_prof = new TProfile("hEprime_xbj_SBS_prof","",nbins,&(bins[0]));
  TProfile *hxmean_xbj_SBS_prof = new TProfile("hxmean_xbj_SBS_prof","",nbins,&(bins[0]));
  TProfile *hW_xbj_SBS_prof = new TProfile("hW_xbj_SBS_prof","",nbins,&(bins[0]));
  TProfile *hy_xbj_SBS_prof = new TProfile("hy_xbj_SBS_prof","",nbins,&(bins[0]));
  TProfile *heps_xbj_SBS_prof = new TProfile("heps_xbj_SBS_prof","",nbins,&(bins[0]));
  TH2D *hQ2_xbj_SBS = new TH2D("hQ2_xbj_SBS","",100,0,1,100,1,12);
  TH2D *hEprime_xbj_SBS = new TH2D("hEprime_xbj_SBS","",100,0,1,100,0,11);
  
  
  g4sbs_a1n_tree *T = new g4sbs_a1n_tree(C);

  long nevent = 0;

  while( T->GetEntry(nevent++) ){
    if( nevent%1000 == 0 ) cout << nevent << endl;

    //sigma is in mb, we want to express in cm^2. 1 barn = 1e-24 cm^2, 1 mb = 1e-27 cm^2
    double sigma = T->primaries_Sigma*1e-27;
    //figure out whether this is a proton file or a neutron file.

    double weight_p = 0.0; //This will be normalized to an event rate:
    double weight_n = 0.0;
    TString fname = C->GetFile()->GetName();

    //cout << "Current filename = " << fname << endl;
    
    if( proton_files.find( fname ) != proton_files.end() ){
      weight_p = 2.0 * sigma * Lumi / double(ngen_p);
    } else if( neutron_files.find( fname ) != neutron_files.end() ){
      weight_n = sigma * Lumi / double(ngen_n);  
    }

    //cout << "event " << nevent << ", weight_p, weight_n = " << weight_p << ", " << weight_n << endl;
    
    //Select events in which the primary electron made a track in SBS:
    if( T->Harm_SBSGEM_Track_ntracks > 0 ){
      for( int track=0; track<T->Harm_SBSGEM_Track_ntracks; track++ ){
	if( (*(T->Harm_SBSGEM_Track_PID))[track] == 11 &&
	    (*(T->Harm_SBSGEM_Track_MID))[track] == 0 &&
	    (*(T->Harm_SBSGEM_Track_P))[track]/T->primaries_Eprime >= 0.9 &&
	    T->primaries_Eprime >= Emin_SBS ){ //Then this is the primary electron track:
	  double xbj = T->primaries_xbj;
	  double Q2 = T->primaries_Q2;
	  double y = T->primaries_y;
	  double Eprime = T->primaries_Eprime;
	  double W = sqrt(T->primaries_W2);
	  double etheta = T->primaries_theta_e;
	  
	  double gamma2 = pow(2.*Mp*xbj,2)/Q2; // Q^2 = 2M nu x --> 2Mx = Q^2/nu --> gamma^2 = Q^2 / nu^2 

	  double epsilon = 1.0/( 1. + 2*(1+1./gamma2)*pow(tan(etheta/2.),2) );
	  
	  hrate_p_xbj_SBS->Fill( xbj, weight_p );
	  hrate_n_xbj_SBS->Fill( xbj, weight_n );
	  hrate_3He_xbj_SBS->Fill( xbj, weight_p + weight_n ); //=2p + n;

	  hQ2_xbj_SBS->Fill( xbj, Q2, weight_p + weight_n );
	  hEprime_xbj_SBS->Fill( xbj, Eprime, weight_p + weight_n );

	  hQ2_xbj_SBS_prof->Fill( xbj, Q2, weight_p + weight_n );
	  hEprime_xbj_SBS_prof->Fill( xbj, Eprime, weight_p + weight_n );
	  hxmean_xbj_SBS_prof->Fill( xbj, xbj, weight_p + weight_n );
	  hW_xbj_SBS_prof->Fill( xbj, W, weight_p + weight_n );
	  hy_xbj_SBS_prof->Fill( xbj, y, weight_p + weight_n );
	  heps_xbj_SBS_prof->Fill( xbj, epsilon, weight_p + weight_n );
	}
      }
    }

    //Select events in which the primary electron made a track in SBS:
    if( T->Earm_BBGEM_Track_ntracks > 0 ){
      for( int track=0; track<T->Earm_BBGEM_Track_ntracks; track++ ){
	if( (*(T->Earm_BBGEM_Track_PID))[track] == 11 &&
	    (*(T->Earm_BBGEM_Track_MID))[track] == 0 &&
	    (*(T->Earm_BBGEM_Track_P))[track]/T->primaries_Eprime >= 0.9 &&
	    T->primaries_Eprime >= Emin_BB ){ //Then this is with a high degree of certainty the primary scattered electron track:
	  double xbj = T->primaries_xbj;
	  double Q2 = T->primaries_Q2;
	  double y = T->primaries_y;
	  double Eprime = T->primaries_Eprime;
	  double W = sqrt(T->primaries_W2);
	  double etheta = T->primaries_theta_e;
	  
	  double gamma2 = pow(2.*Mp*xbj,2)/Q2; // Q^2 = 2M nu x --> 2Mx = Q^2/nu --> gamma^2 = Q^2 / nu^2 

	  double epsilon = 1.0/( 1. + 2*(1+1./gamma2)*pow(tan(etheta/2.),2) );
	  
	  hrate_p_xbj_BB->Fill( xbj, weight_p );
	  hrate_n_xbj_BB->Fill( xbj, weight_n );
	  hrate_3He_xbj_BB->Fill( xbj, weight_p + weight_n ); //=2p + n;

	  hQ2_xbj_BB->Fill( xbj, Q2, weight_p + weight_n );
	  hEprime_xbj_BB->Fill( xbj, Eprime, weight_p + weight_n );

	  hQ2_xbj_BB_prof->Fill( xbj, Q2, weight_p + weight_n );
	  hEprime_xbj_BB_prof->Fill( xbj, Eprime, weight_p + weight_n );
	  hxmean_xbj_BB_prof->Fill( xbj, xbj, weight_p + weight_n );
	  hW_xbj_BB_prof->Fill( xbj, W, weight_p + weight_n );
	  hy_xbj_BB_prof->Fill( xbj, y, weight_p + weight_n );
	  heps_xbj_BB_prof->Fill( xbj, epsilon, weight_p + weight_n );
	}
      }
    }
  }

  fout->Write();
  
}
