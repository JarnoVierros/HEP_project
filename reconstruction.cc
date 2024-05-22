
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

    while(Reader.Next()) {

        if( p.GetSize()==2){
            if(Q[0]*Q[1] == -1){
                ++Double_muon_counter;

                if( abs(eta[0]) < 2.1 && abs(eta[1]) < 2.1 && pT[0] > 20 && pT[1] > 20){
                    ++ trigger_pass;

                    // Using conversion formulas form here: https://en.wikipedia.org/wiki/Pseudorapidity
                    // WE are also in Centre of mass frame where (p_recon)^2 = (recon_m)^2
                    recon_m = sqrt(pow(m[0],2) + pow(m[1],2) + 2*sqrt( pow(m[0], 2) + pow(p[0], 2) )*sqrt( pow(m[1], 2) + pow(p[1], 2) ) - 2*pT[0]*pT[1]*( cos(phi[0])*cos(phi[1])
                            + sin(phi[0])*sin(phi[1]) + sinh(eta[0])*sinh(eta[1]) ));


                    histo -> Fill(recon_m);

                    output_mass = recon_m;
                    if (H[0] == 1 && H[1] == 1) {
                        output_H = 1;
                    } else {
                        output_H = 0;
                    }
                    new_tree.Fill();
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
    
    c -> Print(filename + "_reconstructed_mass.pdf");
    
    new_metadata.Write();
    new_tree.Write();
    out_file.Close();

    //delete histo;
    //delete c;

}
