// Copyright 2022 Ares9323. All Rights Reserved.

#include "RestartEditor.h"
#include "RestartEditorStyle.h"
#include "RestartEditorCommands.h"
#include "ToolMenus.h"

#include "Interfaces/IMainFrameModule.h"
#include "LevelEditor.h"

static const FName RestartEditorTabName("RestartEditor");

#define LOCTEXT_NAMESPACE "FRestartEditorModule"

void FRestartEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FRestartEditorStyle::Initialize();
	FRestartEditorStyle::ReloadTextures();

	FRestartEditorCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FRestartEditorCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FRestartEditorModule::PluginButtonClicked),
		FCanExecuteAction());

	IMainFrameModule& mainFrame = FModuleManager::Get().LoadModuleChecked<IMainFrameModule>("MainFrame");
	mainFrame.GetMainFrameCommandBindings()->Append(PluginCommands.ToSharedRef());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FRestartEditorModule::RegisterMenus));
}

void FRestartEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FRestartEditorStyle::Shutdown();

	FRestartEditorCommands::Unregister();
}

void FRestartEditorModule::PluginButtonClicked()
{
	FUnrealEdMisc::Get().RestartEditor(false);
}

void FRestartEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FRestartEditorCommands::Get().PluginAction, PluginCommands);
		}
	}
	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FRestartEditorCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRestartEditorModule, RestartEditor)