#include "Widgets/TMenuOptionsWidget.h"
#include "TGameUserSettings.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "Kismet/KismetInternationalizationLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"

void UTMenuOptionsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// SCREEN
	ResolutionDropdown->OnSelectionChanged.AddDynamic(this, &UTMenuOptionsWidget::OnResolutionChanged);
	FPSDropdown->OnSelectionChanged.AddDynamic(this, &UTMenuOptionsWidget::OnFPSChanged);
	WindowModeDropdown->OnSelectionChanged.AddDynamic(this, &UTMenuOptionsWidget::OnWindowModeChanged);
	VSyncDropdown->OnSelectionChanged.AddDynamic(this, &UTMenuOptionsWidget::OnVSyncChanged);
	FOVInputText->OnTextCommitted.AddDynamic(this, &UTMenuOptionsWidget::OnFOVCommited);
	FOVSlider->OnValueChanged.AddDynamic(this, &UTMenuOptionsWidget::OnFOVSliderChanged);

	// GRAPHICS
	QualityDropdown->OnSelectionChanged.AddDynamic(this, &UTMenuOptionsWidget::OnQualityChanged);
	ViewDistanceDropdown->OnSelectionChanged.AddDynamic(this, &UTMenuOptionsWidget::OnViewDistanceChanged);
	AntiAliasingDropdown->OnSelectionChanged.AddDynamic(this, &UTMenuOptionsWidget::OnAntiAliasingChanged);
	PostProcessingDropdown->OnSelectionChanged.AddDynamic(this, &UTMenuOptionsWidget::OnPostProcessingChanged);
	ShadowsDropdown->OnSelectionChanged.AddDynamic(this, &UTMenuOptionsWidget::OnShadowsChanged);
	GlobalIlluminationDropdown->OnSelectionChanged.AddDynamic(this, &UTMenuOptionsWidget::OnGlobalIlluminationChanged);
	ReflectionsDropdown->OnSelectionChanged.AddDynamic(this, &UTMenuOptionsWidget::OnReflectionsChanged);
	TextureDropdown->OnSelectionChanged.AddDynamic(this, &UTMenuOptionsWidget::OnTextureChanged);
	EffectsDropdown->OnSelectionChanged.AddDynamic(this, &UTMenuOptionsWidget::OnEffectsChanged);

	// AUDIO
	BackgroundSoundCheckBox->OnCheckStateChanged.AddDynamic(this, &UTMenuOptionsWidget::OnBackgroundSoundChanged);

	// INTERFACE
	LanguageDropdown->OnSelectionChanged.AddDynamic(this, &UTMenuOptionsWidget::OnLanguageChanged);
	ColorDeficiencyDropdown->OnSelectionChanged.AddDynamic(this, &UTMenuOptionsWidget::OnColorDeficiencyChanged);
	DyslexicDropdown->OnSelectionChanged.AddDynamic(this, &UTMenuOptionsWidget::OnDyslexicChanged);
	UIScaleInputText->OnTextCommitted.AddDynamic(this, &UTMenuOptionsWidget::OnUIScaleCommited);
	UIScaleSlider->OnValueChanged.AddDynamic(this, &UTMenuOptionsWidget::OnUIScaleSliderChanged);

	if (GEngine)
		GameUserSettings = Cast<UTGameUserSettings>(GEngine->GetGameUserSettings());

	GameUserSettings->LoadSettings();
	ApplyInitialScreenSettings();
	ApplyInitialGraphicsSettings();
	ApplyInitialAudioSettings();
	ApplyInitialInterfaceSettings();
}

void UTMenuOptionsWidget::ApplyInitialScreenSettings()
{
	FIntPoint ScreenResolution = GameUserSettings->GetScreenResolution();
	FString Resolution = FString::Printf(TEXT("%d × %d"), ScreenResolution.X, ScreenResolution.Y);
	ResolutionDropdown->SetSelectedOption(Resolution);

	FPSDropdown->SetSelectedOption(FString::FromInt(GameUserSettings->GetFrameRateLimit()));

	WindowModeDropdown->SetSelectedIndex(static_cast<int>(GameUserSettings->GetFullscreenMode()));

	VSyncDropdown->SetSelectedIndex(GameUserSettings->IsVSyncEnabled() ? 0 : 1);

	FOVInputText->SetText(FText::FromString(FString::FromInt(GameUserSettings->GetFOV())));
	FOVSlider->SetValue(GameUserSettings->GetFOV());
}

void UTMenuOptionsWidget::ApplyInitialGraphicsSettings()
{
	if (GameUserSettings->GetOverallScalabilityLevel() != -1)
		QualityDropdown->SetSelectedIndex(GameUserSettings->GetOverallScalabilityLevel());
	else
		QualityDropdown->SetSelectedIndex(4);

	ViewDistanceDropdown->SetSelectedIndex(GameUserSettings->GetViewDistanceQuality());
	AntiAliasingDropdown->SetSelectedIndex(GameUserSettings->GetAntiAliasingQuality());
	PostProcessingDropdown->SetSelectedIndex(GameUserSettings->GetPostProcessingQuality());
	ShadowsDropdown->SetSelectedIndex(GameUserSettings->GetShadowQuality());
	GlobalIlluminationDropdown->SetSelectedIndex(GameUserSettings->GetGlobalIlluminationQuality());
	ReflectionsDropdown->SetSelectedIndex(GameUserSettings->GetReflectionQuality());
	TextureDropdown->SetSelectedIndex(GameUserSettings->GetTextureQuality());
	EffectsDropdown->SetSelectedIndex(GameUserSettings->GetVisualEffectQuality());
}

void UTMenuOptionsWidget::ApplyInitialAudioSettings()
{
	BackgroundSoundCheckBox->SetIsChecked(GameUserSettings->IsBackgroundSoundPaused());
}

void UTMenuOptionsWidget::ApplyInitialInterfaceSettings()
{
	FString CurrentLanguage = UKismetInternationalizationLibrary::GetCurrentLanguage();
	if (CurrentLanguage == "en")
		LanguageDropdown->SetSelectedIndex(0);
	else if (CurrentLanguage == "es")
		LanguageDropdown->SetSelectedIndex(1);
	else if (CurrentLanguage == "ca")
		LanguageDropdown->SetSelectedIndex(2);

	ColorDeficiencyDropdown->SetSelectedIndex(static_cast<int>(GameUserSettings->GetColorVisionDeficiency()));
	DyslexicDropdown->SetSelectedIndex(GameUserSettings->IsDyslexicModeOn() ? 0 : 1);
	UIScaleInputText->SetText(FText::FromString(FString::SanitizeFloat(GameUserSettings->GetUIScale())));
	UIScaleSlider->SetValue(GameUserSettings->GetUIScale());
}

void UTMenuOptionsWidget::UpdateAllComboBoxTranslations()
{
	UpdateComboBoxTranslations(WindowModeDropdown, WindowModeOptions);
	UpdateComboBoxTranslations(VSyncDropdown, VSyncOptions);
	UpdateComboBoxTranslations(LanguageDropdown, LanguageOptions);
	UpdateComboBoxTranslations(ColorDeficiencyDropdown, ColorDeficiencyOptions);
	UpdateComboBoxTranslations(DyslexicDropdown, DyslexicOptions);
}

void UTMenuOptionsWidget::UpdateComboBoxTranslations(UComboBoxString* ComboBox, TArray<FText> TranslatedOptions)
{
	ComboBox->ClearOptions();
	for (const FText& Option : TranslatedOptions)
		ComboBox->AddOption(Option.ToString());
}

void UTMenuOptionsWidget::OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	TArray<FString> Result = UKismetStringLibrary::ParseIntoArray(SelectedItem, " × ");

	uint8 X = FCString::Atoi(*Result[0]);
	uint8 Y = FCString::Atoi(*Result[1]);

	GameUserSettings->SetScreenResolution(FIntPoint(X, Y));
	GameUserSettings->ApplySettings(false);
}

void UTMenuOptionsWidget::OnFPSChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	GameUserSettings->SetFrameRateLimit(FCString::Atoi(*SelectedItem));
	GameUserSettings->ApplySettings(false);
}

void UTMenuOptionsWidget::OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	GameUserSettings->SetFullscreenMode(static_cast<EWindowMode::Type>(FCString::Atoi(*SelectedItem)));
	GameUserSettings->ApplySettings(false);
}

void UTMenuOptionsWidget::OnVSyncChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	bool bEnabled = VSyncDropdown->GetOptionAtIndex(0) == SelectedItem;

	GameUserSettings->SetVSyncEnabled(bEnabled);
	GameUserSettings->ApplySettings(false);
}

void UTMenuOptionsWidget::OnFOVCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	int FOV = FCString::Atoi(*Text.ToString());
	UKismetMathLibrary::Clamp(FOV, 90, 110);
	
	FOVInputText->SetText(FText::FromString(FString::FromInt(FOV)));
	FOVSlider->SetValue(FOV);
	GameUserSettings->SetFOV(FOV);
}

void UTMenuOptionsWidget::OnFOVSliderChanged(float Value)
{
	int FOV = FMath::RoundToInt(Value);

	FOVInputText->SetText(FText::FromString(FString::FromInt(FOV)));
	GameUserSettings->SetFOV(FOV);
}

void UTMenuOptionsWidget::OnQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	uint8 SelectedIndex = QualityDropdown->GetSelectedIndex();
	if (SelectedIndex != 4) {
		GameUserSettings->SetOverallScalabilityLevel(SelectedIndex);
		GameUserSettings->ApplySettings(false);

		ViewDistanceDropdown->SetSelectedIndex(SelectedIndex);
		AntiAliasingDropdown->SetSelectedIndex(SelectedIndex);
		PostProcessingDropdown->SetSelectedIndex(SelectedIndex);
		ShadowsDropdown->SetSelectedIndex(SelectedIndex);
		GlobalIlluminationDropdown->SetSelectedIndex(SelectedIndex);
		ReflectionsDropdown->SetSelectedIndex(SelectedIndex);
		TextureDropdown->SetSelectedIndex(SelectedIndex);
		EffectsDropdown->SetSelectedIndex(SelectedIndex);
	}
}

void UTMenuOptionsWidget::OnViewDistanceChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	GameUserSettings->SetViewDistanceQuality(ViewDistanceDropdown->GetSelectedIndex());
	GameUserSettings->ApplySettings(false);
}

void UTMenuOptionsWidget::OnAntiAliasingChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	GameUserSettings->SetAntiAliasingQuality(AntiAliasingDropdown->GetSelectedIndex());
	GameUserSettings->ApplySettings(false);
}

void UTMenuOptionsWidget::OnPostProcessingChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	GameUserSettings->SetPostProcessingQuality(PostProcessingDropdown->GetSelectedIndex());
	GameUserSettings->ApplySettings(false);
}

void UTMenuOptionsWidget::OnShadowsChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	GameUserSettings->SetShadowQuality(ShadowsDropdown->GetSelectedIndex());
	GameUserSettings->SetShadingQuality(ShadowsDropdown->GetSelectedIndex());
	GameUserSettings->ApplySettings(false);
}

void UTMenuOptionsWidget::OnGlobalIlluminationChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	GameUserSettings->SetGlobalIlluminationQuality(GlobalIlluminationDropdown->GetSelectedIndex());
	GameUserSettings->ApplySettings(false);
}

void UTMenuOptionsWidget::OnReflectionsChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	GameUserSettings->SetReflectionQuality(ReflectionsDropdown->GetSelectedIndex());
	GameUserSettings->ApplySettings(false);
}

void UTMenuOptionsWidget::OnTextureChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	GameUserSettings->SetTextureQuality(TextureDropdown->GetSelectedIndex());
	GameUserSettings->ApplySettings(false);
}

void UTMenuOptionsWidget::OnEffectsChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	GameUserSettings->SetVisualEffectQuality(EffectsDropdown->GetSelectedIndex());
	GameUserSettings->ApplySettings(false);
}

void UTMenuOptionsWidget::OnBackgroundSoundChanged(bool bIsChecked)
{
	GameUserSettings->SetBackgroundSoundPaused(bIsChecked);
}

void UTMenuOptionsWidget::OnLanguageChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	switch (LanguageDropdown->GetSelectedIndex()) {
	case 0:
		UKismetInternationalizationLibrary::SetCurrentLanguageAndLocale("en", true);
		break;
	case 1:
		UKismetInternationalizationLibrary::SetCurrentLanguageAndLocale("es", true);
		break;
	case 2:
		UKismetInternationalizationLibrary::SetCurrentLanguageAndLocale("ca", true);
		break;
	default:
		break;
	}
}

void UTMenuOptionsWidget::OnColorDeficiencyChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	uint8 SelectedIndex = ColorDeficiencyDropdown->GetSelectedIndex();
	GameUserSettings->SetColorVisionDeficiency(static_cast<EColorVisionDeficiency>(SelectedIndex));
}

void UTMenuOptionsWidget::OnDyslexicChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	bool bEnabled = static_cast<bool>(DyslexicDropdown->GetSelectedIndex());

	GameUserSettings->SetDyslexicMode(!bEnabled);
	GameUserSettings->ApplySettings(false);
}

void UTMenuOptionsWidget::OnUIScaleCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	FString TextString = Text.ToString();
	UKismetStringLibrary::ReplaceInline(TextString, ",", ".");

	float UIScale = FCString::Atof(*TextString);
	UKismetMathLibrary::Clamp(UIScale, 0.5f, 2.0f);

	UIScaleInputText->SetText(FText::FromString(FString::SanitizeFloat(UIScale)));
	UIScaleSlider->SetValue(UIScale);
	GameUserSettings->SetUIScale(UIScale);
}

void UTMenuOptionsWidget::OnUIScaleSliderChanged(float Value)
{
	float UIScale = Value;

	UIScaleInputText->SetText(FText::FromString(FString::SanitizeFloat(UIScale)));
	GameUserSettings->SetUIScale(UIScale);
}