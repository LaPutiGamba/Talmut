#include "TGameplayStatics.h"
#include "Engine/UserInterfaceSettings.h"

void UTGameplayStatics::SetUIScale(float CustomUIScale)
{
	if (UUserInterfaceSettings* UISettings = GetMutableDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())) {
		UISettings->ApplicationScale = CustomUIScale;
	}
}