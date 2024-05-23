
#include <iostream>
#include <fstream>
#include <map>
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
    TString filename = settings["muon_reader_filename"];

    TFile in_file(filename);

    TTreeReader Reader("muons",&in_file);

    TTreeReaderArray<float> px(Reader, "px");
    TTreeReaderArray<float> py(Reader, "py");
    TTreeReaderArray<float> pz(Reader, "pz");
    TTreeReaderArray<float> e(Reader, "e");
    TTreeReaderArray<float> m(Reader, "m");
    TTreeReaderArray<int> Q(Reader, "Q");
    TTreeReaderArray<int> H(Reader, "H");

    while (Reader.Next()) {
        if (px.GetSize() != 2) {
            continue;
        }
        for (int j=0;j<px.GetSize();j++) {
            cout << "px: " << px[j] << ", py: " << py[j] << ", pz: " << pz[j] << ", e: " << e[j] << ", m: " << m[j] << ", Q: " << Q[j] << ", H: " << H[j] << endl;
            //float m_H = sqrt(pow(e[0] + e[1], 2) - (pow(px[0] + px[1], 2) + pow(py[0] + py[1], 2) + pow(pz[0] + pz[1], 2)));
            //cout << m_H << endl;
        }
        cout << endl;
    }
}