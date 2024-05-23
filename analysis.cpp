
#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TF1.h"
#include "TGraphErrors.h"

using namespace std;

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

string create_interval(float start, float stop) {
    return "((("+to_string(start)+"<x) ? 1 : 0) - (("+to_string(stop)+"<x) ? 1 : 0))";
}

string get_par(TF1 fit, int par) {
    return to_string(fit.GetParameter(par));
}

int main() {

    const int bin_count = 41; //41, 1001
    const float lower_limit = 110; //110, 50
    const float upper_limit = 150; //150, 150

    map<string, string> settings;
    read_settings(settings);
    TString Higgs_filename = settings["analysis_Higgs_filename"];
    TString Drell_Yan_filename = settings["analysis_Drell_Yan_filename"];
    TString ttbar_filename = settings["analysis_ttbar_filename"];


    TFile Higgs_file(Higgs_filename + ".root");

    TTree* Higgs_masses_file;
    Higgs_file.GetObject("reconstructed_masses", Higgs_masses_file);

    float Higgs_m;
    Higgs_masses_file->SetBranchAddress("m", &Higgs_m);

    TH1F* Higgs_histo = new TH1F("Higgs_histo", "Normalized reconstructed mass distribution;m (GeV);events", bin_count, lower_limit, upper_limit);

    for (int i=0; i<Higgs_masses_file->GetEntries(); i++) {
        Higgs_masses_file->GetEntry(i);
        Higgs_histo->Fill(Higgs_m);
    }

    TFile Drell_Yan_file(Drell_Yan_filename + ".root");
    
    TTree* Drell_Yan_masses_file;
    Drell_Yan_file.GetObject("reconstructed_masses", Drell_Yan_masses_file);

    float Drell_Yan_m;
    Drell_Yan_masses_file->SetBranchAddress("m", &Drell_Yan_m);

    TH1F* Drell_Yan_histo = new TH1F("Drell_Yan_histo", "Reconstructed Drell-Yan event masses", bin_count, lower_limit, upper_limit);

    for (int i=0; i<Drell_Yan_masses_file->GetEntries(); i++) {
        Drell_Yan_masses_file->GetEntry(i);
        Drell_Yan_histo->Fill(Drell_Yan_m);
    }


    TFile ttbar_file(ttbar_filename + ".root");
    
    TTree* ttbar_masses_file;
    ttbar_file.GetObject("reconstructed_masses", ttbar_masses_file);

    float ttbar_m;
    ttbar_masses_file->SetBranchAddress("m", &ttbar_m);

    TH1F* ttbar_histo = new TH1F("ttbar_histo", "Reconstructed ttbar event masses", bin_count, lower_limit, upper_limit);

    for (int i=0; i<ttbar_masses_file->GetEntries(); i++) {
        ttbar_masses_file->GetEntry(i);
        ttbar_histo->Fill(ttbar_m);
    }

    TH1F *norm_Higgs_histo = (TH1F *)Higgs_histo->Clone("norm_Higgs_histo");
    float Higgs_coefficient = stof(settings["Higgs_normalization_coefficient"]);
    norm_Higgs_histo->Scale(Higgs_coefficient);

    TH1F *norm_Drell_Yan_histo = (TH1F *)Drell_Yan_histo->Clone("norm_Drell_Yan_histo");
    float Drell_Yan_coefficient = stof(settings["Drell_Yan_normalization_coefficient"]);
    norm_Drell_Yan_histo->Scale(Drell_Yan_coefficient);

    TH1F *norm_ttbar_histo = (TH1F *)ttbar_histo->Clone("norm_ttbar_histo");
    float ttbar_coefficient = stof(settings["ttbar_normalization_coefficient"]);
    norm_ttbar_histo->Scale(ttbar_coefficient);
    
    TH1F *combination_histo = (TH1F *)norm_Higgs_histo->Clone("combination_histo");
    combination_histo->Add(norm_ttbar_histo);
    combination_histo->Add(norm_Drell_Yan_histo);

    combination_histo->SetAxisRange(0, 160000, "Y"); //0, 150000

    TCanvas* combination_canvas = new TCanvas("combination_canvas", "", 800, 600);

    combination_histo->Draw();

    norm_Higgs_histo->Draw("SAME");
    norm_Higgs_histo->SetLineColor(kGreen);
    norm_ttbar_histo->Draw("SAME");
    norm_ttbar_histo->SetLineColor(kRed);
    norm_Drell_Yan_histo->Draw("SAME");
    norm_Drell_Yan_histo->SetLineColor(kBlack);

    combination_canvas->Print("figures/combination_histogram.pdf");


    TCanvas* background_fit_canvas = new TCanvas("background_fit_canvas", "", 800, 600);

    float fit_min = lower_limit;
    float fit_max = upper_limit;

    float signal_start = 123.5;
    float signal_stop = 125.5;

    string function_string = "(" + create_interval(1, signal_start) + " + " + create_interval(signal_stop, 150) + ")*" + "([0]*TMath::BreitWigner(x,[1],[2]) + [3])";
    TString function_TString = function_string;
    cout << "function string: " << function_TString << endl;

    TF1 background_BreitWigner_fit("background_BreitWigner_fit", function_TString, 50, 150);
    background_BreitWigner_fit.SetParameters(4.66888e+08, 90.5234, 0.810367, 1099.38);
    combination_histo->Fit(&background_BreitWigner_fit, "0","",fit_min, fit_max);

    combination_histo->Draw();


    TString background_fit_string = get_par(background_BreitWigner_fit, 0)+"*TMath::BreitWigner(x,"+get_par(background_BreitWigner_fit, 1)+","+get_par(background_BreitWigner_fit, 2)+")";
    TF1 background_fit_start("background_fit_start", background_fit_string, fit_min, signal_start);
    TF1 background_fit_middle("background_fit_start", background_fit_string, signal_start, signal_stop);
    background_fit_middle.SetLineStyle(2);
    TF1 background_fit_stop("background_fit_start", background_fit_string, signal_stop, fit_max);

    background_fit_start.DrawCopy("SAME");
    background_fit_middle.DrawCopy("SAME");
    background_fit_stop.DrawCopy("SAME");

    background_fit_canvas->Print("figures/background_fit.pdf");

    /*
    TCanvas* signal_fit_canvas = new TCanvas("signal_fit_canvas", "", 800, 600);

    //combination_histo->Draw();
    combination_histo->Draw();
    norm_Higgs_histo->Draw("SAME");

    function_string = "[0]*TMath::BreitWigner(x,125,[1]) + " + get_par(background_BreitWigner_fit, 0)+"*TMath::BreitWigner(x,"+get_par(background_BreitWigner_fit, 1)+","+get_par(background_BreitWigner_fit, 2)+")";
    function_TString = function_string;
    cout << "function string: " << function_TString << endl;

    TF1 signal_BreitWigner_fit("signal_BreitWigner_fit", function_TString, 50, 150);
    signal_BreitWigner_fit.SetParameters(8780.72, 2.69004);
    combination_histo->Fit(&signal_BreitWigner_fit, "","",fit_min, fit_max);

    TString isolated_signal_string = get_par(signal_BreitWigner_fit, 0)+"*TMath::BreitWigner(x,125,"+get_par(signal_BreitWigner_fit, 1)+")";
    TF1 isolated_signal("isolated_signal", isolated_signal_string, fit_min, fit_max);
    isolated_signal.DrawCopy("SAME");

    signal_fit_canvas->Print("figures/signal_fit.pdf");

    */

    TCanvas* no_background_canvas = new TCanvas("no_background_canvas", "", 800, 600);
    TF1 globalized_background_fit("globalized_background_fit", background_fit_string, fit_min, fit_max);

    int skip_at_start = 0;

    double x[bin_count-skip_at_start], y[bin_count-skip_at_start], xe[bin_count-skip_at_start], ye[bin_count-skip_at_start];

    for (int i=0; i<bin_count-skip_at_start; i++) {
        x[i] = combination_histo->GetBinCenter(i+1+skip_at_start);
        xe[i] = 0;
        y[i] = combination_histo->GetBinContent(i+1+skip_at_start) - globalized_background_fit(x[i]);
        ye[i] = combination_histo->GetBinError(i+1+skip_at_start);
        /*
        cout << "i: " << i << endl;
        cout << "x: " << x[i] << endl;
        cout << combination_histo->GetBinContent(i+1+skip_at_start) << endl;
        cout << globalized_background_fit(x[i]) << endl;
        cout << "y: " << y[i] << endl;
        cout << endl;
        */
    }


    TGraphErrors *excess = new TGraphErrors (bin_count-skip_at_start, x, y, xe, ye);

    excess->Draw("AP*");
    no_background_canvas->Print("figures/no_background.pdf");

}