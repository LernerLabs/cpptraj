#ifndef INC_IOTOKENLIST_H
#define INC_IOTOKENLIST_H
#include "IO_Token.h"
/// Hold an array of IO tokens which cannot be modified.
class IOtokenList {
    typedef std::vector<IO_Token> IOarray;
  public:
    IOtokenList() {}
    ~IOtokenList();
    void Clear();
    void Add(IO_Token const& cIn) { ioList_.push_back( cIn ); }
    typedef IOarray::const_iterator const_iterator;
    const_iterator begin() const { return ioList_.begin(); }
    const_iterator end()   const { return ioList_.end();   }
    IO_Token const& Back()      const { return ioList_.back();  }
  private:
    IOarray ioList_;
};
#endif
