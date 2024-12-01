#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TGameInstance.generated.h"

UCLASS()
class TALMUT_API UTGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    virtual void Init() override;

    UFUNCTION()
    virtual void BeginLoadingScreen(const FString& InMapName);
    UFUNCTION()
    virtual void EndLoadingScreen();

private:
    UPROPERTY(EditAnywhere, Category = "Game Instance|Loading Screen")
	TSubclassOf<class UUserWidget> LoadingScreenWidget;
};
