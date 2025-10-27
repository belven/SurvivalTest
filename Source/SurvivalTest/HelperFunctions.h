#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HelperFunctions.generated.h"

#define mSetTimerWorld(world, handle, method, delay) world->GetTimerManager().SetTimer(handle, this, method, delay)
#define mSetTimer(handle, method, delay) mSetTimerWorld(GetWorld(), handle, method, delay)
#define mTimeRemaining(handle) GetWorld()->GetTimerManager().GetTimerRemaining(handle)
#define mIsTimerActive(handle) GetWorld()->GetTimerManager().IsTimerActive(handle)

#define mActorLocation GetCharacter()->GetActorLocation()
#define mActorRotation GetCharacter()->GetActorRotation()
#define mCurrentWeapon() GetBaseCharacter()->GetEquippedWeapon()

#define mGetRandom UHelperFunctions::GetRandom
#define mGetRandomEnum UHelperFunctions::GetRandomEnum

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

	//template <class A, class B>
	//static B GetLastMapItem(TMap<A, B> itemArray)
	//{
	//	TArray<B> values;
	//	itemArray.GenerateValueArray(values);
	//	return values.Last();
	//}

	template <class T>
	static T GetRandomEnum(T end)
	{
		return static_cast<T>(FMath::RandRange(0, static_cast<uint8>(end) - 1));
	}

	//template <class T>
	//USTRUCT(BlueprintType)
	//struct  FResult
	//{
	//	GENERATED_USTRUCT_BODY()
	//private:
	//	T value;

	//public:
	//	operator T() const { return value; }

	//	void SetValue(T newValue)
	//	{
	//		value = newValue;
	//		set = true;
	//	}

	//	bool set = false;
	//};

	template <class T>
	static T GetRandom(TArray<T> itemArray)
	{
		//FResult<T> result;

	//	if (!itemArray.IsEmpty()) {
		return  itemArray[FMath::RandRange(0, itemArray.Num() - 1)];
		//	}

		//	UE_LOG(LogTemp, Log, TEXT("GetRandom array is empty"));
		//	return result;
	}
};
