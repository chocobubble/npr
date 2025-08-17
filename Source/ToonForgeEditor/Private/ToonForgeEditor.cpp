#include "ToonForgeEditor.h"
#include "ToonForge.h"
#include "NPRControlPanelWidget.h"

#include "LevelEditor.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "FToonForgeEditorModule"

const FName FToonForgeEditorModule::NPRControlPanelTabId(TEXT("NPRControlPanel"));

void FToonForgeEditorModule::StartupModule()
{
	UE_LOG(LogToonForge, Log, TEXT("ToonForgeEditor module starting up"));
	
	// Register menus and UI
	RegisterMenus();
	
	UE_LOG(LogToonForge, Log, TEXT("ToonForgeEditor module started successfully"));
}

void FToonForgeEditorModule::ShutdownModule()
{
	UE_LOG(LogToonForge, Log, TEXT("ToonForgeEditor module shutting down"));
	
	// Unregister menus and UI
	UnregisterMenus();
	
	UE_LOG(LogToonForge, Log, TEXT("ToonForgeEditor module shut down successfully"));
}

void FToonForgeEditorModule::RegisterMenus()
{
	// Register tab spawner
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		NPRControlPanelTabId,
		FOnSpawnTab::CreateRaw(this, &FToonForgeEditorModule::CreateNPRControlPanel))
		.SetDisplayName(LOCTEXT("NPRControlPanelTitle", "NPR Control Panel"))
		.SetTooltipText(LOCTEXT("NPRControlPanelTooltip", "Open the NPR Control Panel"))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsCategory())
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));
	
	// Add menu entry
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender);
	
	MenuExtender->AddMenuExtension(
		"WindowLayout",
		EExtensionHook::After,
		nullptr,
		FMenuExtensionDelegate::CreateLambda([](FMenuBuilder& Builder)
		{
			Builder.AddSubMenu(
				LOCTEXT("ToonForgeMenu", "ToonForge"),
				LOCTEXT("ToonForgeMenuTooltip", "ToonForge NPR Tools"),
				FNewMenuDelegate::CreateLambda([](FMenuBuilder& SubMenuBuilder)
				{
					SubMenuBuilder.AddMenuEntry(
						LOCTEXT("OpenNPRControlPanel", "NPR Control Panel"),
						LOCTEXT("OpenNPRControlPanelTooltip", "Open the NPR Control Panel"),
						FSlateIcon(),
						FUIAction(FExecuteAction::CreateLambda([]()
						{
							FGlobalTabmanager::Get()->TryInvokeTab(FToonForgeEditorModule::NPRControlPanelTabId);
						}))
					);
				})
			);
		})
	);
	
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
}

void FToonForgeEditorModule::UnregisterMenus()
{
	// Unregister tab spawner
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(NPRControlPanelTabId);
}

TSharedRef<SDockTab> FToonForgeEditorModule::CreateNPRControlPanel(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SNPRControlPanelWidget)
		];
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FToonForgeEditorModule, ToonForgeEditor)
