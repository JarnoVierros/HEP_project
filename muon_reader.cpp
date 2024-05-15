
#include <iostream>
#include "TTree.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"

using namespace std;

int main() {

    TFile in_file("Higgs_muon_decays.root");

    TTreeReader Reader("muons",&in_file);

    TTreeReaderArray<float> px(Reader, "px");
    TTreeReaderArray<float> py(Reader, "py");
    TTreeReaderArray<float> pz(Reader, "pz");
    TTreeReaderArray<float> e(Reader, "e");
    TTreeReaderArray<float> m(Reader, "m");
    TTreeReaderArray<int> Q(Reader, "Q");
    TTreeReaderArray<int> H(Reader, "H");


    TTree* muons;
    in_file.GetObject("muons", muons);


    while (Reader.Next()) {
        for (int j=0;j<px.GetSize();j++) {
            cout << "px: " << px[j] << ", py: " << py[j] << ", pz: " << pz[j] << ", e: " << e[j] << ", m: " << m[j] << ", Q: " << Q[j] << ", H: " << H[j] << endl;
        }
        cout << endl;
    }
}