#include <cstdarg>
#include "IO.h"

void IO::AddIO(IO_Object* oIn, int nKeys, int nTotal, ...) {
  IO_Token::Sarray keys, exts;
  int nExts = nTotal - nKeys;
  va_list args;
  va_start(args, nTotal);
  for (int nk = 0; nk < nKeys; nk++) {
    char* key = va_arg(args, char*);
    keys.push_back( std::string(key) );
  }
  for (int ne = 0; ne < nExts; ne++) {
    char* ext = va_arg(args, char*);
    exts.push_back( std::string(ext) );
  }
  va_end(args);
  tokens_.Add( IO_Token(oIn, keys, exts) );
}
