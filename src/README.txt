This folder contains all of the source code and also the setting file for the source codes.

The settings.txt file in this directory is necessary to run the programs and by modifying the settings.txt you can change which files to analyse or how many events to simulate.
In the settings.txt file the lines starting with # are ignored.

The normalization coefficient depends on the amount of simulations ran with the following equation:
Normalization_coefficient = sigma_of_process * Integrated_Luminosity / Number_of_events_simulated
here sigma_of_process is the cross section one gets from simulation at the end (PYTHIA Event and Cross Section Statistics, use the summed cross section).
The Integrated_Luminosity is the integrated luminosity of a experiment we are trying to replicate.
In the default analysis of which result is in the run file, we have used value of 300 fb^{-1}.

It is adviced to look at settings.txt and understand how it works for the best user experience.


The commands to run to start simulation (you first have to change the pythia path to the correct directory in the makefile) are the following:

make Higgs_event_generator
make Drell_Yan_generator
make ttbar_generator

To start the data processing and analysis you have to run
make data_combiner
make muon_data_smearer
make reconstruction
make analysis

incase you want an alternative analysis with different normalization:
make alternative_analysis

The current version simulations don't give data files that need to be fixed, but for old data that need to be fixed the following command is useful:
make repair_muon_data

To read or inspect the data you can use the following commands (depending on if the data has been smeared or not):
make muon_reader
make smeared_muon_reader

To print the momentums you can use the following command:
make p_printer
