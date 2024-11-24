#include "Menu.h"
#include "GameDescriptionButton.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameFramework/PlayerState.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	if (UWorld* World = GetWorld()) {
		if (APlayerController* PlayerController = World->GetFirstPlayerController()) {
			FInputModeUIOnly InputModeUI;
			InputModeUI.SetWidgetToFocus(TakeWidget());
			InputModeUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeUI);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &UMenu::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionComplete.AddUObject(this, &UMenu::OnFindSession);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &UMenu::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UMenu::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &UMenu::OnStartSession);
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize()) 
		return false;

	if (HostButton)
		HostButton->OnClicked.AddDynamic(this, &UMenu::HostButtonClicked);
	if (JoinButton)
		JoinButton->OnClicked.AddDynamic(this, &UMenu::JoinButtonClicked);

	return true;
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful) {
		if (UWorld* World = GetWorld())
			World->ServerTravel(PathToLobby);
	} else {
		HostButton->SetIsEnabled(true);
	}
}

void UMenu::MenuFindSessions()
{
	if (MultiplayerSessionsSubsystem)
		MultiplayerSessionsSubsystem->FindSessions(10000);
}

void UMenu::OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (!MultiplayerSessionsSubsystem)
		return;

	if (UUserWidget* ListWidget = Cast<UUserWidget>(GetWidgetFromName("WBPMenuListOfGames"))) {
		if (UVerticalBox* ListOfGamesBox = Cast<UVerticalBox>(ListWidget->GetWidgetFromName("ListOfGames"))) {
			ListOfGamesBox->ClearChildren();

			for (auto Result : SessionResults) {
				if (UUserWidget* GameDescription = CreateWidget<UUserWidget>(this, GameDescriptionClass)) {
					if (URichTextBlock* GameDescriptionText = Cast<URichTextBlock>(GameDescription->GetWidgetFromName("Description"))) {
						FString Username;
						Result.Session.SessionSettings.Get(TEXT("Username"), Username);

						GameDescriptionText->SetText(FText::FromString(Username));
					}

					if (UGameDescriptionButton* GameDescriptionButton = Cast<UGameDescriptionButton>(GameDescription->GetWidgetFromName("JoinButton"))) {
						GameDescriptionButton->SetSessionResult(Result);
						GameDescriptionButton->OnGameDescriptionButtonClicked.AddUObject(this, &UMenu::OnJoinButtonClicked);
					}

					ListOfGamesBox->AddChild(GameDescription);
				}
			}
		}
	}

	if (!bWasSuccessful || SessionResults.Num() == 0)
		JoinButton->SetIsEnabled(true);
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if (auto OnlineSubsystem = IOnlineSubsystem::Get()) {
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid()) {
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);

	if (MultiplayerSessionsSubsystem) {
		FString PlayerName = GetOwningPlayerState()->GetPlayerName();
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType, PlayerName);
	}
}

void UMenu::JoinButtonClicked_Implementation()
{
	JoinButton->SetIsEnabled(false);

	MenuFindSessions();
}

void UMenu::OnJoinButtonClicked(const FOnlineSessionSearchResult& Result)
{
	if (MultiplayerSessionsSubsystem && Result.IsValid()) {
		MultiplayerSessionsSubsystem->JoinSession(Result);

		MenuUnSetup();
	}
}

void UMenu::MenuUnSetup()
{
	RemoveFromParent();

	if (UWorld* World = GetWorld()) {
		if (APlayerController* PlayerController = World->GetFirstPlayerController()) {
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->SetShowMouseCursor(false);
		}
	}
}
