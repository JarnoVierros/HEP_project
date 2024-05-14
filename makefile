pythia_path := /home/jarno/pythia8311

all: 
	make Higgs_event_generator
	
Higgs_event_generator: Higgs_event_generator.cpp
	$(CXX) Higgs_event_generator.cpp $$(root-config --glibs --cflags --libs) -L $(pythia_path)/lib/ -l pythia8 -o Higgs_event_generator.exe
