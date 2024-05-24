
#include <iostream>
#include <fstream>
#include "TTree.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"

using namespace std;

float p(float px, float py, float pz) {
    return sqrt(px*px + py*py +pz*pz);
}

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
    TString filename = settings["smeared_muon_reader_filename"];

    TFile in_file(filename);

    TTreeReader Reader("muons",&in_file);

    TTreeReaderArray<float> p(Reader, "p");
    TTreeReaderArray<float> pT(Reader, "pT");
    TTreeReaderArray<float> eta(Reader, "eta");
    TTreeReaderArray<float> phi(Reader, "phi");
    TTreeReaderArray<float> m(Reader, "m");
    TTreeReaderArray<int> Q(Reader, "Q");
    TTreeReaderArray<int> H(Reader, "H");

    const float eta_cut = 2.1;
    const float pT_cut = 20;

    int total_muon_decays = 0;
    int triggered_muon_decays = 0;

    while (Reader.Next()) {

        int valid_muons = 0;
        int higgs_muons = 0;

        for (int i=0; i<p.GetSize(); i++) {
            if(abs(eta[i]) < eta_cut && pT[i] > pT_cut){
                valid_muons++;
            }
            if (H[i] == 1) {
                higgs_muons++;
            }
        }

        if (higgs_muons >= 2) {
            total_muon_decays++;
            if (valid_muons >= 2) {
                triggered_muon_decays++;
            }
        }

    }

    cout << "Total muon decays: " << total_muon_decays << endl;
    cout << "Triggered muon decays: " << triggered_muon_decays << endl;
}