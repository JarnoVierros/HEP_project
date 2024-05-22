
#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TTree.h"

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

    const int bin_count = 25;
    const int lower_limit = 100;
    const int upper_limit = 150;

    map<string, string> settings;
    read_settings(settings);
    TString Higgs_filename = settings["analysis_Higgs_filename"];
    TString Drell_Yan_filename = settings["analysis_Drell_Yan_filename"];
    TString ttbar_filename = settings["analysis_ttbar_filename"];


    TFile Higgs_file(Higgs_filename + ".root");

    TTree* Higgs_masses_file;
    Higgs_file.GetObject("reconstructed_masses", Higgs_masses_file);

    float Higgs_m;
    Higgs_masses_file->SetBranchAddress("m", &Higgs_m);

    TH1F* Higgs_histo = new TH1F("Higgs_histo", "Normalized reconstructed mass distribution;m (GeV);events", bin_count, lower_limit, upper_limit);

    for (int i=0; i<Higgs_masses_file->GetEntries(); i++) {
        Higgs_masses_file->GetEntry(i);
        Higgs_histo->Fill(Higgs_m);
    }


    TFile Drell_Yan_file(Drell_Yan_filename + ".root");
    
    TTree* Drell_Yan_masses_file;
    Drell_Yan_file.GetObject("reconstructed_masses", Drell_Yan_masses_file);

    float Drell_Yan_m;
    Drell_Yan_masses_file->SetBranchAddress("m", &Drell_Yan_m);

    TH1F* Drell_Yan_histo = new TH1F("Drell_Yan_histo", "Reconstructed Drell-Yan event masses", bin_count, lower_limit, upper_limit);

    for (int i=0; i<Drell_Yan_masses_file->GetEntries(); i++) {
        Drell_Yan_masses_file->GetEntry(i);
        Drell_Yan_histo->Fill(Drell_Yan_m);
    }


    TFile ttbar_file(ttbar_filename + ".root");
    
    TTree* ttbar_masses_file;
    ttbar_file.GetObject("reconstructed_masses", ttbar_masses_file);

    float ttbar_m;
    ttbar_masses_file->SetBranchAddress("m", &ttbar_m);

    TH1F* ttbar_histo = new TH1F("ttbar_histo", "Reconstructed ttbar event masses", bin_count, lower_limit, upper_limit);

    for (int i=0; i<ttbar_masses_file->GetEntries(); i++) {
        ttbar_masses_file->GetEntry(i);
        ttbar_histo->Fill(ttbar_m);
    }

    TH1F *norm_Higgs_histo = (TH1F *)Higgs_histo->Clone("norm_Higgs_histo");
    float Higgs_coefficient = stof(settings["Higgs_normalization_coefficient"]);
    norm_Higgs_histo->Scale(Higgs_coefficient);

    TH1F *norm_Drell_Yan_histo = (TH1F *)Drell_Yan_histo->Clone("norm_Drell_Yan_histo");
    float Drell_Yan_coefficient = stof(settings["Drell_Yan_normalization_coefficient"]);
    norm_Drell_Yan_histo->Scale(Drell_Yan_coefficient);

    TH1F *norm_ttbar_histo = (TH1F *)ttbar_histo->Clone("norm_ttbar_histo");
    float ttbar_coefficient = stof(settings["ttbar_normalization_coefficient"]);
    norm_ttbar_histo->Scale(ttbar_coefficient);
    
    TH1F *combination_histo = (TH1F *)norm_Higgs_histo->Clone("combination_histo");
    combination_histo->Add(norm_ttbar_histo);
    combination_histo->Add(norm_Drell_Yan_histo);

    TCanvas* canvas = new TCanvas("normalized_mass_canvas", "", 800, 600);

    combination_histo->Draw();

    cout << combination_histo->Integral() << endl;
    
    canvas->Print("combination_histogram.pdf");

}