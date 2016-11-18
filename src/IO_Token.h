#ifndef INC_IO_TOKEN_H
#define INC_IO_TOKEN_H
#include "IO_Object.h"
#include <vector>
#include <string>
/// Class for holding IO object and its associated keywords/extensions.
class IO_Token {
  public:
    typedef std::vector< std::string > Sarray; // TODO put in common header?
    /// CONSTRUCTOR
    IO_Token() : object_(0) {}
    /// CONSTRUCTOR - Take IO_Object pointer, keywords, and extensions
    IO_Token(IO_Object* o, Sarray const& k, Sarray const& e) : object_(o), keys_(k), exts_(e) {}
    /// Free IO_Object
    void Clear();
    /// \return true if no IO_Object
    bool Empty() const { return object_ == 0; }
    /// \return const reference to underlying IO_Object
    IO_Object const& Obj() const { return *object_; }
    
  private:
    IO_Object* object_; ///< Pointer to IO_Object
    Sarray keys_;       ///< Format keywords
    Sarray exts_;       ///< Format extensions
};
#endif
