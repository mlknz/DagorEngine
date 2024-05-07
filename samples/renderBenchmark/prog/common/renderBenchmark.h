#include <set>
#include <perfMon/dag_statDrv.h>

struct RenderBenchmarkConfig
{
  float workingTimeDebugMS = 50000;
  bool isFreeCamera = true;
  std::set<int> testsToRun = {0};
};

class RenderBenchmark
{
public:
  RenderBenchmark(const RenderBenchmarkConfig &aConfig);

  void startBenchmark();
  void quitIfBenchmarkHasEnded();

  void update();

private:
  RenderBenchmarkConfig config;

  float startTime = 0.0f;
  float runningTime = 0.0f;
};