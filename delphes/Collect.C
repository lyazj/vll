#include <external/ExRootAnalysis/ExRootTreeReader.h>
#include <classes/DelphesClasses.h>

// for memory sharing and process control
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <err.h>

vector<string> branch_names = {
  "Event",
  "Weight",
  "Particle",
  "GenJet",
  "KTjet",
  // "VLCjetR05N2",
  // "VLCjetR05N3",
  // "VLCjetR05N4",
  // "VLCjetR05N5",
  // "VLCjetR05N6",
  // "VLCjetR07N2",
  // "VLCjetR07N3",
  // "VLCjetR07N4",
  // "VLCjetR07N5",
  // "VLCjetR07N6",
  // "VLCjetR10N2",
  // "VLCjetR10N3",
  // "VLCjetR10N4",
  // "VLCjetR10N5",
  // "VLCjetR10N6",
  // "VLCjetR12N2",
  // "VLCjetR12N3",
  // "VLCjetR12N4",
  // "VLCjetR12N5",
  // "VLCjetR12N6",
  // "VLCjetR15N2",
  // "VLCjetR15N3",
  // "VLCjetR15N4",
  // "VLCjetR15N5",
  // "VLCjetR15N6",
  // "VLCjetR02_inclusive",
  "VLCjetR05_inclusive",
  // "VLCjetR07_inclusive",
  // "VLCjetR10_inclusive",
  // "VLCjetR12_inclusive",
  // "VLCjetR15_inclusive",
  "GenMissingET",
  "Track",
  "Tower",
  "EFlowTrack",
  "EFlowPhoton",
  "EFlowNeutralHadron",
  "Photon",
  "Electron",
  "Muon",
  "ForwardMuon",
  "MissingET",
  "ScalarHT",
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
  TClonesArray *BrElectron = reader->UseBranch("Electron");
  TClonesArray *BrMuon = reader->UseBranch("Muon");
  TClonesArray *VLCjetR05 = reader->UseBranch("VLCjetR05_inclusive");
  Long64_t entries = min(entries_max, reader->GetEntries());

  TFile *outfile = new TFile((label + "_collect_" + (minmode ? "min_" : "")
        + mass + "_" + cme + ".root").c_str(), "recreate");
  TTree *Collect = new TTree("Collect", "Collected features for analysis");
  TClonesArray *BrJet = new TClonesArray("Jet", 10);
  Collect->Branch("Jet", &BrJet);
  for(const string &branch_name : branch_names)
  {
    TClonesArray *array;
    if(branch_name == "Electron")
      array = BrElectron;
    else if(branch_name == "Muon")
      array = BrMuon;
    else if(branch_name == "VLCjetR05_inclusive")
      array = VLCjetR05;
    else
      array = reader->UseBranch(branch_name.c_str());
    Collect->Branch(branch_name.c_str(), &array);
  }

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
      clog << "Processing " << directory << ": "
        << setw(to_string(entries).length()) << (entry + 1)
        << " / " << entries << " entries processed\n";
  }

  // Collect->Print();
  Collect->Write();

  delete BrJet;
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
