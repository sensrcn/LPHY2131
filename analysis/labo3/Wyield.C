// this macro allows to make a standard plot with data and monte carlo superimposed
// while fitting the signal and background to data
// signal comes from MC
// background from a control data sample
{

///////////////////////////////////////////////////////////////////////////////
//  CONFIGURATION                                                            //
///////////////////////////////////////////////////////////////////////////////

// constants to be adapted if you want
TString var = "transvMass";
Int_t bins = 25;
Float_t low = 20;
Float_t high = 120;
// the changes in cuts must be done here
TString cuts_signalregion  = "nElectrons>=1";
TString cuts_controlregion = "nElectrons>=1";
// input files
// these points to the default files for this lab. 
// data_file is the data ntuple you want to use. Pick either electrons or muons 
// by uncommenting one of the lines below.
//TFile* data_file = TFile::Open("singleEl2011_ntuple.root");
//TFile* data_file = TFile::Open("singleMu2011_ntuple.root");
TFile* simu_file = TFile::Open("ppChargedCurrentFullsim_ntuple.root");

///////////////////////////////////////////////////////////////////////////////
//  ANALYSIS SCRIPT - ESTIMATES THE W YIELD AND PURITY                       //
///////////////////////////////////////////////////////////////////////////////


// some initialization
gStyle->SetOptFit(111111);

// create the histograms to fill
TH1F* hdata = new TH1F(Form("%s_data",(const char*)var),var,bins,low,high);
hdata->Sumw2();
TH1F* hsimu = new TH1F(Form("%s_simu",(const char*)var),var,bins,low,high);
hsimu->Sumw2();
TH1F* hbkg = new TH1F(Form("%s_bkg",(const char*)var),var,bins,low,high);
hbkg->Sumw2();

// fill and draw the histograms
TCanvas* c1 = new TCanvas;
data_file->cd("LPHY2131analysis");
hdata->SetDirectory(gDirectory);
WeakBosonsAnalysis->Draw(Form("%s>>%s_data",(const char*)var,(const char*)var),cuts_signalregion,"E0 P0");
hdata->SetMarkerStyle(20);
hbkg->SetDirectory(gDirectory);
WeakBosonsAnalysis->Draw(Form("%s>>%s_bkg",(const char*)var,(const char*)var),cuts_controlregion,"HIST SAME");
hbkg->SetFillColor(kBlue);
hbkg->Scale(hdata->GetEntries()/hbkg->GetEntries()/2.);
simu_file->cd("LPHY2131analysis");
hsimu->SetDirectory(gDirectory);
WeakBosonsAnalysis->Draw(Form("%s>>%s_simu",(const char*)var,(const char*)var),cuts_signalregion,"HIST SAME");
hsimu->Scale(hdata->GetEntries()/hsimu->GetEntries()/2.);
hsimu->SetFillColor(kYellow);
hsimu->SetFillStyle(3008);
//hdata->Draw("same");
hdata->Draw();
hbkg->DrawClone("hist same");
hsimu->DrawClone("hist same");
hdata->Draw("same");

// do the fit
TCanvas* c2 = new TCanvas;
TObjArray *contributions = new TObjArray(2);
contributions->Add(hbkg);
contributions->Add(hsimu);
TFractionFitter* fit = new TFractionFitter(hdata,contributions);
fit->Constrain(0,0.0,1.0);
fit->Constrain(1,0.0,1.0);
fit->SetRangeX(1,bins); // full range. This could be restricted.
fit->Fit();
hdata->Draw();
fit->GetPlot()->Draw("same");
//delete c2;

// combined plot
TCanvas* c3 = new TCanvas;
Double_t value,error;
fit->GetResult(0,value,error);
float bkg_sf = hdata->Integral()*value/hbkg->Integral();
fit->GetResult(1,value,error);
float sig_sf = hdata->Integral()*value/hsimu->Integral();
hbkg->Scale(bkg_sf);
hsimu->Scale(sig_sf);
THStack* stack = new THStack;
stack->Add(hbkg);
stack->Add(hsimu);
if(stack->GetMaximum()>hdata->GetMaximum()) {
  stack->Draw("hist");
  hdata->Draw("same");
} else {
  hdata->Draw();
  stack->Draw("hist same");
  hdata->Draw("same");
}
// add a legend
leg = new TLegend(0.15, 0.68, 0.35,0.88);
leg->AddEntry(hdata,"data","l");
leg->AddEntry(hsimu,"signal MC (NLO)","f");
leg->AddEntry(hbkg,"background (from data)","f");
leg->SetBorderSize(0);
leg->SetShadowColor(0);
leg->SetFillColor(0);
leg->Draw("same");

// some final printout

std::cout << std::fixed;
std::cout << std::setprecision(6);
std::cout << "signal purity: " << value << "+/-" << error << std::endl;
std::cout << std::setprecision(0);
std::cout << "N_mc = " << hsimu->GetEntries() << std::endl;
std::cout << "N_data = " << hdata->Integral() << std::endl;
std::cout << std::setprecision(2);
std::cout << "N_W = " << hdata->Integral()*value << std::endl;
std::cout << "N_bkg = " << hdata->Integral()*(1-value) << std::endl;
std::cout << std::setprecision(6);
std::cout << "corresponding scale factor: " << hdata->Integral()*value/hsimu->GetEntries() << std::endl;

// conclude
gPad->Update();
}

