#include "Core/TGameInstance.h"
#include "MoviePlayer.h"
#include "Blueprint/UserWidget.h"

void UTGameInstance::Init()
{
    Super::Init();

    FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UTGameInstance::BeginLoadingScreen);
}

void UTGameInstance::BeginLoadingScreen(const FString& InMapName)
{
	UUserWidget* LoadingScreenWidgetInstance = CreateWidget<UUserWidget>(this, LoadingScreenWidget);

    if (!IsRunningDedicatedServer() && LoadingScreenWidgetInstance) {
        FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.MinimumLoadingScreenDisplayTime = -1.f;
        LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;
        LoadingScreen.bWaitForManualStop = false;
        LoadingScreen.bAllowEngineTick = false;
		LoadingScreen.WidgetLoadingScreen = LoadingScreenWidgetInstance->TakeWidget();

        GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
    }
}

void UTGameInstance::EndLoadingScreen()
{
    GetMoviePlayer()->StopMovie();
}