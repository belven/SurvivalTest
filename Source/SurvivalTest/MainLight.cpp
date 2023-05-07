#include "MainLight.h"

#include "BaseGameInstance.h"
#include "Components/ArrowComponent.h"
#include "Components/DirectionalLightComponent.h"

void AMainLight::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	totalTime += DeltaSeconds;

	int32 timeInt = FMath::RoundToInt32(totalTime);
	timeOfDay = timeInt % dayLength;

	if (timeOfDay < 1)
		lastTime = 0;

	float timePos = timeOfDay / dayLength;

	if (lightCurve) {
		lightLevel = lightCurve->GetFloatValue(timePos);
		GetLightComponent()->SetIntensity(maxLight* lightLevel);
	}

	for (float time : timeTriggers)
	{		
		if(FMath::IsWithin(lightLevel, time -0.015, time + 0.015) && time != lastTime)
		{
			lastTime = time;
			OnTimeTransition.Broadcast(this);
		}
	}

	GetRootComponent()->SetRelativeRotation(FMath::Lerp(startRotation, endRotation, lightLevel));
}

void AMainLight::BeginPlay()
{
	Super::BeginPlay();

	UBaseGameInstance* game = Cast<UBaseGameInstance>(GetGameInstance());

	if (game)
		game->SetMainLight(this);

	timeTriggers.Empty();

	float time = 0.0f;

	while(time < 1.0f)
	{
		timeTriggers.Add(time);
		time += (1.0f / 20.0f);
	}
}

AMainLight::AMainLight(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDirectionalLightComponent>(TEXT("LightComponent0")))
{
	static ConstructorHelpers::FObjectFinder<UCurveFloat> lightCurveAsset(TEXT("CurveFloat'/Game/FirstPerson/Blueprints/LightLevel.LightLevel'"));

	lightCurve = lightCurveAsset.Object;
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FName ID_Lighting;
		FText NAME_Lighting;
		FConstructorStatics()
			: ID_Lighting(TEXT("Lighting"))
			, NAME_Lighting(NSLOCTEXT("SpriteCategory", "Lighting", "Lighting"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

#if !WITH_EDITORONLY_DATA
	UDirectionalLightComponent* DirectionalLightComponent;
#endif
	DirectionalLightComponent = CastChecked<UDirectionalLightComponent>(GetLightComponent());
	DirectionalLightComponent->Mobility = EComponentMobility::Stationary;
	DirectionalLightComponent->SetRelativeRotation(FRotator(-46.0f, 0.0f, 0.0f));
	// Make directional light icons big since they tend to be important
	// This is the root component so its scale affects all other components
	DirectionalLightComponent->SetRelativeScale3D(FVector(2.5f, 2.5f, 2.5f));

	RootComponent = DirectionalLightComponent;

#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent0"));
	if (ArrowComponent)
	{
		ArrowComponent->ArrowColor = FColor(150, 200, 255);

		ArrowComponent->bTreatAsASprite = true;
		ArrowComponent->SpriteInfo.Category = ConstructorStatics.ID_Lighting;
		ArrowComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Lighting;
		ArrowComponent->SetupAttachment(DirectionalLightComponent);
		ArrowComponent->bLightAttachment = true;
		ArrowComponent->bIsScreenSizeScaled = true;
	}

	bIsSpatiallyLoaded = false;
#endif // WITH_EDITORONLY_DATA

	}

void AMainLight::PostLoad()
{
	Super::PostLoad();

	if (GetLightComponent()->Mobility == EComponentMobility::Static)
	{
		GetLightComponent()->LightFunctionMaterial = NULL;
	}
#if WITH_EDITORONLY_DATA
	if (ArrowComponent != nullptr)
	{
		ArrowComponent->ArrowColor = GetLightColor().ToFColor(true);
	}
#endif
}

#if WITH_EDITOR
void AMainLight::LoadedFromAnotherClass(const FName& OldClassName)
{
	Super::LoadedFromAnotherClass(OldClassName);

	if (GetLinkerUEVersion() < VER_UE4_REMOVE_LIGHT_MOBILITY_CLASSES)
	{
		static FName DirectionalLightStatic_NAME(TEXT("DirectionalLightStatic"));
		static FName DirectionalLightMovable_NAME(TEXT("DirectionalLightMovable"));
		static FName DirectionalLightStationary_NAME(TEXT("DirectionalLightStationary"));

		UDirectionalLightComponent* DirLightComp = CastChecked<UDirectionalLightComponent>(GetLightComponent());

		if (OldClassName == DirectionalLightStatic_NAME)
		{
			DirLightComp->Mobility = EComponentMobility::Static;
		}
		else if (OldClassName == DirectionalLightMovable_NAME)
		{
			DirLightComp->Mobility = EComponentMobility::Movable;
			DirLightComp->DynamicShadowDistanceMovableLight = DirLightComp->WholeSceneDynamicShadowRadius_DEPRECATED;
		}
		else if (OldClassName == DirectionalLightStationary_NAME)
		{
			DirLightComp->Mobility = EComponentMobility::Stationary;

			// copy radius to correct var, if it had been changed
			if (DirLightComp->WholeSceneDynamicShadowRadius_DEPRECATED != 20000.f)
			{
				DirLightComp->DynamicShadowDistanceStationaryLight = DirLightComp->WholeSceneDynamicShadowRadius_DEPRECATED;  // copy radius to correct var
			}
		}
	}
}

void AMainLight::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (ArrowComponent != nullptr)
	{
		ArrowComponent->ArrowColor = GetLightColor().ToFColor(true);
	}
}
#endif // WITH_EDITOR