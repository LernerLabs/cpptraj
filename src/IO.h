#ifndef INC_IO_H
#define INC_IO_H
#include "IOtokenList.h"
class IO {
  public:
    static void Init();
    static void Free();
    /// Add IO object, keywords, and extensions to the list
    static void AddIO(IO_Object*, int, int, ...);
  private:
    static IOtokenList tokens_; ///< Master list of tokens.
};
#endif
