#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/Maps/Main")), FString MenuPath = FString(TEXT("/Game/Maps/Menu")));

protected:
	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void MenuFindSessions();
	
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

	UFUNCTION(BlueprintNativeEvent)
	void JoinButtonClicked();

private:
	UFUNCTION()
	void HostButtonClicked();

	void MenuUnSetup();

	void OnJoinButtonClicked(const FOnlineSessionSearchResult& Result);

private:
	TObjectPtr<class UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;

	UPROPERTY(EditAnywhere, Category = "Menu")
	TSubclassOf<class UUserWidget> GameDescriptionClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> HostButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> JoinButton;

	int32 NumPublicConnections{ 4 };
	FString MatchType{ TEXT("Talmut") };
	FString PathToLobby{ TEXT("") };
	FString PathToMenu{ TEXT("") };
};
