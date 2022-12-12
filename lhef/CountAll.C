#include <err.h>

#define PARTICLE_NUMBER_MAX 9999

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

void CountAll()
{
  gSystem->Load("libExRootAnalysis.so");

  struct {
    const char *path;
  } records[] = {
    {"mumuEE_unweighted_events.root"},
    {"mumutt_unweighted_events.root"},
  };

  for(const auto &record : records)
  {
    TFile file(record.path);
    TTree &LHEF(*file.Get<TTree>("LHEF"));
    // LHEF.Print("toponly");
    LHEF.SetMakeClass(kTRUE);

    // EntryNumber: the number of entries
    Long64_t EntryNumber = LHEF.GetEntries();
    cout << "Got record path: " << record.path << endl;
    cout << "Got record entry number: " << EntryNumber << endl;
    cout << endl;

    // ParticleNumber: the number of particles per record
    Int_t ParticleNumber;
    TBranch &Particle_size = *LHEF.GetBranch("Particle_size");
    // Particle_size.Print();
    Particle_size.SetAddress(&ParticleNumber);
    // LHEF.GetBranch("Particle")->Print();
    LHEF.SetBranchAddress("Particle", &ParticleNumber);

    // ParticleStatus: Status of all particles per record
    Int_t ParticleStatus[PARTICLE_NUMBER_MAX];
    TBranch &Particle_Status = *LHEF.GetBranch("Particle.Status");
    // Particle_Status.Print();
    Particle_Status.SetAddress(ParticleStatus);

    // ParticlePID: PID of all particles per record
    Int_t ParticlePID[PARTICLE_NUMBER_MAX];
    TBranch &Particle_PID = *LHEF.GetBranch("Particle.PID");
    // Particle_PID.Print();
    Particle_PID.SetAddress(ParticlePID);

    // // ParticlePT: PT of all particles per record
    // Float_t ParticlePT[PARTICLE_NUMBER_MAX];
    // TBranch &Particle_PT = *LHEF.GetBranch("Particle.PT");
    // // Particle_PT.Print();
    // Particle_PT.SetAddress(ParticlePT);

    // // ParticleEta: Eta of all particles per record
    // Float_t ParticleEta[PARTICLE_NUMBER_MAX];
    // TBranch &Particle_Eta = *LHEF.GetBranch("Particle.Eta");
    // // Particle_Eta.Print();
    // Particle_Eta.SetAddress(ParticleEta);

    // // ParticlePhi: Phi of all particles per record
    // Float_t ParticlePhi[PARTICLE_NUMBER_MAX];
    // TBranch &Particle_Phi = *LHEF.GetBranch("Particle.Phi");
    // // Particle_Phi.Print();
    // Particle_Phi.SetAddress(ParticlePhi);

    Counter BCounter(PARTICLE_NUMBER_MAX);
    Counter QCounter(PARTICLE_NUMBER_MAX);
    Counter XCounter(PARTICLE_NUMBER_MAX);
    Counter TauCounter(PARTICLE_NUMBER_MAX);
    for(Long64_t i = 0; i < EntryNumber; ++i)
    {
      // get the number of particles
      Particle_size.GetEntry(i);
      if(ParticleNumber > PARTICLE_NUMBER_MAX)
        errx(EXIT_FAILURE, "particle number %d too large", ParticleNumber);

      // read the particles
      Particle_Status.GetEntry(i);
      Particle_PID.GetEntry(i);
      for(Int_t j = 0; j < ParticleNumber; ++j)
      {
        if(ParticleStatus[j] != 1)
          continue;
        if(abs(ParticlePID[j]) == 5)
          BCounter.increase();
        else if(1 <= abs(ParticlePID[j]) && abs(ParticlePID[j]) <= 4)
          QCounter.increase();
        else if(abs(ParticlePID[j]) == 15)
          TauCounter.increase();
        else
          XCounter.increase();
      }

      BCounter.update();
      QCounter.update();
      XCounter.update();
      TauCounter.update();
    }

    cout << "b number:" << endl;
    BCounter.summary();
    cout << endl;
    cout << "q number:" << endl;
    QCounter.summary();
    cout << endl;
    cout << "x number:" << endl;
    XCounter.summary();
    cout << endl;
    cout << "tau number:" << endl;
    TauCounter.summary();
    cout << endl;
  }
}
