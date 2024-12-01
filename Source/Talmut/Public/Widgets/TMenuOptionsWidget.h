#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TMenuOptionsWidget.generated.h"

class USlider;
class UEditableTextBox;
class UCheckBox;
class UComboBoxString;
class UTGameUserSettings;

UCLASS()
class TALMUT_API UTMenuOptionsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	// SCREEN
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ResolutionDropdown;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> FPSDropdown;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> WindowModeDropdown;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> VSyncDropdown;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> FOVInputText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> FOVSlider;

	// GRAPHICS
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> QualityDropdown;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ViewDistanceDropdown;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> AntiAliasingDropdown;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> PostProcessingDropdown;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ShadowsDropdown;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> GlobalIlluminationDropdown;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ReflectionsDropdown;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> TextureDropdown;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> EffectsDropdown;

	// AUDIO
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> BackgroundSoundCheckBox;

	// INTERFACE
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> LanguageDropdown;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ColorDeficiencyDropdown;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> DyslexicDropdown;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> UIScaleInputText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> UIScaleSlider;

	// TRANSLATIONS
	UPROPERTY(EditAnywhere, Category = "Translations")
	TArray<FText> WindowModeOptions;
	UPROPERTY(EditAnywhere, Category = "Translations")
	TArray<FText> VSyncOptions;
	UPROPERTY(EditAnywhere, Category = "Translations")
	TArray<FText> GraphicsOptions;
	UPROPERTY(EditAnywhere, Category = "Translations")
	TArray<FText> LanguageOptions;
	UPROPERTY(EditAnywhere, Category = "Translations")
	TArray<FText> ColorDeficiencyOptions;
	UPROPERTY(EditAnywhere, Category = "Translations")
	TArray<FText> DyslexicOptions;

private:
	// UPDATE SETTINGS
	void ApplyInitialScreenSettings();
	void ApplyInitialGraphicsSettings();
	void ApplyInitialAudioSettings();
	void ApplyInitialInterfaceSettings();

	UFUNCTION(BlueprintCallable, Category = "Translations")
	void UpdateAllComboBoxTranslations();
	void UpdateComboBoxTranslations(UComboBoxString* ComboBox, TArray<FText> TranslatedOptions);

	// SCREEN
	UFUNCTION()
	void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnFPSChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnVSyncChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnFOVCommited(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void OnFOVSliderChanged(float Value);

	// GRAPHICS
	UFUNCTION()
	void OnQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnViewDistanceChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnAntiAliasingChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnPostProcessingChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnShadowsChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnGlobalIlluminationChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnReflectionsChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnTextureChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnEffectsChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	// AUDIO
	UFUNCTION()
	void OnBackgroundSoundChanged(bool bIsChecked);

	// INTERFACE
	UFUNCTION()
	void OnLanguageChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnColorDeficiencyChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnDyslexicChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnUIScaleCommited(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void OnUIScaleSliderChanged(float Value);

private:
	TObjectPtr<UTGameUserSettings> GameUserSettings;
};
