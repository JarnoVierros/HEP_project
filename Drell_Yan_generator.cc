
#include "Pythia8/Pythia.h"
#include "TTree.h"
#include "TFile.h"
using namespace std;
using namespace Pythia8;




int main() {


    const int muon_id = 13;
    const int electron_id = 11;
    const int tau_id = 15;

    const int eNeut_id =12;
    const int muNeut_id = 14;
    const int tauNeut_id = 16;

    //Variables for tracking the number of leptons in events:
    int N_muon = 0;
    
    //These could be deleted later if no use (currently used for counting how many Drell Yann process actually happened)
    int N_electron = 0;
    int N_tau = 0;
    //int nDY = 0; // To count not DY processes

    int Ne_muon = 0;
    int Ne_electron = 0;
    int Ne_tau = 0;

    const int eventsN = 1000;

    
    // Generator. Process selection
    Pythia pythia;

    // 13.6 TeV centre of mass beam energy:
    pythia.readString("Beams:eCM = 13600.");

    //Turning off the QCD procesesses since we are simulating Drell Yann
    pythia.readString("HardQCD:all = off");
    pythia.readString("SoftQCD:all = off");

    //Turning on Drell-Yann process
    pythia.readString("WeakSingleBoson:ffbar2ffbar(s:gmZ) = on");


    pythia.readString("Next:numberShowEvent = 0");
    pythia.init();






    //Let's now initialize the ROOT stuff:

    TFile* out_file = new TFile("Drell_Yan_muons.root", "RECREATE");
    TTree* metadata = new TTree("metadata", "Additional information");

    int total_events = eventsN;
    metadata -> Branch("total_events", &total_events, "total_events/I");

    metadata -> Branch("muon_events", &N_muon, "muon_events/I");
    
    TTree* muons = new TTree("muons", "Higgs muon decays");

    float px1;
    muons -> Branch("px1", &px1, "px1/F");
    float py1;
    muons -> Branch("py1", &py1, "py1/F");
    float pz1;
    muons -> Branch("pz1", &pz1, "pz1/F");
    float e1;
    muons -> Branch("e1", &e1, "e1/F");
    float m1;
    muons -> Branch("m1", &m1, "m1/F");


    //Leaving this here incase needed in future!
    /*
    float px2;
    muons -> Branch("px2", &px2, "px2/F");
    float py2;
    muons -> Branch("py2", &py2, "py2/F");
    float pz2;
    muons -> Branch("pz2", &pz2, "pz2/F");
    float e2;
    muons -> Branch("e2", &e2, "e2/F");
    float m2;
    muons -> Branch("m2", &m2, "m2/F");
    */


    //--------------------------------------------------------------


    //Time to begin the event loop:
    for (int iEvent = 0; iEvent < eventsN; ++iEvent){
        if (!pythia.next()) continue;

        for (int i=0; i < pythia.event.size(); ++i){
            if (pythia.event[i].id() == electron_id){
                N_electron += 1;
            } else if (pythia.event[i].id() == muon_id){
                ++ N_muon;
                
                px1 = pythia.event[i].px();
                py1 = pythia.event[i].py();
                pz1 = pythia.event[i].pz();
                e1 = pythia.event[i].e();
                m1 = pythia.event[i].m();

                muons -> Fill();


            } else if (pythia.event[i].id() == tau_id){
                N_tau += 1;
            } else if (pythia.event[i].id() == eNeut_id){
                Ne_electron += 1;
            } else if (pythia.event[i].id() == muNeut_id){
                Ne_muon += 1;
            } else if (pythia.event[i].id() == tauNeut_id){
                Ne_tau += 1;
            } /*else {
                nDY += 1;
            }*/
        }

    }

    pythia.stat();

    cout << "Electron, Muon, Tau lepton: " << N_electron <<", "<< N_muon << ", " << N_tau << endl;
    cout << "Neutrinos (electron, muon, tau): " << Ne_electron <<", "<< Ne_muon << ", " << Ne_tau << endl;
    int nLEP = 2* (N_electron + N_muon + N_tau + Ne_electron + Ne_muon + Ne_tau);
    cout << "The number of leptons produced: " << nLEP << endl;
    //cout << "Not Drell Yan: " << nDY << endl;

    metadata -> Fill();
    metadata -> Write();
    muons -> Write();
    out_file -> Close();

    cout << "Yeahhh ..!" << endl;



}
