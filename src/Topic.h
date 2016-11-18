#ifndef INC_TOPIC_H
#define INC_TOPIC_H
#include "DispatchObject.h"
#include "ArgList.h"
/// Topic is not a command, but a means of obtaining more info on certain topics.
class Topic : public DispatchObject {
  public:
    Topic() : DispatchObject(TOPIC) {}
    DispatchObject* Alloc() const { return 0; }
    void Help() const {}
    virtual void PrintTopic(ArgList&) const = 0;
};

class Topic_Trajout : public Topic { public: void PrintTopic(ArgList&) const; };
#endif
