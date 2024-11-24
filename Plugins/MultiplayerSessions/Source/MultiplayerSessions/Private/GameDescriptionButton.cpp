#include "GameDescriptionButton.h"

UGameDescriptionButton::UGameDescriptionButton()
{
    OnClicked.AddDynamic(this, &UGameDescriptionButton::HandleButtonClicked);
}

void UGameDescriptionButton::HandleButtonClicked()
{
	if (SessionResult.IsValid())
		SetIsEnabled(false);
	else
		SetIsEnabled(true);
	
    OnGameDescriptionButtonClicked.Broadcast(SessionResult);
}