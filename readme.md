# Fairlight

A custom game engine based on the core engine of a 2D platformer I was working on. Currently working on ripping out the 2D systems and replacing them with new 3D systems.

## Notable Features

- Entity Component System
    - Entities are a 32 bit handle storing an index and generation. Only one entity can be alive with a specific index. The generation field can be used to detect if an entity is alive; if an entity index is being handed out again after that entity was destroyed, the generation field will be incremented.
    - Component data is managed entirely by the systems and can be laid out in whatever manner is most efficient for that system.
- Asset Pipeline
    - Assets on disk get cooked offline to an efficient format for runtime.
    - Individual assets are compressed and stored together in a pack file.
- AngelScript Integration
    - Script compilation errors are reported during asset cook time.
    - Script variable default values can be adjusted in scenes and prefabs. Since scripts are compiled at cook time, we can check that the specified variable exists and that the type matches.
