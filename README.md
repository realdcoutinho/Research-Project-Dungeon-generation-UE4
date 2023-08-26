# Research-Project-Dungeon-generation-UE4

Introduction:
This research project focuses on developing a procedural dungeon generation system within the Unreal Engine 4 environment using C++. The primary objective is to create a dynamic process that generates diverse and randomized dungeons for gameplay.

Dungeon Creation:
The core element of dungeon creation is the C_Dungeon class. This class employs a UStaticMeshComponent named m_pStaticBox to represent rooms. During the class constructor's execution, the static mesh is initialized, taking the form of a mesh cube. However, essential attributes like location, dimensions, and visibility remain unset at this stage, leaving them for subsequent configuration.
An important consideration is that UE4 restricts the generation of static meshes during runtime. Consequently, all potential rooms must be pre-created at compile time, regardless of their eventual use. This process is executed within the C_Generate constructor, where dungeon instances are instantiated and subsequently added to the m_pDungeonArray.

Dungeon Configuration:
The SetDungeons() function in the C_Generate class manages dungeon configuration. It initiates by concealing (setting bHiddenInGame) all static meshes within the m_DungeonArray and emptying the cells in m_pGrid (details on this below).
To introduce an element of randomness into the dungeon generation, a random integer within the range of 0 to 1000 is obtained from RandRange. This integer acts as a seed for an FRandomStream. Given the cell dimensions in m_pGrid, the number of rows and columns, and the minimum position, maximum x and y positions are established for each dungeon.
With the foundational parameters set, the next step is to define dungeon attributes. By employing the previously mentioned FRandomStream alongside the specified minimum and maximum center positions, a genuinely random center point is computed. The same process is repeated for determining the dungeon's width and depth, utilizing the designated measurement ranges.
Once the random center is calculated, it is incorporated into the m_pGrid, which identifies the corresponding cell and returns both its center location and index. At this juncture, the system examines existing dungeons. A radius is established (maximum size of width and depth plus a margin). If another dungeon already occupies the space defined by the current dungeon's center and the calculated radius, the process restarts. Conversely, if the area is unoccupied, the cell is designated as filled, and the current dungeon's attributes (center, width, and depth) are finalized. Furthermore, the dungeon's visibility is activated.



https://github.com/realdcoutinho/Research-Project-Dungeon-generation-UE4/assets/95390453/44115f43-e926-4a9e-9829-5d4367b6cb52

