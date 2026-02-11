#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestTile.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;

UCLASS()
class FIREGMEPROJECTFOLDER_API ATestTile : public AActor
{
	GENERATED_BODY()

public:
	ATestTile();

protected:
	virtual void BeginPlay() override;

private:
	// Components
	UPROPERTY(VisibleAnywhere, Category = "Tile")
	UBoxComponent* Collision;

	UPROPERTY(VisibleAnywhere, Category = "Tile")
	UStaticMeshComponent* Mesh;

	// Assign this in the editor to a material that has a Vector parameter named "Tint"
	UPROPERTY(EditDefaultsOnly, Category = "Tile|Visual")
	UMaterialInterface* BaseMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Tile|Visual")
	FName ColorParamName = TEXT("Tint");

	UPROPERTY(EditAnywhere, Category = "Tile|Visual")
	FLinearColor ColorA = FLinearColor(0.1f, 0.8f, 0.1f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "Tile|Visual")
	FLinearColor ColorB = FLinearColor(0.8f, 0.1f, 0.1f, 1.0f);

	UPROPERTY()
	UMaterialInstanceDynamic* MID;

	bool bIsColorB = false;

	void ToggleColor();
	void ApplyColor(const FLinearColor& C);
};
