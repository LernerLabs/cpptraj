#ifndef INC_FILE_H
#define INC_FILE_H
#include <string>
#include <vector>
/// This namespace contains useful file-related routines.
namespace File {
  /// File name, path, extension etc.
  class Name;
  /// Array of file names
  typedef std::vector<Name> NameArray;
  /// Expand given expression to array of file names
  NameArray ExpandToFilenames(std::string const&);
  /// File access types
  enum AccessType { READ=0, WRITE, APPEND, UPDATE };
  /// File compression types
  enum CompressType { NO_COMPRESSION=0, GZIP, BZIP2, ZIP };
  /// Basic file; no IO routines.
  class Base;
  /// Print error message corresponding to 'false' value from 'Exists()'
  void ErrorMsg(const char*);
  /// return last file error message
  const char* StrError();
  /// \return true if file exists and is accessible.
  bool Exists(std::string const&); // TODO remove?
  bool Exists(Name const&);
}

/** Class to hold file name, extension, etc. */
class File::Name {
  public:
    Name() {}
    Name(std::string const& s) { SetName(s); }
    Name(const char* s) { SetName( std::string(s) ); }
    Name(const Name&);
    Name& operator=(const Name&);
    /// Set file name and extensions, perform expansion as necessary.
    int SetName(std::string const&);
    /// Set file name, no expansions.
    int SetName_NoExpansion(std::string const&);
    /// Append given string to file name but do not change extension info.
    int Append(std::string const&); //TODO this can probably replace AppendNumber some places
    /// \return File name with given string appended - do not change extension info.
    Name AppendName(std::string const&) const;
    /// \return File name with given string prepended to base file name.
    Name PrependName(std::string const&) const;
    /// \return File name with given string prepended to extension
    Name PrependExt(std::string const&) const;
    /// Clear File name
    void clear();
    /// \return true if string matches full or base file name.
    bool MatchFullOrBase(std::string const&) const;

    const std::string& Full()      const { return fullPathName_;         }
    const std::string& Base()      const { return baseName_;             }
    const std::string& Ext()       const { return extension_;            }
    const char* full()             const { return fullPathName_.c_str(); }
    const char* base()             const { return baseName_.c_str();     }
    const char* ext()              const { return extension_.c_str();    }
    const std::string& Compress()  const { return compressExt_;          }
    const std::string& DirPrefix() const { return dirPrefix_;            }
    bool empty()                   const { return fullPathName_.empty(); }
  private:
    std::string fullPathName_;
    std::string baseName_;
    std::string extension_;
    std::string compressExt_;
    std::string dirPrefix_;
};

/** Base class that all files will inherit. */ 
class File::Base {
  public:
    Base();
    Base(int);
    virtual ~Base() {} // Virtual since class is inherited
    Base(Base const&);
    Base& operator=(Base const&);
    Name const& Filename()     const { return fname_;        }
    unsigned int Size()        const { return file_size_;    }
    int Debug()                const { return debug_;        }
    AccessType Access()        const { return access_;       }
    CompressType Compression() const { return compressType_; }
    bool IsOpen()              const { return isOpen_;       }
    bool IsStream()            const { return isStream_;     }
    /// \return string based on current access
    const char* accessStr()    const { return AccessTypeName_[access_]; }

    void SetDebug(int d) { debug_ = d; }
    /// Set up file for given access but do not open.
    int Setup(Name const&, AccessType);
    /// Open the file with current access.
    int Open();
    /// Setup the file with given access and open.
    int Open(Name const&, AccessType);
    /// Close the file.
    void Close();
  protected:
    /// File-specific setup, called by Setup()
    virtual int InternalSetup() = 0;
    /// File-specific open, called by Open()
    virtual int InternalOpen() = 0;
    /// File-specific close, called by Close()
    virtual void InternalClose() = 0;
  private:
    static const char* AccessTypeName_[];

    Name fname_;
    unsigned int file_size_;           ///< Actual file size
    int debug_;
    AccessType access_;         ///< Current file access
    CompressType compressType_; ///< Type of compression present
    bool isOpen_;               ///< True if file is open and ready for IO
    bool isStream_;             ///< True if file is to/from a stream.
};
#endif