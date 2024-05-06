#include "renderBenchmark.h"

#include <perfMon/dag_cpuFreq.h>
#include <startup/dag_globalSettings.h>
#include <util/dag_globDef.h>
#include <debug/dag_debug.h>
#include <perfMon/dag_daProfiler.h>
#include <workCycle/dag_workCyclePerf.h>

RenderBenchmark::RenderBenchmark(const RenderBenchmarkConfig &aConfig) : config(aConfig) {
}

void RenderBenchmark::startBenchmark() {
	startTime = get_time_msec();
    debug("RenderBenchmark: starting");
}

void RenderBenchmark::quitIfBenchmarkHasEnded()
{
  if (runningTime >= config.workingTimeDebugMS)
  {
    debug("RenderBenchmark: quitting, was running for %d ms", runningTime);

    ::da_profiler::BenchmarkData &data = ::da_profiler::GetBenchmarkData();
    const size_t framesCount = data.size();
    const size_t dataEntrySize = sizeof(data[0]);

    debug("RenderBenchmark: frames: %d, dataSize: %d bytes", framesCount, dataEntrySize * framesCount);

    float cpuTotalFlipCallbackSum = 0.0f;
    float cpuMainCycleSum = 0.0f;
    float cpuActSum = 0.0f;
    float cpuBeforeDrawSum = 0.0f;
    float cpuDrawSum = 0.0f;
    for (const auto& it : data)
    {
      cpuMainCycleSum += it.frameTimeCPUMainCycle;
      cpuActSum += it.frameTimeCPUAct;
      cpuBeforeDrawSum += it.frameTimeCPUBeforeDraw;
      cpuDrawSum += it.frameTimeCPUDraw;
    }

    cpuMainCycleSum /= framesCount; // check size != 0
    cpuActSum /= framesCount;       // check size != 0
    cpuBeforeDrawSum /= framesCount; // check size != 0
    cpuDrawSum /= framesCount;       // check size != 0

    debug("RenderBenchmark: cpuMainCycle avg: %f ms", cpuMainCycleSum);
    debug("RenderBenchmark: cpuAct avg: %f ms", cpuActSum);
    debug("RenderBenchmark: cpuBeforeDraw avg: %f ms", cpuBeforeDrawSum);
    debug("RenderBenchmark: cpuDraw avg: %f ms", cpuDrawSum);

    quit_game(1);
  }
}

void RenderBenchmark::update() {
    runningTime = get_time_msec() - startTime;
    float frameCPUTotalMS = float(workcycleperf::frame_time_total_swap) * 1e-3;
    debug("frame total: %f ms", frameCPUTotalMS);
}