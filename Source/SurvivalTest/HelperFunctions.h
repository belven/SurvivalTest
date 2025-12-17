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

#define mOnScreenMessageBasic(message, duration, colour) GEngine->AddOnScreenDebugMessage(INDEX_NONE, duration, colour, message);
#define mOnScreenMessage(message) mOnScreenMessageBasic(message, 5.0f, FColor::Yellow);

//class ABaseCharacter;
//class ABaseAIController;
//class UTableManager;
//class ABasePlayerController;
//
//USTRUCT(BlueprintType)
//struct FStandardAccessors
//{
//	GENERATED_USTRUCT_BODY()
//
//	FStandardAccessors(ABaseCharacter* character)
//	{
//		baseCharacter = character;
//
//		if (baseCharacter->IsPlayer())
//		{
//			basePlayerController = Cast<ABasePlayerController>(baseCharacter->GetController());
//			baseGameInstance = basePlayerController->GetBaseGameInstance();
//			tableManager = baseGameInstance->GetTableManager();
//		}
//		else
//		{
//			baseAIController = Cast<ABaseAIController>(baseCharacter->GetController());
//			baseGameInstance = GameInstance(character->GetWorld());
//			tableManager = baseGameInstance->GetTableManager();
//		}
//	}
//
//	FStandardAccessors(AController* controller)
//	{
//		basePlayerController = Cast<ABasePlayerController>(controller);
//		baseAIController = Cast<ABaseAIController>(controller);
//
//		if (basePlayerController != NULL)
//		{
//			baseCharacter = basePlayerController->GetBaseCharacter();
//			baseGameInstance = basePlayerController->GetBaseGameInstance();
//			tableManager = baseGameInstance->GetTableManager();			
//		}
//		else
//		{
//			baseCharacter = baseAIController->GetBaseCharacter();
//			baseGameInstance = baseAIController->GetBaseGameInstance();
//			tableManager = baseGameInstance->GetTableManager();
//		}
//	}
//
//	UPROPERTY()
//	ABaseCharacter* baseCharacter = nullptr;
//
//	UPROPERTY()
//	ABasePlayerController* basePlayerController = nullptr;
//
//	UPROPERTY()
//	ABaseAIController* baseAIController = nullptr;
//
//	UPROPERTY()
//	UBaseGameInstance* baseGameInstance = nullptr;
//
//	UPROPERTY()
//	UTableManager* tableManager = nullptr;
//};

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
	
	static FVector IncreaseVectorHeight(const FVector& location, int32 increase)
	{
		FVector newVec;
		newVec.X = location.X;
		newVec.Y = location.Y;
		newVec.Z = location.Z + increase;
		return newVec;
	}


	static FString BoolToString(bool value)
	{
		return value ? "True" : "False";
	}

	template <class T>
	static T GetRandomEnum(T end)
	{
		return static_cast<T>(FMath::RandRange(0, static_cast<uint8>(end) - 1));
	}


	template <class T>
	static T* GetValue(T*& Ptr, UObject* Outer)
	{
		if (!Ptr)
		{
			Ptr = NewObject<T>(Outer);
		}
		return Ptr;
	}

	template <class T>
	static T* GetValue(T*& Ptr, UWorld* world)
	{
		if (!Ptr)
		{
			Ptr = world->SpawnActor<T>(T::StaticClass());
		}
		return Ptr;
	}

	template <class T>
	static T* CastValue(T*& Ptr, UObject* object)
	{
		if (!Ptr)
		{
			Ptr = Cast<T>(object);
		}
		return Ptr;
	}

	template <class T>
	static T GetRandom(TArray<T> itemArray, T defaultValue = NULL)
	{
		if (!itemArray.IsEmpty()) {
			return  itemArray[FMath::RandRange(0, itemArray.Num() - 1)];
		}

		UE_LOG(LogTemp, Log, TEXT("GetRandom encountered empty array"));
		return defaultValue;
	}
};