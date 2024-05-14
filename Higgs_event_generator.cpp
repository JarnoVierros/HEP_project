
#include "Pythia8/Pythia.h"
#include "TTree.h"
#include "TFile.h"
using namespace std;
using namespace Pythia8;

int find_final_children(Event event, Particle parent, int daughter_indices[2]) {
    Particle daughter1 = event[parent.daughter1()];
    if (daughter1.id() != parent.id()) {
        daughter_indices[0] = parent.daughter1();
        daughter_indices[1] = parent.daughter2();
        return daughter1.id();
    } else {
        return find_final_children(event, daughter1, daughter_indices);
    }
}

int main() {

    const float Higgs_mass = 125;
    const int Higgs_id = 25;
    const int muon_id = 13;
    const int event_count = 100000;

    TFile out_file("Higgs_muon_decays.root","RECREATE");

    TTree metadata("metadata", "Additional information");

    int total_events = event_count;
    metadata.Branch("total_events", &total_events, "total_events/I");
    int muon_events = 0;
    metadata.Branch("muon_events", &muon_events, "muon_events/I");

    TTree muons("muons", "Higgs muon decays");

    float px1;
    muons.Branch("px1", &px1, "px1/F");
    float py1;
    muons.Branch("py1", &py1, "py1/F");
    float pz1;
    muons.Branch("pz1", &pz1, "pz1/F");
    float e1;
    muons.Branch("e1", &e1, "e1/F");
    float m1;
    muons.Branch("m1", &m1, "m1/F");

    float px2;
    muons.Branch("px2", &px2, "px2/F");
    float py2;
    muons.Branch("py2", &py2, "py2/F");
    float pz2;
    muons.Branch("pz2", &pz2, "pz2/F");
    float e2;
    muons.Branch("e2", &e2, "e2/F");
    float m2;
    muons.Branch("m2", &m2, "m2/F");

    Pythia pythia;
    pythia.readString("25:m0 = " + to_string(Higgs_mass));
    pythia.readString("HiggsSM:all = on");
    pythia.readString("Beams:eCM = 13600.");

    pythia.init();

    for (int iEvent = 0; iEvent < event_count; ++iEvent) {
        pythia.next();
        int higgs_index;
        for (int i=0; i<pythia.event.size(); ++i) {
            if (pythia.event[i].id() == Higgs_id) {
                int daughter_indices[2];
                int daughter_id = find_final_children(pythia.event, pythia.event[i], daughter_indices);
                //cout << daughter_id << ", " << daughter_indices[0] << ", " << daughter_indices[1] << endl;
                if (daughter_id == muon_id) {
                    ++muon_events;
                    Particle daughter1 = pythia.event[daughter_indices[0]];
                    Particle daughter2 = pythia.event[daughter_indices[1]];
                    px1 = daughter1.px();
                    py1 = daughter1.py();
                    pz1 = daughter1.pz();
                    e1 = daughter1.e();
                    m1 = daughter1.m();
                    px2 = daughter2.px();
                    py2 = daughter2.py();
                    pz2 = daughter2.pz();
                    e2 = daughter2.e();
                    m2 = daughter2.m();
                    muons.Fill();
                }
                break;
            }
        }
    }

    pythia.stat();

    cout << "Muon decays: " << muon_events <<endl;

    metadata.Fill();
    metadata.Write();
    muons.Write();
    out_file.Close();

    cout << "\"Higgs_muon_decays.root\" successfully written." << endl;

}