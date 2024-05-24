pythia_path := /home/jarno/pythia8311

all: 
	make Higgs_event_generator
	make muon_reader
	make smeared_muon_reader
	make Drell_Yan_generator
	make ttbar_generator
	make muon_data_smearer
	make reconstruction
	make analysis
	make pt_cut_analysis
	make data_combiner
	make repair_muon_data
	
Higgs_event_generator: Higgs_event_generator.cpp
	$(CXX) Higgs_event_generator.cpp $$(root-config --glibs --cflags --libs) -L $(pythia_path)/lib/ -l pythia8 -o Higgs_event_generator.exe

muon_reader: muon_reader.cpp
	$(CXX) muon_reader.cpp $$(root-config --glibs --cflags --libs) -o muon_reader.exe

smeared_muon_reader: smeared_muon_reader.cpp
	$(CXX) smeared_muon_reader.cpp $$(root-config --glibs --cflags --libs) -o smeared_muon_reader.exe

Drell_Yan_generator: Drell_Yan_generator.cc
	$(CXX) Drell_Yan_generator.cc $$(root-config --glibs --cflags --libs) -L $(pythia_path)/lib/ -l pythia8 -o Drell_Yan_generator.exe

ttbar_generator: ttbar_generator.cc
	$(CXX) ttbar_generator.cc $$(root-config --glibs --cflags --libs) -L $(pythia_path)/lib/ -l pythia8 -o ttbar_generator.exe

muon_data_smearer: muon_data_smearer.cpp
	$(CXX) muon_data_smearer.cpp $$(root-config --glibs --cflags --libs) -o muon_data_smearer.exe

reconstruction: reconstruction.cc
	$(CXX) reconstruction.cc $$(root-config --glibs --cflags --libs) -o reconstruction.exe

analysis: analysis.cpp
	$(CXX) analysis.cpp $$(root-config --glibs --cflags --libs) -o analysis.exe

pt_cut_analysis: pt_cut_analysis.cpp
	$(CXX) pt_cut_analysis.cpp $$(root-config --glibs --cflags --libs) -o pt_cut_analysis.exe

data_combiner: data_combiner.cpp
	$(CXX) data_combiner.cpp $$(root-config --glibs --cflags --libs) -o data_combiner.exe

repair_muon_data: repair_muon_data.cpp
	$(CXX) repair_muon_data.cpp $$(root-config --glibs --cflags --libs) -o repair_muon_data.exe
	
clean:
	-rm *.exe
