#ifndef G00_CONFIG_H
#define G00_CONFIG_H

#include "G00_database.h"
#include "G00_video.h"
#include "G00_memory.h"

enum G00_UINodeType {
  G00_UI_NODE_TYPE_MENU,
  G00_UI_NODE_TYPE_LABEL,
  G00_UI_NODE_TYPE_ITEM,
};

struct G00_UILabelNode {
  enum G00_UINodeType type;
  char* title;
};

struct G00_UIItemNode {
  enum G00_UINodeType type;
  char* script_commands;
  char* title;
  char* description_text;
};

struct G00_UIMenuNode {
  enum G00_UINodeType type;
  char* label;
};

union G00_UINode {
  struct G00_UIMenuNode menu;
  struct G00_UIItemNode item;
  struct G00_UILabelNode label;
};

struct G00_UINodeList {
  struct G00_UIMenuNode* data;
  struct G00_UINodeList* next;
};

struct G00_App {
  sqlite3* db;
  unsigned long long ticks;
  struct G00_Video video;
  struct G00_MemoryState memory;
  struct G00_UINodeList* ui_root;

  struct G00_UIMenuNode* current_menu;
  struct G00_UIItemNode* current_item;
};

enum G00_CommandArgumentType {
  G00_COMMAND_ARGUMENT_TYPE_U8 = 0x1,
  G00_COMMAND_ARGUMENT_TYPE_U16 = 0x2,
  G00_COMMAND_ARGUMENT_TYPE_U32 = 0x4,
  G00_COMMAND_ARGUMENT_TYPE_U64 = 0x8,
  G00_COMMAND_ARGUMENT_TYPE_F32 = 0x10,
  G00_COMMAND_ARGUMENT_TYPE_STRING = 0x20,
};

struct G00_CommandArgument {
  const char* name;
  enum G00_CommandArgumentType type;
};

struct G00_CommandArgumentDefinition {
  unsigned char minimum;
  unsigned char maximum;
  struct G00_CommandArgument* defs;
};

struct G00_CommandEntry {
  const char* name;
  void* execute_fn;
  struct G00_CommandArgumentDefinition args;
};

void G00_ConfigExecuteScript(const char*, struct G00_App*);

#endif
