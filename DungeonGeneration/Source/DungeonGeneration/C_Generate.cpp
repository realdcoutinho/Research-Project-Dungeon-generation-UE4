// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Generate.h"

// Sets default values
AC_Generate::AC_Generate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Creates Meshes for each location
	m_MaxNumRooms = 20;
	CreateMeshes();

	m_NewSeed = false;
	m_NumberRooms = 3;
	float marginIncrement = 2500;

	m_Margin = marginIncrement * m_NumberRooms;
	m_pGraph = CreateDefaultSubobject<UC_Graph>(TEXT("TriangulationGraph"));
}

void AC_Generate::CreateMeshes()
{
	for (int32 number{ 0 }; number < m_MaxNumRooms; ++number)
	{
		//turns to FString
		FString IntAsString = FString::Printf(TEXT("%d"), number);

		// Create the name for the UStaticMeshComponent with the int32 value from the loop.
		FString actorComponentName = FString::Printf(TEXT("Dungeon%s"), *IntAsString);

		//Creates a Dungeon objects
		UC_Dungeon* newDungeon = CreateDefaultSubobject<UC_Dungeon>(*actorComponentName);

		//Adds To Array
		m_pDungeonArray.Add(newDungeon);
	}
}


void AC_Generate::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyNumberRooms = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyNumberRooms == GET_MEMBER_NAME_CHECKED(AC_Generate, m_NumberRooms))
	{
		// code here to react to changes in m_NumberRooms during gameplay
		SetCells();
		UE_LOG(LogTemp, Warning, TEXT("m_NumberRooms was changed to %d"), m_NumberRooms);
		UE_LOG(LogTemp, Warning, TEXT("New Seed Number was changed to %d"), m_Seed);
	}

	FName PropertyNewSeed = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyNewSeed == GET_MEMBER_NAME_CHECKED(AC_Generate, m_NewSeed))
	{
		// code here to react to changes in m_NewSeed during gameplay
		SetCells();
		UE_LOG(LogTemp, Warning, TEXT("New Seed Number was changed to %d"), m_Seed);
		m_NewSeed = false;
	}

	// Call the parent class's implementation of PostEditChangeProperty
	Super::PostEditChangeProperty(PropertyChangedEvent);
}



// Called when the game starts or when spawned
void AC_Generate::BeginPlay()
{
	Super::BeginPlay();


	//Get m_pGrid in the World
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AC_Grid::StaticClass(), FoundActors);

	//if found, assign it
	if (FoundActors[0] != nullptr)
		m_pGrid = Cast<AC_Grid>(FoundActors[0]);
}

void AC_Generate::SetCells()
{
	// Loop over all the dungeons, and hide them in game (insivible) (bHiddenInGame = true)
	for (UC_Dungeon* d : m_pDungeonArray)
	{
		d->SetVisibility(true);
	}

	//Empty Ce;;
	if (m_pGrid->GetArraySize() > 0)
		m_pGrid->EmptyCells();

	//Get Seed
	m_Seed = FMath::RandRange(0, 1000 - 1);
	//GetRandomStream based on new Seed
	FRandomStream RandomStream(m_Seed);

	int32 nrRows = 100;
	int32 nrColumns = 100;
	int32 minPosition = 0;
	int32 lenght = 100;
	int32 maxPosition = nrColumns * lenght;

	int32 minSize = 300;
	int32 maxSize = 600;
	
	//go over all the number desirable of rooms
	for (int32 i{ 0 }; i < m_NumberRooms; ++i)
	{
		//while overlap is true, run. if not, skip to next index
		bool bOverlap = false;
		do
		{
			//Random center given 0, lowest x and y, and maxPosition, highest x and y (since width and depth are the same value, and we have nrRows = nrColumns
			FVector randomCenter = FVector(RandomStream.FRandRange(minPosition, maxPosition), RandomStream.FRandRange(minPosition, maxPosition), 0);
			
			//get a random width
			int32 width = RandomStream.RandRange(minSize, maxSize);
			//get random depth
			int32 depth = RandomStream.RandRange(minSize, maxSize);

			//find cell index at random center
			int32 index = m_pGrid->GetCellIndex(randomCenter);
			//Get cell at given index
			FCell* cell = m_pGrid->GetCellAtIndex(index);
			//assign its index to itself
			cell->_index = index;

			//new center == cell center
			FVector center = FVector(cell->_center.X, cell->_center.Y, cell->_center.Z);

			bOverlap = false;
			//loop over existing dungeons in dungeon array
			for (const UC_Dungeon* ExistingDungeon : m_pDungeonArray)
			{
				//if current cell is not itself
				if (ExistingDungeon != m_pDungeonArray[i])
				{

					float margin = 200.0f;
					//this circle radius will define an area in which a new dungeon cannot be placed
					float circleRadius = maxSize + margin;
					

					// Define the point you want to check
					FVector PointToCheck = ExistingDungeon->m_Center;

					// Calculate the squared distance between the circle's center and the point
					float SquaredDistance = FVector::DistSquared(center, PointToCheck);

					// Compare the squared distance to the squared radius
					float SquaredRadius = circleRadius * circleRadius;
					if (SquaredDistance <= SquaredRadius)
					{
						bOverlap = true;
						break;
					}
				}


			}

			if (bOverlap != true)
			{
				if (!cell->isEmpty())
				{
					bOverlap = true;
					break;
				}

				cell->SetFull();
				//give the static mesh in dungeon its position, width and depth
				m_pDungeonArray[i]->SetVariables(center, width, depth);
				//make it visible (notHidden) for render
				m_pDungeonArray[i]->SetVisibility(false);
				break;
			}

		} while (bOverlap);

	}

	int32 radiusSuperTriangle = 5000;
	if (m_pGraph->m_Locations.Num() > 0)
		m_pGraph->DeletePoints();
	m_pGraph->CreateSuperTriangle(radiusSuperTriangle, m_NumberRooms, m_Margin);

	//points for triangulation will be the dungeons center
	for (int32 j{ 0 }; j < m_NumberRooms; ++j)
	{
		m_pGraph->AddPoint(m_pDungeonArray[j]->m_Center);
	}

	//run triangulation algorithm
	m_pGraph->TriangulationAlgorithm();
}

// Called every frame
void AC_Generate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugFunctions();
}

void AC_Generate::DrawDebugFunctions() const
{
	if (m_DrawDebugTriangulation)
		m_pGraph->DrawDebugTriangulation();

	if (m_DrawDebugMST)
		m_pGraph->DrawDebugMTS();

	if (m_DrawDebugGrid)
		m_pGrid->DrawDebugGrid();

	if (m_DrawDebugAStar)
		m_pGrid->DrawDebugAStar();
}

