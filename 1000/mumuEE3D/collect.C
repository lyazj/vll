#include <classes/DelphesClasses.h>
#include <ExRootAnalysis/ExRootTreeReader.h>

int file_index(const string &f)
{
  size_t b = f.find('-');
  size_t e = f.find('/');
  if(b == f.npos || e == f.npos || b + 1 >= e)
    throw runtime_error("invalid filename: " + f);
  return atoi(f.substr(b + 1, e).c_str());
}

vector<string> get_rootfiles()
{
  vector<string> rootfiles;
  char rootfile[FILENAME_MAX + 2];  // "\n\0"

  FILE *p = popen("ls mumuEE-*/mumuEE/"
      "Events/run_01/mumuEE_delphes_events.root", "r");
  if(p == NULL)
    throw runtime_error("popen returned NULL");

  while(fgets(rootfile, sizeof rootfile, p))
  {
    char *ptr = strchr(rootfile, '\n');
    if(ptr == NULL)
      throw runtime_error("filename too long");
    *ptr = 0;
    rootfiles.push_back(rootfile);
  }

  if(ferror(p))
    throw runtime_error("pipe read error");
  pclose(p);

  sort(rootfiles.begin(), rootfiles.end(),
      [](const string &f1, const string &f2) {
        return file_index(f1) < file_index(f2);
      });
  return rootfiles;
}

bool near(const TLorentzVector &v1, const TLorentzVector &v2)
{
  return hypot(v1.Eta() - v2.Eta(), v1.Phi() - v2.Phi()) < 0.5;
}

void collect()
{
  gSystem->Load("libDelphes.so");

  TChain *Delphes = new TChain("Delphes");
  vector<string> rootfiles = get_rootfiles();
  for(string rootfile : rootfiles)
  {
    clog << "Loading: " << rootfile << endl;
    Delphes->Add(rootfile.c_str());
  }

  ExRootTreeReader *reader = new ExRootTreeReader(Delphes);
  TClonesArray *BrElectron = reader->UseBranch("Electron");
  TClonesArray *BrMuon = reader->UseBranch("Muon");
  TClonesArray *VLCjetR05 = reader->UseBranch("VLCjetR05_inclusive");
  Long64_t entries = reader->GetEntries();

  TFile *outfile = new TFile("mumuEE-collect.root", "recreate");
  TTree *Collect = new TTree("Collect", "Collected features for analysis");
  TClonesArray *BrJet = new TClonesArray("Jet", 10);
  Collect->Branch("Jet", &BrJet);

  for(Long64_t entry = 0; entry < entries; ++entry)
  {
    reader->ReadEntry(entry);

    Long64_t njet = VLCjetR05->GetEntries();
    Long64_t c = 0;

    for(Long64_t i = 0; i < njet; ++i)
    {
      Jet *jet = (Jet *)VLCjetR05->At(i);
      if(jet->BTag)  // TODO check why BTag out of 0 and 1
      {
        bool valid = 1;
        TLorentzVector pj = jet->P4();

        Long64_t nelectron = BrElectron->GetEntries();
        for(Long64_t j = 0; j < nelectron; ++j)
        {
          Electron *electron = (Electron *)BrElectron->At(j);
          if(near(pj, electron->P4()))
          {
            valid = 0;
            break;
          }
        }

        if(valid)
        {
          Long64_t nmuon = BrMuon->GetEntries();
          for(Long64_t j = 0; j < nmuon; ++j)
          {
            Muon *muon = (Muon *)BrMuon->At(j);
            if(near(pj, muon->P4()))
            {
              valid = 0;
              break;
            }
          }
        }

        if(valid)
          new((*BrJet)[c++]) Jet(*jet);
      }
    }
    Collect->Fill();
    BrJet->Clear();

    if((entry + 1) % 1000 == 0)
      clog << "Processing: "
           << setw(to_string(entries).length()) << (entry + 1)
           << " / " << entries << " entries processed" << endl;
  }

  Collect->Print();
  Collect->Write();
}
