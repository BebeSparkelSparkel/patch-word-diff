
typedef struct {
  int count;
  FilePatch
} Patch;

enum {
  Git,
  FileDiff,
} PatchType;

typedef char* Path;
typedef char* Hash;
typedef uint16_t FileMode;

typedef struct {
  Path pathA, pathB;
  Hash indexA, indexB;
  FileMode fileMode;
  FileDiff *fileDiff;
} Git;

typedef struct {
  Path pathA, pathB;
  int count;
  HunkHeader *hunkHeaders;
} FileDiff;

typedef int Line;
typedef int Column;

struct {
  Line lineA, lineB;
  Column columnA, columnB;
  int count;
  Diff *diffs;
} HunkHeader;

typedef enum {Match, Addition, Removal} DiffType;
typedef struct {
  DiffType type;
  char *string;
} Diff;

