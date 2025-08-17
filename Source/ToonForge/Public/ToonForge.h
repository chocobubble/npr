#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogToonForge, Log, All);

class FToonForgeModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** Get the ToonForge module instance */
	static FToonForgeModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FToonForgeModule>("ToonForge");
	}
	
	/** Check if the ToonForge module is loaded */
	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ToonForge");
	}

private:
	/** Core manager classes */
	TUniquePtr<class FToonForgeManager> Manager;
	TUniquePtr<class FNPRRenderingPipeline> RenderPipeline;
	TUniquePtr<class FNPRPerformanceMonitor> PerformanceMonitor;
};
