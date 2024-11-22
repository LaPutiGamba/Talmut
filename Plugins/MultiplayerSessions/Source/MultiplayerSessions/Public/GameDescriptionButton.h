#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "OnlineSessionSettings.h"
#include "GameDescriptionButton.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameDescriptionButtonClicked, const FOnlineSessionSearchResult& Result);

UCLASS()
class MULTIPLAYERSESSIONS_API UGameDescriptionButton : public UButton
{
    GENERATED_BODY()

public:
    UGameDescriptionButton();

    UFUNCTION()
    void HandleButtonClicked();

	void SetSessionResult(const FOnlineSessionSearchResult& Result) { SessionResult = Result; }

public:
    FOnGameDescriptionButtonClicked OnGameDescriptionButtonClicked;

private:
    FOnlineSessionSearchResult SessionResult;
};
