#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UObject/NoExportTypes.h"
#include "HelperFunctions.generated.h"

#define mSetTimerWorld(world, handle, method, delay) world->GetTimerManager().SetTimer(handle, this, method, delay)
#define mSetTimer(handle, method, delay) mSetTimerWorld(GetWorld(), handle, method, delay)
#define mTimeRemaining(handle) GetWorld()->GetTimerManager().GetTimerRemaining(handle)
#define mIsTimerActive(handle) GetWorld()->GetTimerManager().IsTimerActive(handle)
#define mClearTimerWorld(world, handle, method, delay) world->GetTimerManager().SetTimer(handle, this, method, delay)

#define mActorLocation GetCharacter()->GetActorLocation()
#define mActorRotation GetCharacter()->GetActorRotation()
#define mCurrentWeapon() GetBaseCharacter()->GetEquippedWeapon()

#define mGetRandom UHelperFunctions::GetRandom

UCLASS()
class SURVIVALTEST_API UHelperFunctions : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void ToggleUIVisibility(UUserWidget* widget)
	{
		if (widget)
		{
			if (widget->GetVisibility() == ESlateVisibility::Visible)
			{
				widget->SetVisibility(ESlateVisibility::Collapsed);
			}
			else
			{
				widget->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}

	template <class T>
	static T GetRandom(TArray<T> itemArray)
	{
		return itemArray[FMath::RandRange(0, itemArray.Num() - 1)];
	}
};
