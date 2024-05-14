
#include "Pythia8/Pythia.h"

using namespace Pythia8;

int main() {

    const float Higgs_mass = 125;
    const int event_count = 1;

    Pythia pythia;

    pythia.readString("25:m0 = " + to_string(Higgs_mass));
    pythia.readString("HiggsSM:all = on");
    pythia.readString("Beams:eCM = 13600.");

    pythia.init();

    for (int iEvent = 0; iEvent < event_count; ++iEvent) {
        pythia.next();
    }

    pythia.stat();

}