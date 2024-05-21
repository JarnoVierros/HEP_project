
#include <iostream>
#include "TTree.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TH1F.h"
#include "TCanvas.h"

#include <vector>
#include <math.h>
using namespace std;




int main() {

    
    TFile inFile("Drell_Yan_muons_smeared.root");

    TTreeReader Reader("muons",&inFile);

    TTreeReaderArray<float> p(Reader, "p");
    TTreeReaderArray<float> pT(Reader, "pT");
    TTreeReaderArray<float> eta(Reader, "eta");
    TTreeReaderArray<float> phi(Reader, "phi");
    TTreeReaderArray<float> m(Reader, "m");
    TTreeReaderArray<int> Q(Reader, "Q");
    //TTreeReaderArray<int> H(Reader, "H");  //No use for this in this code


    //TTree* reconstructed = new TTree("reconstructed", "Reconstructed particle from muon and anti muon");
    
    //float recon_p;
    //float reocon_pT;
    //float recon_eta;
    //float recon_phi;
    float recon_m;
    //int recon_Q;

    // I'm not sure we need the following part (I would say no):
    // vector<int> recon_H;

    //reconstructed -> Branch("p", "vector<float>", &recon_p);
    //reconstructed -> Branch("pT", "vector<float>", &recon_pT);
    //reconstructed -> Branch("eta", "vector<float>", &recon_eta);
    //reconstructed -> Branch("phi", "vector<float>", &recon_phi);
    //reconstructed -> Branch("m", "float", &recon_m);
    //reconstructed -> Branch("Q", "float", &recon_Q);


    int Double_muon_counter = 0;
    int trigger_pass = 0;
    TH1F* histo = new TH1F("h1", "Mass of the reconstructed particle", 200, 0, 200);

    while(Reader.Next()) {

        if( p.GetSize()==2){
            if(Q[0]*Q[1] == -1){
                ++Double_muon_counter;

                if( abs(eta[0]) < 2.1 && abs(eta[1]) < 2.1 && pT[0] > 20 && pT[1] > 20){
                    ++ trigger_pass;

                // Using conversion formulas form here: https://en.wikipedia.org/wiki/Pseudorapidity
                // WE are also in Centre of mass frame where E = |p| (with the p being the 3-momentum)
                recon_m = sqrt(pow(m[0],2) + pow(m[1],2) + 2*p[0]*p[1] - 2*p[0]*p[1]*( cos(phi[0])*cos(phi[1])
                        + sin(phi[0])*sin(phi[1]) + sinh(eta[0])*sinh(eta[1]) ));

                //recon_Q = 0;

                //reconstructed -> Fill();
                histo -> Fill(recon_m);
                }
            }
        }
    }

    cout << "Number of double muons encountered: " << Double_muon_counter << endl;
    cout << "Number of double muons passing the trigger: " << trigger_pass << endl;


    TCanvas* c = new TCanvas("mass_canvas", "", 800, 600);

    c -> SetFillColor(42);
    histo -> SetLineColor(kBlack);
    histo -> SetFillColor(kYellow);
    histo -> GetXaxis() -> SetTitle("Mass");
    histo -> GetYaxis() -> SetTitle("Number of triggers");

    histo -> Draw();
    
    c -> Print("Reconstructed_mass.root");
    c -> Print("Reconstructed_mass.pdf");

   delete histo;
   delete c;


}
