#include "renderBenchmark.h"

#include <perfMon/dag_cpuFreq.h>
#include <startup/dag_globalSettings.h>
#include <util/dag_globDef.h>
#include <debug/dag_debug.h>
#include <perfMon/dag_daProfiler.h>
#include <workCycle/dag_workCyclePerf.h>
#include <perfMon/dag_graphStat.h>

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
    const float framesCountF = static_cast<float>(framesCount);
    const size_t dataEntrySize = sizeof(data[0]);

    debug("RenderBenchmark: frames: %d, dataSize: %d bytes", framesCount, dataEntrySize * framesCount);

    float cpuTotalFlipCallbackSum = 0.0f;
    float cpuMainCycleSum = 0.0f;
    float cpuActSum = 0.0f;
    float cpuBeforeDrawSum = 0.0f;
    float cpuDrawSum = 0.0f;
    uint64_t trisTotal = 0;

    float drawPrimsAvg = 0.0f;
    float lockVIBufAvg = 0.0f;
    float updateRTAvg = 0.0f;
    float updatePSAvg = 0.0f;
    float drawInstancesAvg = 0.0f;
    float updateLogicalRPAvg = 0.0f;

    for (const auto& it : data)
    {
      cpuTotalFlipCallbackSum += it.frameTimeCPUTotalFlipCallback;
      cpuMainCycleSum += it.frameTimeCPUMainCycle;
      cpuActSum += it.frameTimeCPUAct;
      cpuBeforeDrawSum += it.frameTimeCPUBeforeDraw;
      cpuDrawSum += it.frameTimeCPUDraw;

      trisTotal += uint64_t(it.trianglesCount);

      drawPrimsAvg += float(it.drawPrimsCount);
      lockVIBufAvg += float(it.updateLockVIBufCount);
      updateRTAvg += float(it.updateRenderTargetCount);
      updatePSAvg += float(it.updateShaderProgramCount);
      drawInstancesAvg += float(it.drawInstancesCount);
      updateLogicalRPAvg += float(it.updateLogicalRenderPassesCount);
    }

    // check size != 0 (framesCount)
    cpuTotalFlipCallbackSum /= framesCount;
    cpuMainCycleSum /= framesCount;
    cpuActSum /= framesCount;
    cpuBeforeDrawSum /= framesCount;
    cpuDrawSum /= framesCount;

    double trisAvg = double(trisTotal) / double(framesCount);

    drawPrimsAvg /= framesCount;
    lockVIBufAvg /= framesCount;
    updateRTAvg /= framesCount;
    updatePSAvg /= framesCount;
    drawInstancesAvg /= framesCount;
    updateLogicalRPAvg /= framesCount;

    debug("RenderBenchmark: cpuTotalFlipCallbackSum avg: %f ms", cpuTotalFlipCallbackSum);
    debug("RenderBenchmark: cpuMainCycle avg: %f ms", cpuMainCycleSum);
    debug("RenderBenchmark: cpuAct avg: %f ms", cpuActSum);
    debug("RenderBenchmark: cpuBeforeDraw avg: %f ms", cpuBeforeDrawSum);
    debug("RenderBenchmark: cpuDraw avg: %f ms", cpuDrawSum);

    debug("RenderBenchmark: Triangles Count avg: %f", trisAvg);

    debug("RenderBenchmark: Draw Prims avg: %f", drawPrimsAvg);
    debug("RenderBenchmark: Lock VI Buffers avg: %f", lockVIBufAvg);
    debug("RenderBenchmark: Update Render Targets avg: %f", updateRTAvg);
    debug("RenderBenchmark: Update Shader Program avg: %f", updatePSAvg);
    debug("RenderBenchmark: Draw Instances avg: %f", drawInstancesAvg);
    debug("RenderBenchmark: Update Logical Render Passes avg: %f", updateLogicalRPAvg);

    data.clear();

    quit_game(1);
  }
}

void RenderBenchmark::update() {
    ::da_profiler::BenchmarkData &data = ::da_profiler::GetBenchmarkData();
    ::da_profiler::BenchmarkFrameData &frameData = data.back();

    runningTime = get_time_msec() - startTime;
    float frameCPUTotalMS = float(workcycleperf::frame_time_total_swap) * 1e-3;
    frameData.frameTimeCPUTotalFlipCallback = frameCPUTotalMS;

    frameData.trianglesCount = Stat3D::g_draw_stat.tri;

    frameData.drawPrimsCount = Stat3D::g_draw_stat.val[DRAWSTAT_DP];
    frameData.updateLockVIBufCount = Stat3D::g_draw_stat.val[DRAWSTAT_LOCKVIB];
    frameData.updateRenderTargetCount = Stat3D::g_draw_stat.val[DRAWSTAT_RT];
    frameData.updateShaderProgramCount = Stat3D::g_draw_stat.val[DRAWSTAT_PS];
    frameData.drawInstancesCount = Stat3D::g_draw_stat.val[DRAWSTAT_INS];
    frameData.updateLogicalRenderPassesCount = Stat3D::g_draw_stat.val[DRAWSTAT_RENDERPASS_LOGICAL];
    
    //debug("frame total: %f ms", frameCPUTotalMS);
    //debug("DP: %d", Stat3D::g_draw_stat.val[DRAWSTAT_DP]);
}