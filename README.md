#VulkanEngine

Window : game.h, game.cpp
------------
This Game class encapsulates a game loop structure with Vulkan rendering and GLFW for window management, along with multithreaded updates for sound, network, and game logic. It initializes states, manages input, and handles window callbacks effectively for a game application.

Rendering : VulkanRenderer.h, VulkanRenderer.cpp, Utilities.h
---------------
This Renderer class encapsulates a significant amount of Vulkan-specific functionality and provides an interface for managing rendering, resources, and UI elements within a Vulkan application. Each method and member serves a specific role in initializing, rendering, and cleaning up Vulkan resources.

Utilities.h code consists of Vulkan-related utilities and structures for handling rendering tasks. It includes definitions for constants, vertex data structures, queue families, swap chain details, and various helper functions for file reading, buffer creation, memory type detection, and command buffer management. These utilities are essential for setting up Vulkan applications, managing resources, and performing graphics operations efficiently.

Models: Mesh.h, Mesh.cpp, MeshModel.h, MeshModel.cpp
-----------
The Mesh class encapsulates Vulkan-specific functionalities for managing vertex and index buffers, along with model and normal matrices. It provides methods to interact with these resources and manages their creation and destruction efficiently within a Vulkan application context.

The MeshModel class is designed to manage a collection of Mesh objects in a Vulkan application. It provides methods to access and modify the model matrix, load meshes from an Assimp scene, and destroy Vulkan resources associated with the meshes. This class encapsulates functionality for handling complex models composed of multiple meshes efficiently.

States, StateMachine: state.h, state_machine.h

The State class serves as a foundational element for implementing different game states, ensuring consistency in state management across various parts of a game or application. It promotes modularity and encapsulation by separating state-specific logic into discrete classes that adhere to a common interface.

The StateMachine class encapsulates state management logic, allowing for flexible and organized control over different states within an application or game. It leverages smart pointers for efficient memory management and provides methods to add, remove, switch, and clear states as needed, ensuring robust and maintainable software design.

These classes (TemplateState and MainMenuState) are designed to encapsulate specific states within an application or game, inheriting from a base State class (state.h). They provide methods to initialize, update, render, handle input, and manage data related to their respective states, utilizing various utility functions for matrix operations, quaternion handling, and UI rendering. The use of #pragma once ensures that header files are included only once to prevent multiple definitions and related errors during compilation.


