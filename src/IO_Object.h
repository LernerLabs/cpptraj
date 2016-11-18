#ifndef INC_IO_OBJECT_H
#define INC_IO_OBJECT_H
/// Abstract base class that all file IO classes will inherit.
class IO_Object {
  public:
    /// IO object categories.
    enum IOtype { NONE = 0, TRAJ, PARM, DATA };
    /// CONSTRUCTOR
    IO_Object() : type_(NONE) {}
    /// CONSTRUCTOR - take IO object type
    IO_Object(IOtype t) : type_(t) {}
    /// \return IO object type
    IOtype Type() const { return type_; }
    /// DESTRUCTOR - virtual since class is inherited
    virtual ~IO_Object() {}
    /// List read options
    virtual void ReadOptions() const = 0;
    /// List write options
    virtual void WriteOptions() const = 0;
    /// \return new instance of this object
    virtual IO_Object* Alloc() const = 0;
  private:
    IOtype type_;
};
#endif
