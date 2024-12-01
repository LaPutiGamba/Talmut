#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "TGameUserSettings.generated.h"

enum class EColorVisionDeficiency : uint8;

UCLASS()
class TALMUT_API UTGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UTGameUserSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	static UTGameUserSettings* GetTGameUserSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetColorVisionDeficiency(EColorVisionDeficiency NewColorVisionDeficiency);

	UFUNCTION(BlueprintPure)
	EColorVisionDeficiency GetColorVisionDeficiency() const { return ColorVisionDeficiency; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetUIScale(float NewUIScale);

	UFUNCTION(BlueprintPure)
	float GetUIScale() const { return UIScale; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetFOV(int NewFOV);

	UFUNCTION(BlueprintPure)
	int GetFOV() const { return FOV; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetBackgroundSoundPaused(bool bIsPaused);

	UFUNCTION(BlueprintPure)
	bool IsBackgroundSoundPaused() const { return bIsBackgroundSoundPaused; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetDyslexicMode(bool bIsOn);

	UFUNCTION(BlueprintPure)
	bool IsDyslexicModeOn() const { return bIsDyslexicModeOn; }

protected:
	UPROPERTY(Config)
	EColorVisionDeficiency ColorVisionDeficiency;

	UPROPERTY(Config)
	float UIScale;

	UPROPERTY(Config)
	int FOV;

	UPROPERTY(Config)
	bool bIsBackgroundSoundPaused;

	UPROPERTY(Config)
	bool bIsDyslexicModeOn;
};
