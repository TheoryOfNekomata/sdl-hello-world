# Project Architecture

## Overview

This is a bespoke 2D game engine written in C using SDL3 for a gun-and-run horizontal scrolling game. The engine follows a modular architecture with clear separation of concerns across different subsystems.

## Project Structure

```
src/
├── main.c                  # Application entry point
├── G00_app.h              # Main application structure
├── G00_asset.{c,h}        # Asset loading utilities
├── G00_command.{c,h}      # Command system framework
├── G00_commands.h         # Command registry and definitions
├── G00_config.{c,h}       # Configuration script execution
├── G00_database.{c,h}     # SQLite database integration
├── G00_memory.{c,h}       # Memory pool management
├── G00_ui.{c,h}           # User interface system
├── G00_video.{c,h}        # Video/rendering system
├── commands/              # Command implementations
│   ├── G00_global.c      # Global commands (help, exit)
│   ├── G00_memory.c      # Memory-related commands
│   ├── G00_ui.c          # UI-related commands
│   └── G00_video.c       # Video-related commands
├── default.app.cfg        # Default application configuration
├── default.menu.cfg       # Default menu configuration
└── schema.sql             # Database schema
```

## Core Modules

### 1. Application Core (`G00_app.h`, `main.c`)

**Purpose:** Central application state and lifecycle management.

**Key Components:**
- `struct G00_App` - Main application state container
  - `sqlite3* db` - Database connection
  - `unsigned long long ticks` - Frame tick counter
  - `struct G00_Video video` - Video subsystem
  - `struct G00_MemoryState memory` - Memory subsystem
  - `struct G00_UIState ui` - UI subsystem
  - `flags.force_exit` - Application exit flag

**Lifecycle Functions:**
- `G00_AppInit()` - Initializes all subsystems in order:
  1. Load configuration scripts
  2. Initialize memory pool
  3. Initialize database
  4. Initialize SDL3 and SDL_ttf
  5. Initialize video system
  6. Initialize UI system
  7. Load menu configuration
- `G00_AppUpdate()` - Main game loop
- `G00_AppTeardown()` - Cleanup all resources

**Configuration Loading Order:**
1. `default.app.cfg` (required)
2. `autoexec.app.cfg` (optional, skipped with `--ignore-custom`)
3. `default.asset.cfg` (required)
4. `autoexec.asset.cfg` (optional, skipped with `--ignore-custom`)
5. `default.menu.cfg` (required)

### 2. Memory Management (`G00_memory.{c,h}`)

**Purpose:** Pre-allocated memory pool for asset streaming and management.

**Key Components:**
- `struct G00_MemoryState` - Memory subsystem state
  - `config.pool_size_bytes` - Total memory pool size
  - `config.pool_max_entries` - Maximum number of asset entries
  - `config.asset_resource_pack_dir` - Asset directory path
  - `entries[]` - Array of memory offset entries
  - `data` - Raw memory buffer
  - `asset_data_processed_bytes` - Current memory usage

- `struct G00_MemoryOffsetEntry` - Asset metadata
  - `name` - Asset identifier
  - `offset` - Byte offset in memory pool
  - `len` - Asset size in bytes
  - `reference_asset_path` - Original file path

**Functions:**
- `G00_MemoryInit()` - Allocate memory pool
- `G00_MemoryRetrieveIndex()` - Lookup asset by name
- `G00_MemoryTeardown()` - Free memory pool

**Design Pattern:** 
Uses a contiguous memory pool with offset-based indexing for cache efficiency. Assets are loaded into the pool during initialization and referenced by index throughout the application lifecycle.

### 3. Video System (`G00_video.{c,h}`)

**Purpose:** Rendering pipeline and graphics resource management using SDL3.

**Key Components:**
- `struct G00_Video` - Video subsystem state
  - `config` - Video configuration (resolution, FPS, limits)
  - `window` - SDL window handle
  - `renderer` - SDL renderer handle
  - `loaded_textures[]` - Texture pool
  - `loaded_fonts[]` - Font pool
  - `loaded_sprites[]` - Sprite pool

- `struct G00_VideoSprite` - Sprite definition
  - `type` - Object type identifier
  - `texture_index` - Index into texture pool
  - `rect` - Position and dimensions (SDL_FRect)

**Configuration:**
- `screen_width` / `screen_height` - Display resolution
- `aspect_ratio` - Calculated aspect ratio
- `frames_per_second` - Target framerate
- `millis_per_tick` - Calculated frame time
- `max_loaded_textures` - Texture pool capacity
- `max_loaded_fonts` - Font pool capacity
- `max_loaded_sprites` - Sprite pool capacity

**Functions:**
- `G00_VideoInit()` - Create window and renderer
- `G00_VideoUpdate()` - Render frame
- `G00_VideoLoadImageSprite()` - Load image from memory
- `G00_VideoGenerateSurfaceSprite()` - Create colored sprite variant
- `G00_VideoGenerateTextSprite()` - Render text to sprite
- `G00_VideoLoadFont()` - Load font from memory
- `G00_VideoTeardown()` - Cleanup graphics resources

**Rendering Features:**
- Sprite batching with indexed texture system
- Dynamic text rendering
- Colored sprite variants (for shadow effects)
- Parallax scrolling support (demonstrated in menu)

### 4. User Interface (`G00_ui.{c,h}`)

**Purpose:** Menu system and UI state management.

**Key Components:**
- `struct G00_UIState` - UI subsystem state
  - `menus` - Linked list of all menus
  - `current_menu` - Active menu node
  - `current_item` - Selected menu item
  - `messages` - Localization/message key-value pairs
  - `history_stack[]` - Menu navigation history (32 deep)
  - `history_stack_index` - Current stack position

**Node Types:**
- `G00_UI_NODE_TYPE_MENU` - Container for menu items
- `G00_UI_NODE_TYPE_ITEM` - Selectable menu item with command
- `G00_UI_NODE_TYPE_LABEL` - Static text display
- `G00_UI_NODE_TYPE_NODE` - Base node type

**Data Structures:**
- `struct G00_UIMenuNode` - Menu container
  - `label` - Menu title (max 255 chars)
  - `children` - Linked list of child nodes

- `struct G00_UIItemNode` - Selectable item
  - `title` - Display text key
  - `script_commands` - Commands to execute on selection
  - `description_text` - Help text key

- `struct G00_UILabelNode` - Static label
  - `title` - Display text

- `struct G00_MessageEntry` - Localization entry
  - `key` - Message identifier
  - `fallback_value` - Default display text

**Functions:**
- `G00_UIInit()` - Initialize UI state
- `G00_ListAppend()` - Generic linked list append

**Design Pattern:**
Tree-based menu hierarchy with stack-based navigation. Supports localization through message key system with fallback values.

### 5. Database (`G00_database.{c,h}`, `schema.sql`)

**Purpose:** SQLite integration for persistent data storage.

**Schema:**
- `user` table
  - `id` - UUID (36 chars)
  - `username` - User login (256 chars)
  - `password` - Hashed password (256 chars)

**Functions:**
- `G00_DatabaseInit()` - Open database connection and execute schema
- `G00_DatabaseTeardown()` - Close database connection

**Note:** Schema includes TODO comment for migration system implementation.

### 6. Asset Loading (`G00_asset.{c,h}`)

**Purpose:** File I/O for loading assets into memory pool.

**Functions:**
- `G00_AssetLoad()` - Read file into memory state

**Integration:** Works with memory pool to load assets during initialization phase.

### 7. Configuration System (`G00_config.{c,h}`)

**Purpose:** Script-based configuration execution.

**Functions:**
- `G00_ConfigExecuteScript()` - Parse and execute configuration script

**Script Format:** Commands are parsed and executed against the application state, allowing declarative configuration of video settings, memory limits, asset loading, and UI structure.

### 8. Command System (`G00_command.{c,h}`, `G00_commands.h`, `commands/`)

**Purpose:** Extensible command framework for configuration and runtime control.

**Architecture:**
- Command registry with typed argument definitions
- Type-safe argument parsing
- Multiple command contexts (App, Video, Memory, UI)

**Command Types:**
```c
typedef int G00_Command(char*, struct G00_CommandArgumentDefinition, struct G00_App*);
typedef int G00_CommandVideo(char*, struct G00_CommandArgumentDefinition, struct G00_Video*);
typedef int G00_CommandMemory(char*, struct G00_CommandArgumentDefinition, struct G00_MemoryState*);
typedef int G00_CommandUI(char*, struct G00_CommandArgumentDefinition, struct G00_UIState*);
```

**Argument Types:**
- `G00_COMMAND_ARGUMENT_TYPE_U8` - Unsigned 8-bit integer
- `G00_COMMAND_ARGUMENT_TYPE_U16` - Unsigned 16-bit integer
- `G00_COMMAND_ARGUMENT_TYPE_U32` - Unsigned 32-bit integer
- `G00_COMMAND_ARGUMENT_TYPE_U64` - Unsigned 64-bit integer
- `G00_COMMAND_ARGUMENT_TYPE_F32` - 32-bit float
- `G00_COMMAND_ARGUMENT_TYPE_STRING` - String argument

**Available Commands:**

*Global Commands:*
- `help [command]` - Display command list or specific command help
- `exit` - Terminate application

*Video Commands:*
- `video_screen_size <width:u32> <height:u32>` - Set screen resolution
- `video_ideal_fps <fps:f32>` - Set target framerate
- `video_max_loaded_textures <count:u8>` - Set texture pool size
- `video_max_loaded_fonts <count:u8>` - Set font pool size
- `video_max_loaded_sprites <count:u8>` - Set sprite pool size

*Memory Commands:*
- `memory_pool_size_bytes <bytes:u64>` - Set memory pool size
- `memory_pool_max_entries <count:u32>` - Set maximum asset entries
- `memory_asset_load <path:string>` - Load asset into memory pool

*UI Commands:*
- `ui_msg_key <key:string> <fallback_value:string>` - Define localization message
- `ui_menu <label:string>` - Create menu container
- `ui_item <command:string>` - Create selectable menu item
- `ui_title <msg_key:string>` - Set item title message key
- `ui_description_text <msg_key:string>` - Set item description message key
- `ui_label` - Create static label
- `ui_menu_push <label:string>` - Push menu onto history stack
- `ui_menu_pop` - Pop menu from history stack
- `ui_menu_show_start` - Display starting menu

**Design Pattern:**
Static command registry with function pointers and typed argument definitions. Enables declarative configuration through script files while maintaining type safety.

## Data Flow

### Initialization Flow

```
main()
  └─> G00_AppInit()
      ├─> G00_ConfigExecuteScript("default.app.cfg")
      │   └─> Execute video/memory configuration commands
      ├─> G00_MemoryInit()
      │   └─> Allocate memory pool
      ├─> G00_DatabaseInit()
      │   └─> Open SQLite connection
      ├─> G00_ConfigExecuteScript("default.asset.cfg")
      │   └─> Execute asset loading commands
      ├─> SDL_Init()
      ├─> TTF_Init()
      ├─> G00_VideoInit()
      │   ├─> SDL_CreateWindow()
      │   └─> SDL_CreateRenderer()
      ├─> G00_UIInit()
      └─> G00_ConfigExecuteScript("default.menu.cfg")
          └─> Build UI menu hierarchy
```

### Render Loop Flow

```
G00_AppUpdate()
  ├─> G00_AppLoadMenuAssets()
  │   ├─> G00_MemoryRetrieveIndex("font-ui.ttf")
  │   ├─> G00_VideoLoadFont()
  │   ├─> G00_MemoryRetrieveIndex("menu-fg-parallax.png")
  │   └─> G00_VideoLoadImageSprite()
  ├─> G00_AppRenderMenu()
  │   ├─> Generate text sprites for menu items
  │   └─> Position sprites based on menu structure
  ├─> G00_VideoGenerateSurfaceSprite() [multiple shadow layers]
  ├─> Load background parallax layer
  └─> Event Loop
      ├─> SDL_PollEvent()
      │   ├─> Handle SDL_EVENT_QUIT → set force_exit
      │   └─> Handle SDL_EVENT_MOUSE_MOTION → update parallax positions
      └─> G00_VideoUpdate()
          └─> Render all sprites to screen
```

## Memory Management Strategy

### Resource Pooling

All dynamic resources use pre-allocated pools:
- **Memory Pool:** Single contiguous buffer for all assets
- **Texture Pool:** Fixed array of SDL_Texture pointers
- **Font Pool:** Fixed array of TTF_Font pointers
- **Sprite Pool:** Fixed array of sprite descriptors

**Benefits:**
- Predictable memory usage
- Cache-friendly data layout
- No runtime allocation after initialization
- Simple resource management

### Asset Streaming

Assets are loaded once during initialization into the memory pool and referenced by index. This eliminates file I/O during gameplay and ensures consistent performance.

## Rendering Architecture

### Sprite System

Sprites are lightweight descriptors that reference textures in the texture pool:
- Texture index (indirection for flexibility)
- Position and size (SDL_FRect)
- Type identifier (for future extensibility)

### Parallax Scrolling

Implemented through mouse-relative sprite positioning:
- Background layer moves opposite to mouse (depth illusion)
- Foreground layer moves with mouse (exaggerated depth)
- Shadow layers use amplified offsets for dramatic effect

### Text Rendering

Dynamic text-to-texture pipeline:
1. Generate SDL surface from TTF font
2. Convert surface to texture
3. Create sprite referencing the texture
4. Position sprite in world space

## UI System Design

### Menu Hierarchy

Menus are organized as a tree:
- Root menu contains sub-menus and items
- Items contain commands to execute
- Labels provide non-interactive text
- History stack enables navigation back/up

### Localization Support

Message key system allows text substitution:
- Keys reference localized strings
- Fallback values provide default text
- Loaded from configuration scripts

### Command Execution

Menu items store command strings that can be executed when selected (not yet fully implemented in current codebase).

## Configuration System

### Script-Based Configuration

Configuration files are parsed as command scripts:
- One command per line
- Arguments parsed based on command definition
- Executed against application context
- Enables data-driven setup

### Configuration Phases

1. **App Config:** Window settings, memory limits, resource pools
2. **Asset Config:** Load required assets into memory pool
3. **Menu Config:** Build UI hierarchy and localization

## Extension Points

### Adding New Commands

1. Define command function in `commands/` directory
2. Add function signature to `G00_commands.h`
3. Register in `G00_COMMAND_ENTRIES[]` array
4. Specify argument types and counts

### Adding New UI Node Types

1. Add enum value to `G00_UINodeType`
2. Define node structure
3. Add to `union G00_UIMenuChildNode`
4. Implement rendering in `G00_AppRenderMenu()`

### Adding New Asset Types

Memory pool is type-agnostic. New asset types:
1. Load via `memory_asset_load` command
2. Retrieve via `G00_MemoryRetrieveIndex()`
3. Process in subsystem-specific loader

## Performance Considerations

### Memory Layout

- Contiguous asset buffer improves cache performance
- Fixed-size pools avoid heap fragmentation
- Index-based references enable fast lookups

### Rendering Pipeline

- Sprite batching reduces draw calls
- Pre-loaded textures eliminate file I/O
- Fixed timestep enables consistent physics (when implemented)

### Configuration Overhead

All configuration is processed at startup. Runtime performance is unaffected by configuration complexity.

## Known Limitations

1. **Menu Interaction:** Item selection/navigation not fully implemented
2. **Database Usage:** User table present but unused in current code
3. **Asset Hotloading:** No runtime asset reloading support
4. **Memory Pool Fragmentation:** No defragmentation once loaded
5. **Sprite Limits:** Hard-coded pool sizes require recompilation to change
6. **Error Recovery:** Limited error handling in rendering pipeline

## Future Considerations

### Potential Enhancements

1. **Gameplay Systems:**
   - Player character controller
   - Enemy AI and spawning
   - Weapon system and projectiles
   - Collision detection
   - Level scrolling and camera

2. **UI Improvements:**
   - Menu navigation with keyboard/gamepad
   - Item selection visual feedback
   - Animated transitions
   - HUD overlay system

3. **Asset Pipeline:**
   - Asset pack/archive format
   - Compression support
   - Streaming for large levels
   - Asset hot-reloading for development

4. **Graphics Features:**
   - Particle systems
   - Animation system
   - Lighting effects
   - Shader support

5. **Audio:**
   - SDL_mixer integration
   - Music and sound effects
   - Spatial audio

6. **Persistence:**
   - Save game system
   - User profiles
   - Statistics tracking
   - Achievements

## Development Workflow

### Building

See main README.md for build instructions.

### Configuration Testing

1. Modify `default.*.cfg` or `autoexec.*.cfg` files
2. Rebuild and run
3. Use `--ignore-custom` flag to skip autoexec scripts

### Adding Assets

1. Place asset files in asset directory
2. Add `memory_asset_load <path>` to asset configuration
3. Reference asset by name in code via `G00_MemoryRetrieveIndex()`

### Debugging

- Use `help` command to list available commands
- Check stderr output for error messages
- Verify asset paths in configuration files
- Ensure memory pool size accommodates all assets

## Coding Conventions

### Naming

- **Prefix:** All symbols prefixed with `G00_`
- **Modules:** PascalCase after prefix (`G00_Video`, `G00_Memory`)
- **Types:** PascalCase struct names (`struct G00_App`)
- **Functions:** Module + action (`G00_VideoLoadFont`)
- **Enums:** All caps with prefix (`G00_APP_INIT_RESULT_OK`)
- **Files:** Match module name (`G00_video.{c,h}`)

### Structure

- Headers contain interface (types, function signatures)
- Implementation files contain logic
- Circular dependencies resolved through forward declarations
- Command implementations separated by subsystem

### Output Parameters

Multiple return values use `out0`, `out1` naming convention in parameter lists.

### Memory Safety

- All allocations checked for NULL
- Resources freed in reverse initialization order
- Pool-based allocation reduces leak risk
- Fixed-size buffers prevent dynamic allocation issues

## References

- SDL3 Documentation: https://wiki.libsdl.org/SDL3/
- SDL_ttf Documentation: https://wiki.libsdl.org/SDL3_ttf/
- SQLite Documentation: https://www.sqlite.org/docs.html
