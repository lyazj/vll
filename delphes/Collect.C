#include <external/ExRootAnalysis/ExRootTreeReader.h>
#include <classes/DelphesClasses.h>

// for memory sharing and process control
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <err.h>

#define NUMBER_MAX 1024

#undef assert
#define assert(expr) do { \
  if(!(expr)) throw runtime_error("assertion failed: " #expr); \
} while(0)

vector<string> branch_names = {
  // "Event",
  // "Weight",
  // "Particle",
  // "GenJet",
  // "KTjet",
  // // "VLCjetR05N2",
  // // "VLCjetR05N3",
  // // "VLCjetR05N4",
  // // "VLCjetR05N5",
  // // "VLCjetR05N6",
  // // "VLCjetR07N2",
  // // "VLCjetR07N3",
  // // "VLCjetR07N4",
  // // "VLCjetR07N5",
  // // "VLCjetR07N6",
  // // "VLCjetR10N2",
  // // "VLCjetR10N3",
  // // "VLCjetR10N4",
  // // "VLCjetR10N5",
  // // "VLCjetR10N6",
  // // "VLCjetR12N2",
  // // "VLCjetR12N3",
  // // "VLCjetR12N4",
  // // "VLCjetR12N5",
  // // "VLCjetR12N6",
  // // "VLCjetR15N2",
  // // "VLCjetR15N3",
  // // "VLCjetR15N4",
  // // "VLCjetR15N5",
  // // "VLCjetR15N6",
  // // "VLCjetR02_inclusive",
  // "VLCjetR05_inclusive",
  // // "VLCjetR07_inclusive",
  // // "VLCjetR10_inclusive",
  // // "VLCjetR12_inclusive",
  // // "VLCjetR15_inclusive",
  // "GenMissingET",
  // "Track",
  // "Tower",
  // "EFlowTrack",
  // "EFlowPhoton",
  // "EFlowNeutralHadron",
  // "Photon",
  // "Electron",
  // "Muon",
  // "ForwardMuon",
  // "MissingET",
  // "ScalarHT",
};

vector<string> directories = {
  "../500/mumuEE10D",
  "../500/mumuEE3D",
  "../500/mumuNN10D",
  "../500/mumuNN3D",
  "../1000/mumuEE10D",
  "../1000/mumuEE3D",
  "../1000/mumuNN10D",
  "../1000/mumuNN3D",
  "../1500/mumuEE10D",
  "../1500/mumuEE3D",
  "../1500/mumuNN10D",
  "../1500/mumuNN3D",
  "../2000/mumuEE10D",
  "../2000/mumuEE3D",
  "../2000/mumuNN10D",
  "../2000/mumuNN3D",
  "../3000/mumuEE10D",
  "../3000/mumuEE3D",
  "../3000/mumuNN10D",
  "../3000/mumuNN3D",
  "../bg/mumubb10D",
  "../bg/mumubb3D",
  "../bg/mumubbz10D",
  "../bg/mumubbz3D",
  "../bg/mumutata10D",
  "../bg/mumutata3D",
  "../bg/mumutataz10D",
  "../bg/mumutataz3D",
  "../bg/mumutt10D",
  "../bg/mumutt3D",
  "../bg/mumuttz10D",
  "../bg/mumuttz3D",
  "../bg/mumuvbsbb10D",
  "../bg/mumuvbsbb3D",
  "../bg/mumuvbstata10D",
  "../bg/mumuvbstata3D",
  "../bg/mumuvbstt10D",
  "../bg/mumuvbstt3D",
  "../bg/mumuvbsww10D",
  "../bg/mumuvbsww3D",
  "../bg/mumuww10D",
  "../bg/mumuww3D",
  "../bg/mumuwwz10D",
  "../bg/mumuwwz3D",
};

Long64_t entries_max = 50000;

struct Initializer {

  Initializer()
  {
    static int init = 1;
    if(!init)
      return;
    init = 0;

    auto it = unique(branch_names.begin(), branch_names.end());
    branch_names.erase(it, branch_names.end());
  }

} initializer;

size_t ncpu()
{
  size_t n;
  FILE *p = popen("cat /proc/cpuinfo | grep processor | wc -l", "r");
  if(fscanf(p, "%zu", &n) != 1)
    throw runtime_error("error getting processor number");
  pclose(p);
  return n;
}

bool split(string path, string &dir, string &base)
{
  size_t d = path.rfind('/');
  if(d == path.npos)
  {
    dir = "";
    base = path;
    return 0;
  }
  dir = path.substr(0, d);
  base = path.substr(d + 1);
  return 1;
}

string get_label(const string &path,
    string *cme = NULL, string *delphes = NULL, string *mass = NULL)
{
  string dir = path;
  string base;
  while(base.empty() && split(dir, dir, base));
  if(base.empty())
    throw runtime_error("invalid path: " + path);
  auto c = find_if(base.begin(), base.end(), [](char c){ return isdigit(c); });
  auto d = find(c, base.end(), 'D');
  if(cme)
    *cme = {c, d};
  if(delphes)
    *delphes = {d, base.end()};
  if(mass)
  {
    mass->clear();
    while(mass->empty() && split(dir, dir, *mass));
  }
  return { base.begin(), c };
}

int file_index(const string &f)
{
  size_t b = f.find('-');
  if(b == f.npos)
    throw runtime_error("invalid filename: " + f);
  size_t e = f.find('/', b + 1);  // it's OK if e == f.npos
  return atoi(f.substr(b + 1, e).c_str());
}

vector<string> get_rootfiles(const string &directory)
{
  vector<string> rootfiles;
  char rootfile[FILENAME_MAX + 2];  // "\n\0"

  string label = get_label(directory);

  FILE *p = popen(("ls \"" + directory + "\"/" + label + "-*/" + label + "/"
      + "Events/run_01/" + label + "_delphes_events.root").c_str(), "r");
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

void Collect(const string &directory, bool minmode = 0)
{
  string cme, mass;
  string label = get_label(directory, &cme, NULL, &mass);

  TChain *Delphes = new TChain("Delphes");
  vector<string> rootfiles = get_rootfiles(directory);
  for(string rootfile : rootfiles)
    Delphes->Add(rootfile.c_str());

  ExRootTreeReader *reader = new ExRootTreeReader(Delphes);
  TClonesArray *BrEvent = reader->UseBranch("Event");
  TClonesArray *BrWeight = reader->UseBranch("Weight");
  TClonesArray *BrElectron = reader->UseBranch("Electron");
  TClonesArray *BrMuon = reader->UseBranch("Muon");
  TClonesArray *VLCjetR05 = reader->UseBranch("VLCjetR05_inclusive");
  TClonesArray *BrMissingET = reader->UseBranch("MissingET");
  Long64_t entries = min(entries_max, reader->GetEntries());

  TFile *outfile = new TFile((label + "_collect_" + (minmode ? "min_" : "")
        + mass + "_" + cme + ".root").c_str(), "recreate");
  TTree *Collect = new TTree("Collect", "Collected features for analysis");

  Float_t *BrHepMCWeight = new Float_t;
  Collect->Branch("HepMCWeight", BrHepMCWeight, "HepMCWeight/F");

  Float_t *BrHepMCCS = new Float_t;
  Collect->Branch("HepMCCS", BrHepMCCS, "HepMCCS/F");

  Float_t *BrW = new Float_t[2];
  Collect->Branch("Weight", BrW, "Weight[2]/F");

  UInt_t *BrJetNumber = new UInt_t;
  Collect->Branch("JetNumber", BrJetNumber, "JetNumber/i");

  Float_t *BrJetPT = new Float_t[NUMBER_MAX];
  Collect->Branch("JetPT", BrJetPT, "JetPT[JetNumber]/F");

  Float_t *BrJetEta = new Float_t[NUMBER_MAX];
  Collect->Branch("JetEta", BrJetEta, "JetEta[JetNumber]/F");

  Float_t *BrJetPhi = new Float_t[NUMBER_MAX];
  Collect->Branch("JetPhi", BrJetPhi, "JetPhi[JetNumber]/F");

  Float_t *BrJetE = new Float_t[NUMBER_MAX];
  Collect->Branch("JetE", BrJetE, "JetE[JetNumber]/F");

  UInt_t *BrJetBTag = new UInt_t[NUMBER_MAX];
  Collect->Branch("JetBTag", BrJetBTag, "JetBTag[JetNumber]/i");

  UInt_t *BrJetTauTag = new UInt_t[NUMBER_MAX];
  Collect->Branch("JetTauTag", BrJetTauTag, "JetTauTag[JetNumber]/i");

  UInt_t *BrElectronNumber = new UInt_t;
  Collect->Branch("ElectronNumber", BrElectronNumber, "ElectronNumber/i");

  Float_t *BrElectronPT = new Float_t[NUMBER_MAX];
  Collect->Branch("ElectronPT", BrElectronPT, "ElectronPT[ElectronNumber]/F");

  Float_t *BrElectronEta = new Float_t[NUMBER_MAX];
  Collect->Branch("ElectronEta", BrElectronEta, "ElectronEta[ElectronNumber]/F");

  Float_t *BrElectronPhi = new Float_t[NUMBER_MAX];
  Collect->Branch("ElectronPhi", BrElectronPhi, "ElectronPhi[ElectronNumber]/F");

  Float_t *BrElectronE = new Float_t[NUMBER_MAX];
  Collect->Branch("ElectronE", BrElectronE, "ElectronE[ElectronNumber]/F");

  UInt_t *BrMuonNumber = new UInt_t;
  Collect->Branch("MuonNumber", BrMuonNumber, "MuonNumber/i");

  Float_t *BrMuonPT = new Float_t[NUMBER_MAX];
  Collect->Branch("MuonPT", BrMuonPT, "MuonPT[MuonNumber]/F");

  Float_t *BrMuonEta = new Float_t[NUMBER_MAX];
  Collect->Branch("MuonEta", BrMuonEta, "MuonEta[MuonNumber]/F");

  Float_t *BrMuonPhi = new Float_t[NUMBER_MAX];
  Collect->Branch("MuonPhi", BrMuonPhi, "MuonPhi[MuonNumber]/F");

  Float_t *BrMuonE = new Float_t[NUMBER_MAX];
  Collect->Branch("MuonE", BrMuonE, "MuonE[MuonNumber]/F");

  UInt_t *BrLeptonNumber = new UInt_t;
  Collect->Branch("LeptonNumber", BrLeptonNumber, "LeptonNumber/i");

  Float_t *BrMET = new Float_t;
  Collect->Branch("MET", BrMET, "MET/F");

  Float_t *BrMETEta = new Float_t;
  Collect->Branch("METEta", BrMETEta, "METEta/F");

  Float_t *BrMETPhi = new Float_t;
  Collect->Branch("METPhi", BrMETPhi, "METPhi/F");

  for(const string &branch_name : branch_names)
  {
    TClonesArray *array;
    if(branch_name == "Event")
      array = BrEvent;
    else if(branch_name == "Weight")
      array = BrWeight;
    else if(branch_name == "Electron")
      array = BrElectron;
    else if(branch_name == "Muon")
      array = BrMuon;
    else if(branch_name == "VLCjetR05_inclusive")
      array = VLCjetR05;
    else if(branch_name == "MissingET")
      array = BrMissingET;
    else
      array = reader->UseBranch(branch_name.c_str());
    Collect->Branch(branch_name.c_str(), array);
  }

  for(Long64_t entry = 0; entry < entries; ++entry)
  {
    reader->ReadEntry(entry);

    Long64_t nevent = BrEvent->GetEntries();
    Long64_t nweight = BrWeight->GetEntries();
    Long64_t nelectron = BrElectron->GetEntries();
    Long64_t nmuon = BrMuon->GetEntries();
    Long64_t njet = VLCjetR05->GetEntries();
    Long64_t nmet = BrMissingET->GetEntries();
    Long64_t c = 0;

    assert(nevent == 1);
    assert(nweight == 2);
    assert(nelectron <= NUMBER_MAX);
    assert(nmuon <= NUMBER_MAX);
    assert(njet <= NUMBER_MAX);
    assert(nmet == 1);
    *BrElectronNumber = nelectron;
    *BrMuonNumber = nmuon;
    *BrLeptonNumber = nelectron + nmuon;

    HepMCEvent *event = (HepMCEvent *)BrEvent->At(0);
    *BrHepMCWeight = event->Weight;
    *BrHepMCCS = event->CrossSection;

    Weight *weight = (Weight *)BrWeight->At(0);
    BrW[0] = weight->Weight;
    weight = (Weight *)BrWeight->At(1);
    BrW[1] = weight->Weight;

    vector<TLorentzVector> pes;
    for(Long64_t i = 0; i < nelectron; ++i)
    {
      Electron *electron = (Electron *)BrElectron->At(i);
      TLorentzVector pe = electron->P4();
      pes.push_back(pe);
      BrElectronPT[i] = pe.Pt();
      BrElectronEta[i] = pe.Eta();
      BrElectronPhi[i] = pe.Phi();
      BrElectronE[i] = pe.E();
    }

    vector<TLorentzVector> pms;
    for(Long64_t i = 0; i < nmuon; ++i)
    {
      Muon *muon = (Muon *)BrMuon->At(i);
      TLorentzVector pm = muon->P4();
      pms.push_back(pm);
      BrMuonPT[i] = pm.Pt();
      BrMuonEta[i] = pm.Eta();
      BrMuonPhi[i] = pm.Phi();
      BrMuonE[i] = pm.E();
    }

    for(Long64_t i = 0; i < njet; ++i)
    {
      Jet *jet = (Jet *)VLCjetR05->At(i);
      bool valid = 1;
      TLorentzVector pj = jet->P4();

      for(const TLorentzVector &pe : pes)
      {
        if(pe.Pt() > 20 && near(pj, pe))
        {
          valid = 0;
          break;
        }
      }

      if(valid) for(const TLorentzVector &pm : pms)
      {
        if(pm.Pt() > 20 && near(pj, pm))
        {
          valid = 0;
          break;
        }
      }

      if(valid)
      {
        BrJetPT[c] = pj.Pt();
        BrJetEta[c] = pj.Eta();
        BrJetPhi[c] = pj.Phi();
        BrJetE[c] = pj.E();
        BrJetBTag[c] = jet->BTag;
        BrJetTauTag[c] = jet->TauTag;
        ++c;
      }

    }
    *BrJetNumber = c;

    MissingET *missingET = (MissingET *)BrMissingET->At(0);
    *BrMET = missingET->MET;
    *BrMETEta = missingET->Eta;
    *BrMETPhi = missingET->Phi;

    Collect->Fill();
    if((entry + 1) % 1000 == 0)
      clog << "Processing " << directory << ": "
        << setw(to_string(entries).length()) << (entry + 1)
        << " / " << entries << " entries processed\n";
  }

  // Collect->Print();
  Collect->Write();

  delete BrMETPhi;
  delete BrMETEta;
  delete BrMET;
  delete BrLeptonNumber;
  delete[] BrMuonE;
  delete[] BrMuonPhi;
  delete[] BrMuonEta;
  delete[] BrMuonPT;
  delete BrMuonNumber;
  delete[] BrElectronE;
  delete[] BrElectronPhi;
  delete[] BrElectronEta;
  delete[] BrElectronPT;
  delete BrElectronNumber;
  delete[] BrJetTauTag;
  delete[] BrJetBTag;
  delete[] BrJetE;
  delete[] BrJetPhi;
  delete[] BrJetEta;
  delete[] BrJetPT;
  delete BrJetNumber;
  delete[] BrW;
  delete BrHepMCCS;
  delete BrHepMCWeight;
  delete Collect;
  delete outfile;
  delete reader;
  delete Delphes;
}

void Collect(const vector<string> &directories = ::directories, bool minmode = 0)
{
  atomic_size_t *directory_index;
  directory_index = (atomic_size_t *)mmap(NULL, sizeof *directory_index,
      PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if(directory_index == MAP_FAILED)
    err(1, "mmap");

  size_t nworker = min(ncpu(), directories.size());
  for(size_t w = 0; w < nworker; ++w)
  {
    pid_t pid = fork();
    if(pid < 0)
      err(1, "fork");
    if(pid == 0)  // child
    {
      size_t i;
      while((i = (*directory_index)++) < directories.size())
        Collect(directories[i], minmode);
      exit(0);
    }
  }

  for(size_t w = 0; w < nworker; ++w)
    if(wait(NULL) < 0)
      err(1, "wait");
  if(munmap(directory_index, sizeof *directory_index) < 0)
    err(1, "munmap");
}
