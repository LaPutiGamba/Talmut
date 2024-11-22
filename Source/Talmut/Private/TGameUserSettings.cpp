#include "TGameUserSettings.h"
#include "TGameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UTGameUserSettings::UTGameUserSettings()
{
	FullscreenMode = EWindowMode::WindowedFullscreen;
	FrameRateLimit = 60;
	ColorVisionDeficiency = EColorVisionDeficiency::NormalVision;
	UIScale = 1.0f;
	FOV = 90;
	bIsBackgroundSoundPaused = true;
	bIsDyslexicModeOn = false;
}

UTGameUserSettings* UTGameUserSettings::GetTGameUserSettings()
{
	return Cast<UTGameUserSettings>(UGameUserSettings::GetGameUserSettings());
}

void UTGameUserSettings::SetColorVisionDeficiency(EColorVisionDeficiency NewColorVisionDeficiency)
{
	UWidgetBlueprintLibrary::SetColorVisionDeficiencyType(NewColorVisionDeficiency, 1.0f, false, false);
	ColorVisionDeficiency = NewColorVisionDeficiency;
	SaveSettings();
}

void UTGameUserSettings::SetUIScale(float NewUIScale)
{
	UTGameplayStatics::SetUIScale(NewUIScale);
	UIScale = NewUIScale;
	SaveSettings();
}

void UTGameUserSettings::SaveFOV(int NewFOV)
{
	FOV = NewFOV;
	SaveSettings();
}

void UTGameUserSettings::SetBackgroundSoundPaused(bool bIsPaused)
{
	bIsBackgroundSoundPaused = bIsPaused;
	SaveSettings();
}

void UTGameUserSettings::SetDyslexicMode(bool bIsOn)
{
	bIsDyslexicModeOn = bIsOn;
	SaveSettings();
}
