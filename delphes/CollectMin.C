#include "Collect.C"

void CollectMin(const vector<string> &directories = ::directories)
{
  branch_names.clear();
  Collect(directories, 1);
}
