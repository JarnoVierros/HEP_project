
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

    while (Reader.Next()) {
        if (p.GetSize() != 2) {
            continue;
        }

        
        float m_H = sqrt(pow(m[0],2) + pow(m[1],2) + 2*p[0]*p[1] - 2*p[0]*p[1]*( cos(phi[0])*cos(phi[1]) + sin(phi[0])*sin(phi[1]) + sinh(eta[0])*sinh(eta[1]) ));
        cout << m_H << endl;
        cout << pow(m[0],2) + pow(m[1],2) + 2*p[0]*p[1] - 2*p[0]*p[1]*( cos(phi[0])*cos(phi[1]) + sin(phi[0])*sin(phi[1]) + sinh(eta[0])*sinh(eta[1]) ) << endl;
        
        cout << endl;
    }
}