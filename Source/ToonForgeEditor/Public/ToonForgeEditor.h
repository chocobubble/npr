#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToonForgeEditorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** Get the ToonForgeEditor module instance */
	static FToonForgeEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FToonForgeEditorModule>("ToonForgeEditor");
	}
	
	/** Check if the ToonForgeEditor module is loaded */
	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ToonForgeEditor");
	}

private:
	/** Register editor UI */
	void RegisterMenus();
	void UnregisterMenus();
	
	/** Create NPR control panel */
	TSharedRef<class SDockTab> CreateNPRControlPanel(const class FSpawnTabArgs& Args);
	
	/** Tab identifiers */
	static const FName NPRControlPanelTabId;
};
