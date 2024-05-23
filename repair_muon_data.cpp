
#include <iostream>
#include <fstream>
#include "TTree.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TRandom3.h"
#include <string>

using namespace std;

float calculate_p(float px, float py, float pz) {
    return sqrt(px*px + py*py + pz*pz);
}

float calculate_theta(float p, float pz){
    return acos(pz/p);
}

float calculate_eta(float theta) {
    return -log(tan(theta/2));
}

float calculate_eta(float px, float py, float pz) {
    return atanh(pz/calculate_p(px, py, pz));
}

float calculate_phi(float px, float py) {
    return atan2(py, px);
}

float calculate_pT(float p, float eta) {
    return p/cosh(eta);
}

void copy_from_to(const TTreeReaderArray<float> original, vector<float> *copy) {
    for (int i=0; i<original.GetSize(); i++) {
        copy->push_back(original[i]);
    }
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
    TString filename = settings["repair_filename"];

    //Creating objects for old data

    TFile in_file(filename + ".root");

    TTree* old_metadata;
    in_file.GetObject("metadata", old_metadata);

    int total_events;
    old_metadata->SetBranchAddress("total_events", &total_events);

    old_metadata->GetEntry(0);


    TTreeReader Reader("muons",&in_file);

    TTreeReaderArray<float> old_px(Reader, "px");
    TTreeReaderArray<float> old_py(Reader, "py");
    TTreeReaderArray<float> old_pz(Reader, "pz");
    TTreeReaderArray<float> old_e(Reader, "e");
    TTreeReaderArray<float> old_m(Reader, "m");
    TTreeReaderArray<int> old_Q(Reader, "Q");
    TTreeReaderArray<int> old_H(Reader, "H");


    //Creating objects for new data

    TFile out_file(filename + "_repaired.root","RECREATE");

    TTree new_metadata("metadata", "Additional information");

    new_metadata.Branch("total_events", &total_events, "total_events/I");
    int muon_event_count = 0;
    new_metadata.Branch("muon_events", &muon_event_count, "muon_events/I");

    


    TTree new_muons("muons", "Higgs muon decays with smearing");

    vector<float> new_px;
    vector<float> new_py;
    vector<float> new_pz;
    vector<float> new_e;
    vector<float> new_m;
    vector<int> new_Q;
    vector<int> new_H;

    new_muons.Branch("px", "vector<float>", &new_px);
    new_muons.Branch("py", "vector<float>", &new_py);
    new_muons.Branch("pz", "vector<float>", &new_pz);
    new_muons.Branch("e", "vector<float>", &new_e);
    new_muons.Branch("m", "vector<float>", &new_m);
    new_muons.Branch("Q", "vector<int>", &new_Q);
    new_muons.Branch("H", "vector<int>", &new_H);

    bool first_event = true;

    while (Reader.Next()) {

        if (old_px.GetSize() == 1 && !first_event) {
            new_muons.Fill();
            muon_event_count++;

            /*
            for (int i=0; i<new_px.size(); i++) {
                cout << new_px[i] << endl;
            }

            cout << endl;

            for (int i=0; i<new_py.size(); i++) {
                cout << new_py[i] << endl;
            }
            cout << endl;
            cout << endl;
            */
        }

        new_px.clear();
        new_py.clear();
        new_pz.clear();
        new_e.clear();
        new_m.clear();
        new_Q.clear();
        new_H.clear();
        
        for (int i=0; i<old_px.GetSize(); i++) {
            new_px.push_back(old_px[i]);
            new_py.push_back(old_py[i]);
            new_pz.push_back(old_pz[i]);
            new_e.push_back(old_e[i]);
            new_m.push_back(old_m[i]);
            new_Q.push_back(old_Q[i]);
            new_H.push_back(old_H[i]);
        }

        if (first_event) {first_event = false;}
    }

    new_metadata.Fill();
    new_metadata.Write();
    new_muons.Write();
    out_file.Close();

    cout << "\"" + filename + "_repaired.root\" successfully written." << endl;

}