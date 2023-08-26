// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Grid.h"


// Sets default values
AC_Grid::AC_Grid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_NrRow = 100;
	m_NrColumns = 100;
	m_Width = 100;
	m_Depth = 100;


	m_Directions = {
		FVector(1, 0, 0),
		FVector(0, 1, 0),
		FVector(-1, 0, 0),
		FVector(0, -1, 0)
	};


	CreateCells();
}

// Called when the game starts or when spawned
void AC_Grid::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void AC_Grid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AC_Grid::CreateCells()
{
	//run throw every cell index
	for (int32 column{ 0 }; column < m_NrColumns; ++column)
	{
		for (int32 row{ 0 }; row < m_NrRow; ++row)
		{
			//create cell
			FCell cell = FCell({ row * m_Width, column * m_Depth, 0 }, m_Width, m_Depth, this, FCellType::Empty);
			//create index
			cell._index = column * m_NrColumns + row;

			//create static mesh. give it a distinct name
			FString IntAsString = FString::Printf(TEXT("%d"), cell._index);
			FString StaticMeshComponentName = FString::Printf(TEXT("StaticMeshComponent_%s"), *IntAsString);

			UStaticMeshComponent* pStaticBox = CreateDefaultSubobject<UStaticMeshComponent>(*StaticMeshComponentName);
			static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
			if (MeshAsset.Succeeded())
			{
				pStaticBox->SetStaticMesh(MeshAsset.Object);
			}

			//assign the static mesh
			cell.pStaticBox = pStaticBox;
			//set the location and scale of said static mesh
			cell.pStaticBox->SetRelativeLocation(cell._center);
			FVector scale = FVector(m_Width, m_Depth, 100.0f); // Adjust the scale factors as needed.
			cell.pStaticBox->SetRelativeScale3D(scale / 100);
			//make it invisible at the start
			cell.SetVisibillity(true);

			//add it to the TArray
			m_CellsArray.AddUnique(cell);
		}
	}

	//Now with cells created, create connections between cells
	CreateConnections();
}

void AC_Grid::CreateConnections()
{
	//loops through all existing cells
	for (FCell& cell : m_CellsArray)
	{
		//find its column
		int32 col = GetColumnIndex(cell._center.X);
		//find its row
		int32 row = GetRowIndex(cell._center.Y);

		//now loop thourgh every desirable direction each connection should take
		for (const FVector& direction : m_Directions)
		{
			//find its right/left neighboring column
			int32 neighborCol = col + direction.X;
			//find its forward/back neighboring row
			int32 neighborRow = row + direction.Y;

			//does said column and row indexes exist?
			if (neighborCol >= 0 && neighborCol < m_NrColumns && neighborRow >= 0 && neighborRow < m_NrRow)
			{
				//if so...

				//calculate the cell index
				int32 neighborIdx = neighborRow * m_NrColumns + neighborCol;

				//create a connections
				FGridConnection connection = FGridConnection(cell._index, neighborIdx, 1.0f);

				//add said connection to the cell
				cell._connections.Add(connection);
			}
		}
	}
}

void AC_Grid::AStartPath(const FVector& startPos, const FVector& endPos)
{
	int32 startIndex = GetCellIndex(startPos);
	int32 endIndex = GetCellIndex(endPos);

	FCell* startCell = GetCellAtIndex(startIndex);
	FCell* endCell = GetCellAtIndex(endIndex);

	TArray<FCell*> path;
	TArray<FNodeRecord> openList;
	TArray<FNodeRecord> closedList;
	FNodeRecord currentRecord;

	FNodeRecord startRecord;
	startRecord.pNode = startCell;
	startRecord.pConnection;
	startRecord.estimatedTotalCost = GetHeuristicCost(startCell, endCell);
	openList.AddUnique(startRecord);
	float costSoFar{};

	while (openList.Num() != 0)
	{
		int32 lowestCostIndex = INDEX_NONE;
		float lowestCost = FLT_MAX;
		for (int32 i = 0; i < openList.Num(); ++i)
		{
			if (openList[i].estimatedTotalCost < lowestCost)
			{
				lowestCost = openList[i].estimatedTotalCost;
				lowestCostIndex = i;
			}
		}

		if (lowestCostIndex != INDEX_NONE)
		{
			currentRecord = openList[lowestCostIndex];
			///openList.RemoveAt(lowestCostIndex); // Remove the element from the openList
		}

		if (currentRecord.pNode == endCell)
		{
			break;
		}

		TArray<FGridConnection> recordConnections = currentRecord.pNode->_connections;

		for (FGridConnection& connection : recordConnections)
		{
			costSoFar = currentRecord.costSoFar + connection.m_Cost;

			////// 2.D 
			for (const FNodeRecord& closed : closedList)
			{
				if (connection.m_To == closed.pNode->_index)
				{
					if (closed.costSoFar <= costSoFar)
					{
						break;
					}
					else
					{
						closedList.Remove(closed);
					}
				}
			}
			////// 2.E
			for (const FNodeRecord& open : openList)
			{
				if (connection.m_From == open.pNode->_index)
				{
					if (open.costSoFar <= costSoFar)
					{
						break;
					}
					else
					{
						openList.Remove(open);
					}
				}
			}

			FNodeRecord newRecord;
			newRecord.pNode = GetCellAtIndex(connection.m_To);
			newRecord.pConnection = connection;
			newRecord.costSoFar = costSoFar;
			newRecord.estimatedTotalCost = costSoFar + GetHeuristicCost(newRecord.pNode, endCell);
			openList.AddUnique(newRecord);
		}

		openList.Remove(currentRecord);
		closedList.AddUnique(currentRecord);
	}

	while (currentRecord.pNode != startCell)
	{
		path.AddUnique(currentRecord.pNode);

		for (const auto& closed : closedList)
		{
			if (currentRecord.pConnection.m_From == closed.pNode->_index)
			{
				currentRecord = closed;
				break;
			}
		}
	}

	for (FCell* cell : path)
	{
		cell->_isCorridor = true;
		cell->SetVisibillity(false);
	}


	//NO NEED TO REVERSE PATH
	//path.AddUnique(startCell);
	//int32 StartIndex = 0;
	//int32 EndIndex = path.Num() - 2;

	//while (StartIndex < EndIndex)
	//{
	//    FCell* Temp = path[StartIndex];
	//    path[StartIndex] = path[EndIndex];
	//    path[EndIndex] = Temp;

	//    ++StartIndex;
	//    --EndIndex;
	//};

	//std::reverse(path.begin(), path.end());

}

float AC_Grid::GetHeuristicCost(const FCell* pStartNode, const FCell* pEndNode) const
{
	float deltaX = static_cast<float>(pEndNode->_center.X - pStartNode->_center.X);
	float deltaY = static_cast<float>(pEndNode->_center.Y - pStartNode->_center.Y);
	return FMath::Sqrt(deltaX * deltaX + deltaY * deltaY);
}



#pragma region IndexPosition Calculations
int AC_Grid::GetCellIndex(const FVector& pos) const
{
	int widthIndex{ GetColumnIndex(pos.X) }; //Gets the width-> Column Index
	int heightIndex{ GetRowIndex(pos.Y) };

	int index{ heightIndex * (m_NrColumns)+widthIndex };

	return index;
}

int AC_Grid::GetColumnIndex(const float xPosition) const
{
	int heightIndex{ static_cast<int>(xPosition / m_Depth) }; //The result will gives us the number of the row to which the yPos belongs to
	//Because the result is an int, it will the correct cordinate, most of the times
	heightIndex = FMath::Clamp(heightIndex, 0, m_NrRow - 1);	   //The yPos might go above the m_SpaceHeight. If that is the case the heightIndex might go outside of bounds
	//That is fixed by clamping the width index between two values: 0 (min index) and m_NrOfRows-1 (max index)  
	return heightIndex;
}


int AC_Grid::GetRowIndex(const float yPosition) const
{
	int widthIndex{ static_cast<int>(yPosition / m_Width) }; //The result will gives us the number of the column to which the xPos belongs to
	//Because the result is an int, it will the correct cordinate, most of the times
	widthIndex = FMath::Clamp(widthIndex, 0, m_NrColumns - 1);    //The xPos might go above the m_SpaceWidth. If that is the case the widthIndex might go outside of bounds
	//That is fixed by clamping the width index between two values: 0 (min index) and m_NrOfCols-1 (max index)  
	return widthIndex;
}

FCell* AC_Grid::GetCellAtIndex(int32 index)
{
	return &m_CellsArray[index];
}

void AC_Grid::EmptyCells()
{
	//m_CellsArray.Empty();
	for (FCell& cell : m_CellsArray)
	{
		cell._isEmpty = true;
		cell._isCorridor = false;
		cell.SetVisibillity(true);
	}
}

int32 AC_Grid::GetArraySize()
{
	return m_CellsArray.Num();
}

void AC_Grid::DrawDebugGrid() const
{
	for (const FCell& cell : m_CellsArray)
	{
		const FVector& bl = cell._bottomLeft;
		const FVector br{ bl.X + cell._width, bl.Y, 0 };
		const FVector tl{ bl.X, bl.Y + cell._depth, 0 };
		const FVector tr{ bl.X + cell._width, bl.Y + cell._depth, 0 };

		const FColor color = FColor::Blue;
		const float duration = -1.f;
		const uint8 depthPriority = 0;
		const float thickness = 6.f;

		DrawDebugLine(GetWorld(), bl, br, color, false, duration, depthPriority, thickness);
		DrawDebugLine(GetWorld(), bl, tl, color, false, duration, depthPriority, thickness);
		DrawDebugLine(GetWorld(), tl, tr, color, false, duration, depthPriority, thickness);
		DrawDebugLine(GetWorld(), tr, br, color, false, duration, depthPriority, thickness);
	}
}

void AC_Grid::DrawDebugAStar() const
{
	for (const FCell& cell : m_CellsArray)
	{
		if (cell._isCorridor)
		{
			const FVector& center = cell._center;
			const FColor color = FColor::Yellow;
			const float size = 5.0f;
			DrawDebugPoint(GetWorld(), { center.X, center.Y, 80.0f }, size, FColor::Yellow, false, -1.f, 0);
		}
	}
}