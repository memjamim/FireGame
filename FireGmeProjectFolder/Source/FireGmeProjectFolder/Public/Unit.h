#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Unit.generated.h"

class ATile;

/*
* Data structure that matches DT_Unit
*/
USTRUCT(BlueprintType)
struct FUnitData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
    int32 ID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
    int32 Unit_Movement_Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
    int32 Stamina;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
    FVector Coordinates;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
    TSoftObjectPtr<UStaticMesh> Model;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
    TSoftObjectPtr<UMaterialInterface> Material;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
    int32 Action_Cost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
    int32 Turns_To_Deploy;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
    TSubclassOf<class AUnit> Unit_BP;
};

UCLASS()
class FIREGMEPROJECTFOLDER_API AUnit : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AUnit();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
    UStaticMeshComponent* UnitMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* Root;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
    FUnitData UnitData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
    UDataTable* UnitDataTable;

    // Tile unit is standing on 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
    ATile* CurrentTile;

    // Current Stamina
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
    int32 CurrentStamina;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
    FVector CubeCoordinates;

    // Selection state
    UPROPERTY(BlueprintReadWrite, Category = "Unit")
    bool bIsSelected;

    UPROPERTY(BlueprintReadWrite, Category = "Unit")
    bool bHasActedThisTurn;

    // Initialization
    UFUNCTION(BlueprintCallable, Category = "Unit")
    void InitializeFromDataTable(FName RowName);

    // Movement
    UFUNCTION(BlueprintCallable, Category = "Unit")
    bool CanMoveToTile(ATile* TargetTile);

    UFUNCTION(BlueprintCallable, Category = "Unit")
    bool MoveToTile(ATile* TargetTile);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Unit")
    void OnMoveComplete();

    // Selection interface
    UFUNCTION(BlueprintCallable, Category = "Unit|Selection")
    void Select();

    UFUNCTION(BlueprintCallable, Category = "Unit|Selection")
    void Deselect();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Unit|Selection")
    void OnSelected();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Unit|Selection")
    void OnDeselected();

    // Cube coordinate helper functions
    UFUNCTION(BlueprintPure, Category = "Unit|Coordinates")
    static int32 GetCubeDistance(FVector CubeA, FVector CubeB);

    UFUNCTION(BlueprintPure, Category = "Unit|Coordinates")
    static bool IsValidCubeCoordinate(FVector Cube);

    UFUNCTION(BlueprintPure, Category = "Unit|Coordinates")
    static TArray<FVector> GetNeighbors(FVector CubeCoord);

    UFUNCTION(BlueprintPure, Category = "Unit|Coordinates")
    TArray<ATile*> GetTilesInRange(int32 Range);

    // Ability system hooks (implement in Blueprint)
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Unit|Abilities")
    void UseAbility(int32 AbilityIndex, ATile* TargetTile);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Unit|Abilities")
    TArray<FString> GetAvailableAbilities();

    UFUNCTION(BlueprintCallable, Category = "Unit|Abilities")
    bool CanUseAbility(int32 AbilityIndex);

    // Turn management
    UFUNCTION(BlueprintCallable, Category = "Unit|Turn")
    void StartTurn();

    UFUNCTION(BlueprintCallable, Category = "Unit|Turn")
    void EndTurn();

    // Stamina management
    UFUNCTION(BlueprintCallable, Category = "Unit")
    void ConsumeStamina(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Unit")
    void RestoreStamina(int32 Amount);

    UFUNCTION(BlueprintPure, Category = "Unit")
    bool HasEnoughStamina(int32 Required) const;
};