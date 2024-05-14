pythia_path := /home/jarno/pythia8311

all: 
	make event_generator
	
event_generator: event_generator.cpp
	$(CXX) event_generator.cpp -L $(pythia_path)/lib/ -l pythia8 -o event_generator
