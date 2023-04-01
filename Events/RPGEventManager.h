#pragma once
#include "CoreMinimal.h"
#include "BaseEvent.h"
#include "EventListener.h"
#include "UObject/NoExportTypes.h"
#include "RPGEventManager.generated.h"

#define mEventTriggered(GameInstance, baseEvent)  GameInstance->GetEventManager()->EventTriggered(baseEvent)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventTriggered, UBaseEvent*, eventTriggered);

USTRUCT(BlueprintType)
struct FListeners
{
	GENERATED_USTRUCT_BODY()
public:
	TArray<IEventListener*>  listeners;
};

UCLASS()
class SURVIVALTEST_API URPGEventManager : public UObject
{
	GENERATED_BODY()

public:
	TMap<EEventType, FListeners>& GetManagerListeners() { return ManagerListeners; }

	void RegisterListener(TArray<EEventType> types, IEventListener* listener);
	void EventTriggered(UBaseEvent* inEvent);

	UPROPERTY()
		FEventTriggered OnEventTriggered;
private:
	UPROPERTY()
		TMap<EEventType, FListeners> ManagerListeners;
};