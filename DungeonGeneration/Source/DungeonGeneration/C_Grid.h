// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_Block.h"
#include "DrawDebugHelpers.h"


#include "C_Grid.generated.h"
enum class FCellType
{
	Room,
	Corridor,
	Empty
};

USTRUCT(BlueprintType)
struct FGridConnection
{
	GENERATED_BODY()
		FGridConnection() {};
	FGridConnection(int32 from, int32 to, float cost = 1.f)
		: m_From(from),
		m_To(to),
		m_Cost(cost)
	{
	};

	int GetFrom() const { return m_From; }
	void SetFrom(int newFrom) { m_From = newFrom; }

	int GetTo() const { return m_To; }
	void SetTo(int newTo) { m_To = newTo; }

	float GetCost() const { return m_Cost; }
	void SetCost(float newCost) { m_Cost = newCost; }

	bool IsValid() const { return (m_From != -1 && m_To != -1); }

	bool operator==(const FGridConnection& rhs) const
	{
		return rhs.m_From == this->m_From &&
			rhs.m_To == this->m_To &&
			rhs.m_Cost == this->m_Cost;
	}

	bool operator!=(const FGridConnection& rhs) const
	{
		return !(*this == rhs);
	}

	int m_From;
	int m_To;

	// the cost of traversing the edge
	float m_Cost;
};


//Struct used for Astart Algorithm
USTRUCT(BlueprintType)
struct FNodeRecord
{
	GENERATED_BODY()

	FCell* pNode = nullptr;
	FGridConnection pConnection;
	float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
	float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

	bool operator==(const FNodeRecord& other) const
	{
		return pNode == other.pNode
			&& pConnection == other.pConnection
			&& costSoFar == other.costSoFar
			&& estimatedTotalCost == other.estimatedTotalCost;
	};

	bool operator<(const FNodeRecord& other) const
	{
		return estimatedTotalCost < other.estimatedTotalCost;
	};
};

USTRUCT(BlueprintType)
struct FCell
{
	GENERATED_BODY()


	FCell() {};
	FCell(FVector bottomLeft, float width, float depth, AC_Grid* grid,FCellType type = FCellType::Empty)
		: pStaticBox(nullptr),
		_bottomLeft(bottomLeft),
		_width(width),
		_depth(depth),
		_type(FCellType::Empty),
		_isEmpty(true),
		_isCorridor(false)
	{
		_center = FVector(_bottomLeft.X + (_width / 2.0f), _bottomLeft.Y + (_depth / 2.0f), 0);
	}

	UStaticMeshComponent* pStaticBox;
	//list of connections. minimum of 2, maximum of 4
	TArray<FGridConnection> _connections;

	//hides the sattic meshes
	void SetVisibillity(bool isHidden)
	{
		pStaticBox->bHiddenInGame = isHidden;
		pStaticBox->MarkRenderStateDirty();
	}

	bool isEmpty()
	{
		//if (_type == FCellType::Empty)
		//	return true;
		//return false;
		return _isEmpty;
	}

	void SetFull()
	{
		_type = FCellType::Room;
		_isEmpty = false;
	}

	FVector _bottomLeft;
	FVector _center;
	float _width;
	float _depth;
	FCellType _type;
	float _cost;
	bool _isEmpty = true;
	int32 _index;
	bool _isCorridor = false;

	bool operator==(const FCell& Other) const
	{
		if (_bottomLeft == Other._bottomLeft)
			return true;

		return false;
	}


};



UCLASS()
class DUNGEONGENERATION_API AC_Grid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_Grid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Returns the index of a cell given its position
	int32 GetCellIndex(const FVector& pos) const;
	//Returns the Cell given an index
	FCell* GetCellAtIndex(int32 index);
	//return the array size
	int32 GetArraySize();

	//"Empties the cells" turns the static meshes invisible
	void EmptyCells();

	void AStartPath(const FVector& startPos, const FVector& endPos);
	float GetHeuristicCost(const FCell* pStartNode, const FCell* pEndNode) const;


	//Debug Drawing Functions
	void DrawDebugGrid() const;
	void DrawDebugAStar() const;

private:

	int32 m_NrRow = 100;
	int32 m_NrColumns = 100;

	float m_Width = 100;
	float m_Depth = 100;
	TArray<FCell> m_CellsArray;
	TArray<FVector> m_Directions;


	//creates each individual cell
	void CreateCells();
	//creates connections for each individual cell
	void CreateConnections();


	//finds the index of the row given yPos
	int32 GetRowIndex(const float yPosition) const;
	//finds the index of the column given xPos
	int32 GetColumnIndex(const float xPosition) const;


};
