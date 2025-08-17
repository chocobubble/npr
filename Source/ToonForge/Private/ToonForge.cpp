#include "ToonForge.h"
#include "ToonForgeManager.h"
#include "NPRRenderingPipeline.h"
#include "NPRPerformanceMonitor.h"

#define LOCTEXT_NAMESPACE "FToonForgeModule"

DEFINE_LOG_CATEGORY(LogToonForge);

void FToonForgeModule::StartupModule()
{
	UE_LOG(LogToonForge, Log, TEXT("ToonForge module starting up"));
	
	// Initialize core systems
	Manager = MakeUnique<FToonForgeManager>();
	RenderPipeline = MakeUnique<FNPRRenderingPipeline>();
	PerformanceMonitor = MakeUnique<FNPRPerformanceMonitor>();
	
	// Initialize manager
	if (Manager.IsValid())
	{
		Manager->Initialize();
	}
	
	UE_LOG(LogToonForge, Log, TEXT("ToonForge module started successfully"));
}

void FToonForgeModule::ShutdownModule()
{
	UE_LOG(LogToonForge, Log, TEXT("ToonForge module shutting down"));
	
	// Shutdown in reverse order
	if (Manager.IsValid())
	{
		Manager->Shutdown();
	}
	
	PerformanceMonitor.Reset();
	RenderPipeline.Reset();
	Manager.Reset();
	
	UE_LOG(LogToonForge, Log, TEXT("ToonForge module shut down successfully"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FToonForgeModule, ToonForge)
