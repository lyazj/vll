#include <err.h>
#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>
#include <stdexcept>
#include <iostream>

#define PARTICLE_NUMBER_MAX 9999

using namespace std;

void CountAllMain();

int main()
{
  CountAllMain();
  return 0;
}

class Counter {
private:
  size_t *counter;
  size_t counter_sum;
  size_t counter_max;
  size_t value_now;
  size_t value_max;
  size_t value_end;

public:
  Counter(size_t vend);
  Counter(const Counter &) = delete;
  ~Counter();
  void increase(size_t n = 1);
  void update();
  size_t value() const { return value_now; }
  size_t max() const { return value_max; }
  size_t end() const { return value_end; }
  void summary() const;
};

Counter::Counter(size_t vend)
{
  if(vend == SIZE_MAX)
    throw overflow_error("too large counter value end");
  counter = new size_t[vend + 1] { };
  counter_sum = 0;
  counter_max = 0;
  value_now = 0;
  value_max = 0;
  value_end = vend;
}

Counter::~Counter()
{
  delete[] counter;
}

void Counter::increase(size_t n)
{
  if(value_now + n < n)
    throw overflow_error("counter value overflow");
  value_now += n;
}

void Counter::update()
{
  if(value_now > value_end)
    throw overflow_error("counter value exceeded");
  if(counter_sum == SIZE_MAX)
    throw overflow_error("counter exceeded");
  ++counter[value_now];
  ++counter_sum;
  counter_max = std::max(counter_max, counter[value_now]);
  value_max = std::max(value_max, value_now);
  value_now = 0;
}

void Counter::summary() const
{
  size_t nbit_value = to_string(value_max).length();
  size_t nbit_counter = to_string(counter_sum).length();
  cout << fixed << setprecision(2);
  for(size_t v = 0; v <= value_max; ++v)
    cout << setw(nbit_value) << v
         << '\t' << setw(nbit_counter) << counter[v]
         << '\t' << setw(6) << counter[v] / ((double)counter_sum / 1e2) << '%'
         << endl;
  cout << setw(nbit_value) << ""
       << '\t' << setw(nbit_counter) << counter_sum
       << '\t' << setw(6) << 100.00 << '%'
       << endl;
  cout << setprecision(6);
  cout.unsetf(cout.floatfield);
}

void CountAllMain()
{
  gSystem->Load("libDelphes.so");

  struct {
    const char *path;
  } records[] = {
    {"mumuEE_delphes_events.root"},
    {"mumutt_delphes_events.root"},
  };

  for(const auto &record : records)
  {
    TFile file(record.path);
    TTree &Delphes(*file.Get<TTree>("Delphes"));
    // Delphes.Print("toponly");
    Delphes.SetMakeClass(kTRUE);

    // EntryNumber: the number of entries
    Long64_t EntryNumber = Delphes.GetEntries();
    cout << "Got record path: " << record.path << endl;
    cout << "Got record entry number: " << EntryNumber << endl;
    cout << endl;

    // ParticleNumber: the number of particles per record
    Int_t ParticleNumber;
    TBranch &Particle_size = *Delphes.GetBranch("Particle_size");
    // Particle_size.Print();
    Particle_size.SetAddress(&ParticleNumber);
    // Delphes.GetBranch("Particle")->Print();
    Delphes.SetBranchAddress("Particle", &ParticleNumber);

    // // ParticleStatus: Status of all particles per record
    // Int_t ParticleStatus[PARTICLE_NUMBER_MAX];
    // TBranch &Particle_Status = *Delphes.GetBranch("Particle.Status");
    // // Particle_Status.Print();
    // Particle_Status.SetAddress(ParticleStatus);

    // ParticlePID: PID of all particles per record
    Int_t ParticlePID[PARTICLE_NUMBER_MAX];
    TBranch &Particle_PID = *Delphes.GetBranch("Particle.PID");
    // Particle_PID.Print();
    Particle_PID.SetAddress(ParticlePID);

    // // ParticlePT: PT of all particles per record
    // Float_t ParticlePT[PARTICLE_NUMBER_MAX];
    // TBranch &Particle_PT = *Delphes.GetBranch("Particle.PT");
    // // Particle_PT.Print();
    // Particle_PT.SetAddress(ParticlePT);

    // // ParticleEta: Eta of all particles per record
    // Float_t ParticleEta[PARTICLE_NUMBER_MAX];
    // TBranch &Particle_Eta = *Delphes.GetBranch("Particle.Eta");
    // // Particle_Eta.Print();
    // Particle_Eta.SetAddress(ParticleEta);

    // // ParticlePhi: Phi of all particles per record
    // Float_t ParticlePhi[PARTICLE_NUMBER_MAX];
    // TBranch &Particle_Phi = *Delphes.GetBranch("Particle.Phi");
    // // Particle_Phi.Print();
    // Particle_Phi.SetAddress(ParticlePhi);

    // JetNumber: the number of jets per record
    Int_t JetNumber;
    TBranch &Jet_size = *Delphes.GetBranch("Jet_size");
    // Jet_size.Print();
    Jet_size.SetAddress(&JetNumber);
    // Delphes.GetBranch("Jet")->Print();
    Delphes.SetBranchAddress("Jet", &JetNumber);

    // JetFlavor: the number of jets per record
    UInt_t JetFlavor[PARTICLE_NUMBER_MAX];
    TBranch &Jet_Flavor = *Delphes.GetBranch("Jet.Flavor");
    // Jet_Flavor.Print();
    Jet_Flavor.SetAddress(JetFlavor);

    Counter BjetCounter(PARTICLE_NUMBER_MAX);
    Counter QjetCounter(PARTICLE_NUMBER_MAX);
    Counter XjetCounter(PARTICLE_NUMBER_MAX);
    Counter TauCounter(PARTICLE_NUMBER_MAX);
    for(Long64_t i = 0; i < EntryNumber; ++i)
    {
      // get the number of particles
      Particle_size.GetEntry(i);
      if(ParticleNumber > PARTICLE_NUMBER_MAX)
        errx(EXIT_FAILURE, "particle number %d too large", ParticleNumber);

      // read the particles
      Particle_PID.GetEntry(i);
      for(Int_t j = 0; j < ParticleNumber; ++j)
        if(abs(ParticlePID[j]) == 15)
          TauCounter.increase();

      // get the number of jets
      Jet_size.GetEntry(i);
      if(JetNumber > PARTICLE_NUMBER_MAX)
        errx(EXIT_FAILURE, "jet number %d too large", JetNumber);

      // read the jets
      Jet_Flavor.GetEntry(i);
      for(Int_t j = 0; j < JetNumber; ++j)
      {
        if(JetFlavor[j] == 5)
          BjetCounter.increase();
        else if(1 <= JetFlavor[j] && JetFlavor[j] <= 4)
          QjetCounter.increase();
        else
          XjetCounter.increase();
      }

      BjetCounter.update();
      QjetCounter.update();
      XjetCounter.update();
      TauCounter.update();
    }

    cout << "b jet number:" << endl;
    BjetCounter.summary();
    cout << endl;
    cout << "q jet number:" << endl;
    QjetCounter.summary();
    cout << endl;
    cout << "x jet number:" << endl;
    XjetCounter.summary();
    cout << endl;
    cout << "tau number:" << endl;
    TauCounter.summary();
    cout << endl;
  }
}
