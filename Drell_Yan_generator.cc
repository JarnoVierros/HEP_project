
#include <ctime>
#include "Pythia8/Pythia.h"
#include "TTree.h"
#include "TFile.h"
#include <vector>
using namespace std;
using namespace Pythia8;



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

    const int muon_id = 13;
    //const int electron_id = 11;
    //const int tau_id = 15;

    //Neutrinos:
    /*
    const int eNeut_id =12;
    const int muNeut_id = 14;
    const int tauNeut_id = 16;
    */

    //Variables for tracking the number of leptons in events:
    int N_muon = 0;
    int N_anti_muon = 0;
    
    // These could be deleted later if no use (currently used for counting how many Drell Yann process actually happened)
    // int N_electron = 0;
    // int N_tau = 0;
    // int nDY = 0; // To count not DY processes

//    int Ne_muon = 0;
//    int Ne_electron = 0;
//    int Ne_tau = 0;

    const int eventsN = int(stoi(settings["Drell_Yan_generator_event_count"]));

    
    // Generator. Process selection
    Pythia pythia;

    pythia.readString("WeakZ0:gmZmode = 2");
    pythia.readString("23:onMode = 0");
    pythia.readString("23:onIfAny = 13");

    // 13.6 TeV centre of mass beam energy:
    pythia.readString("Beams:eCM = 13600.");

    //Turning off the QCD procesesses since we are simulating Drell Yann
    pythia.readString("HardQCD:all = off");
    pythia.readString("SoftQCD:all = off");

    //Turning on Drell-Yann process
    pythia.readString("WeakSingleBoson:ffbar2ffbar(s:gmZ) = on");

    //setting random seed:
    pythia.readString("Random:setSeed = on");
    pythia.readString("Random:seed = 0"); //This 0 means that a different seed is chosen based on time!
    //-------------------------------------------

    pythia.readString("Next:numberShowEvent = 0");
    pythia.init();






    //Let's now initialize the ROOT stuff:

    time_t now = time(0);
    string now_string = to_string(now);
    TString out_filename = "Drell_Yan_muons" + now_string + ".root";

    TFile* out_file = new TFile(out_filename, "RECREATE");
    TTree* metadata = new TTree("metadata", "Additional information");

    int total_events = eventsN;

    metadata -> Branch("total_events", &total_events, "total_events/I");
    metadata -> Branch("muon_events", &N_muon, "muon_events/I");
    
    TTree* muons = new TTree("muons", "Drell Yann muons");

    vector<float> px;
    vector<float> py;
    vector<float> pz;
    vector<float> e;
    vector<float> m;
    vector<int> Q;
    vector<int> H;

    muons -> Branch("px", "vector<float>" ,&px);
    muons -> Branch("py", "vector<float>" ,&py);
    muons -> Branch("pz", "vector<float>" ,&pz);
    muons -> Branch("e", "vector<float>" ,&e);
    muons -> Branch("m", "vector<float>" ,&m);
    muons -> Branch("Q", "vector<float>" ,&Q);
    muons -> Branch("H", "vector<float>" ,&H);


    //--------------------------------------------------------------


    //Time to begin the event loop:
    for (int iEvent = 0; iEvent < eventsN; ++iEvent){
        if (!pythia.next()) continue;
	
	//Vector size print for troubleshoot:
	//cout << "Vector size: " << px.size() << endl;
	
        // emptying the vectors every event:
        px.clear();
        py.clear();
        pz.clear();
        e.clear();
        m.clear();
        Q.clear();
        H.clear();

        for (int i=0; i < pythia.event.size(); ++i){
            
            /*
            if (pythia.event[i].id() == electron_id && pythia.event[i].isFinal()){
                N_electron += 1;
            }
            */
           
            if (pythia.event[i].id() == muon_id && pythia.event[i].isFinal()){
                ++ N_muon;
                
                px.push_back(pythia.event[i].px());
                py.push_back(pythia.event[i].py());
                pz.push_back(pythia.event[i].pz());
                e.push_back(pythia.event[i].e());
                m.push_back(pythia.event[i].m());
                Q.push_back(1);
                H.push_back(0);

                muons -> Fill();
		//cout << "Event: " << iEvent << ". Muon: " << px.size() << endl;
		

            }/* else if (pythia.event[i].id() == tau_id && pythia.event[i].isFinal()){
                N_tau += 1;
            } else if (pythia.event[i].id() == eNeut_id && pythia.event[i].isFinal()){
                Ne_electron += 1;
            } else if (pythia.event[i].id() == muNeut_id && pythia.event[i].isFinal()){
                Ne_muon += 1;
            } else if (pythia.event[i].id() == tauNeut_id && pythia.event[i].isFinal()){
                Ne_tau += 1;
            }*/ /*else {
                nDY += 1;
            }*/
            
            
            
            if (pythia.event[i].id() == -muon_id && pythia.event[i].isFinal()){
                ++ N_anti_muon;
                
                px.push_back(pythia.event[i].px());
                py.push_back(pythia.event[i].py());
                pz.push_back(pythia.event[i].pz());
                e.push_back(pythia.event[i].e());
                m.push_back(pythia.event[i].m());
                Q.push_back(-1);
                H.push_back(0);

                muons -> Fill();
                //cout << "Event: " << iEvent << ". Anti muon: " << px.size() << endl;
            }

            
        }

    }

    pythia.stat();

    /*
    cout << "Final state Electron, Muon, Tau lepton: " << N_electron <<", "<< N_muon << ", " << N_tau << endl;
    cout << "Final state anti muon: " << N_anti_muon << endl;
    cout << "Final state Neutrinos (electron, muon, tau): " << Ne_electron <<", "<< Ne_muon << ", " << Ne_tau << endl;
    int nLEP = (N_electron + N_muon + N_tau + Ne_electron + Ne_muon + Ne_tau);
    cout << "The number of leptons produced (doesn't include anti-leptons): " << nLEP << endl;
    //cout << "Not Drell Yan: " << nDY << endl;
    */

    cout << "Final state muon: " << N_muon << endl;
    cout << "Final state anti muon: " << N_anti_muon << endl;


    metadata -> Fill();
    metadata -> Write();
    muons -> Write();
    out_file -> Close();

    
    cout << "Yeahhh ..!" << endl;



}
