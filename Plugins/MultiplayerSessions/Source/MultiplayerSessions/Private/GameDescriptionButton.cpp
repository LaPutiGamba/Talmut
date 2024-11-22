#include "GameDescriptionButton.h"

UGameDescriptionButton::UGameDescriptionButton()
{
    OnClicked.AddDynamic(this, &UGameDescriptionButton::HandleButtonClicked);
}

void UGameDescriptionButton::HandleButtonClicked()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Button Clicked - On Clicked"));
    OnGameDescriptionButtonClicked.Broadcast(SessionResult);
}