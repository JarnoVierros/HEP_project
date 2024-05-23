
#include <iostream>
#include <fstream>
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

    const int number_of_files_to_combine = stoi(settings["number_of_files_to_combine"]);

    TString out_file_name = settings["combined_file_name"] + ".root";
    TFile out_file(out_file_name,"RECREATE");

    TTree new_metadata("metadata", "Additional information");
    
    int new_total_events = 0;
    new_metadata.Branch("total_events", &new_total_events, "total_events/I");
    int new_muon_events = 0;
    new_metadata.Branch("muon_events", &new_muon_events, "muon_events/I");

    TTree new_muons("muons", "Muon data");

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

    int muon_event_count = 0;

    for (int i = 1; i<number_of_files_to_combine+1; i++) {
        string filename = settings["combine" + to_string(i)];
        if (filename.length() == 0) {
            continue;
        }
        TString TFilename = filename;
        TFile in_file(TFilename + ".root");

        TTree* old_metadata;
        in_file.GetObject("metadata", old_metadata);

        int old_total_events;
        old_metadata->SetBranchAddress("total_events", &old_total_events);

        int old_muon_events;
        old_metadata->SetBranchAddress("muon_events", &old_muon_events);
        old_metadata->GetEntry(0);

        cout << "total events: " << new_total_events << endl;
        cout << "muon events: " << muon_event_count << endl;

        new_total_events += old_total_events;
        new_muon_events += old_muon_events;


        TTreeReader Reader("muons",&in_file);

        TTreeReaderArray<float> old_px(Reader, "px");
        TTreeReaderArray<float> old_py(Reader, "py");
        TTreeReaderArray<float> old_pz(Reader, "pz");
        TTreeReaderArray<float> old_e(Reader, "e");
        TTreeReaderArray<float> old_m(Reader, "m");
        TTreeReaderArray<int> old_Q(Reader, "Q");
        TTreeReaderArray<int> old_H(Reader, "H");

        cout << "Reading file: " << filename << endl;

        while (Reader.Next()) {
            muon_event_count++;
            for (int j=0;j<old_px.GetSize();j++) {
                new_px.push_back(old_px[j]);
                new_py.push_back(old_py[j]);
                new_pz.push_back(old_pz[j]);
                new_e.push_back(old_e[j]);
                new_m.push_back(old_m[j]);
                new_Q.push_back(old_Q[j]);
                new_H.push_back(old_H[j]);
            }
            new_muons.Fill();
            new_px.clear();
            new_py.clear();
            new_pz.clear();
            new_e.clear();
            new_m.clear();
            new_Q.clear();
            new_H.clear();
        }
    }

    cout << "total events: " << new_total_events << endl;
    cout << "muon events: " << muon_event_count << endl;

    out_file.cd();
    new_metadata.Fill();
    new_metadata.Write();
    new_muons.Write();
    out_file.Close();

    cout << "\"" + out_file_name + "\" successfully written." << endl;

}