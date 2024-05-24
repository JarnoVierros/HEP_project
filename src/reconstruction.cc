
#include <iostream>
#include <fstream>
#include "TTree.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TH1F.h"
#include "TCanvas.h"

#include <string>
#include <vector>
#include <math.h>
using namespace std;


void read_settings(map<string, string>& settings) {
    string line;
    ifstream ReadFile("settings.txt");
    while (getline (ReadFile, line)) {
        if (line[0] == '#') { //skip line if it's commented
            continue;
        }
        string variable = "";
        string value = "";
        bool variable_name_area = true;
        for (int i=0; i<line.length(); i++) {
            if (line[i] == ' ' || line[i] == '\n') {
                continue;
            }
            if (line[i] == '=') {
                variable_name_area = false;
                continue;
            }
            if (variable_name_area) {
                variable += line[i];
            } else {
                value += line[i];
            }
        }
        settings[variable] = value;
    }
}

int main() {

    map<string, string> settings;
    read_settings(settings);
    cout << settings["reconstruction_filename"] << endl;
    TString filename = settings["reconstruction_filename"];
    
    TFile inFile(filename + ".root");

    TTreeReader Reader("muons",&inFile);

    TTreeReaderArray<float> p(Reader, "p");
    TTreeReaderArray<float> pT(Reader, "pT");
    TTreeReaderArray<float> eta(Reader, "eta");
    TTreeReaderArray<float> phi(Reader, "phi");
    TTreeReaderArray<float> m(Reader, "m");
    TTreeReaderArray<int> Q(Reader, "Q");
    TTreeReaderArray<int> H(Reader, "H");


    float recon_m;


    TTree* old_metadata;
    inFile.GetObject("metadata", old_metadata);

    int total_events;
    old_metadata->SetBranchAddress("total_events", &total_events);
    int muon_events;
    old_metadata->SetBranchAddress("muon_events", &muon_events);

    old_metadata->GetEntry(0);

    TFile out_file(filename + "_reconstructed_mass.root","RECREATE");

    TTree new_metadata("metadata", "Additional information");

    new_metadata.Branch("total_events", &total_events, "total_events/I");
    new_metadata.Branch("muon_events", &muon_events, "muon_events/I");

    new_metadata.Fill();


    TTree new_tree("reconstructed_masses", "Invariant mass of reconstructed two muon particle");

    float output_mass;
    int output_H;
    new_tree.Branch("m", &output_mass, "m/F");
    new_tree.Branch("H", &output_H, "H/I");

    int Double_muon_counter = 0;
    int trigger_pass = 0;
    TH1F* histo = new TH1F("reconstructed_mass", "Mass of the reconstructed particle", 200, 0, 250);
    TH1F* histo_eta = new TH1F("reconstructed_eta", "eta of the reconstructed particle", 200, -10, 10);
    TH1F* histo_pT = new TH1F("reconstructed_pT", "pT of the reconstructed particle", 200, -10, 200);

    const float eta_cut = 2.1; //2.1, 1
    const float pT_cut = 20; //20, 50
    const float reconstructed_pT_cut = 0; //90

    while(Reader.Next()) {

        vector<int> muon_indices;

        for (int i=0; i<p.GetSize(); i++) {
            if( abs(eta[i]) < eta_cut && pT[i] > pT_cut){
                muon_indices.push_back(i);
            }
        }

        if( muon_indices.size()==2){
            if(Q[muon_indices[0]]*Q[muon_indices[1]] == -1){
                ++Double_muon_counter;

                ++ trigger_pass;

                // Using conversion formulas form here: https://en.wikipedia.org/wiki/Pseudorapidity
                // WE are also in Centre of mass frame where (p_recon)^2 = (recon_m)^2
                recon_m = sqrt(pow(m[muon_indices[0]],2) + pow(m[muon_indices[1]],2) + 2*sqrt( pow(m[muon_indices[0]], 2) + pow(p[muon_indices[0]], 2) )*sqrt( pow(m[muon_indices[1]], 2) + pow(p[muon_indices[1]], 2) ) - 2*pT[muon_indices[0]]*pT[muon_indices[1]]*( cos(phi[muon_indices[0]])*cos(phi[muon_indices[1]])
                        + sin(phi[muon_indices[0]])*sin(phi[muon_indices[1]]) + sinh(eta[muon_indices[0]])*sinh(eta[muon_indices[1]]) ));

                float px0 = pT[muon_indices[0]]*cos(phi[muon_indices[0]]);
                float py0 = pT[muon_indices[0]]*sin(phi[muon_indices[0]]);
                float pz0 = pT[muon_indices[0]]*sinh(eta[muon_indices[0]]);
                float p0 = sqrt(px0*px0 + py0*py0 + pz0*pz0);
                float pT0 = sqrt(px0*px0 + py0*py0);

                float px1 = pT[muon_indices[1]]*cos(phi[muon_indices[1]]);
                float py1 = pT[muon_indices[1]]*sin(phi[muon_indices[1]]);
                float pz1 = pT[muon_indices[1]]*sinh(eta[muon_indices[1]]);
                float p1 = sqrt(px1*px1 + py1*py1 + pz1*pz1);
                float pT1 = sqrt(px1*px1 + py1*py1);

                float recon_eta = atanh((pz0+pz1)/sqrt(pow(px0+px1, 2)+pow(py0+py1, 2)+pow(pz0+pz1, 2)));
                float recon_pT = (pz0 + pz1)/sinh(recon_eta);

                if (recon_pT<reconstructed_pT_cut) {
                    continue;
                }

                histo -> Fill(recon_m);
                histo_eta->Fill(recon_eta);
                histo_pT->Fill(recon_pT);

                output_mass = recon_m;
                if (H[muon_indices[0]] == 1 && H[muon_indices[1]] == 1) {
                    output_H = 1;
                } else {
                    output_H = 0;
                }
                new_tree.Fill();
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
    
    c -> Print(filename + "_reconstructed_mass.pdf");

    TCanvas* eta_canvas = new TCanvas("eta_canvas", "", 800, 600);
    histo_eta->Draw();
    eta_canvas->Print(filename + "_eta_hist.pdf");

    TCanvas* pT_canvas = new TCanvas("pT_canvas", "", 800, 600);
    histo_pT->Draw();
    pT_canvas->Print(filename + "_pT_hist.pdf");
    
    new_metadata.Write();
    new_tree.Write();
    out_file.Close();

    //delete histo;
    //delete c;

}
