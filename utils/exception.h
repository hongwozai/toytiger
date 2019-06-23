#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

namespace tiger {

class Location {
 public:

  Location(const std::string &filename, int line)
      : file_(filename),
        line_(line),
        num_(-1) {}

  Location(const std::string &filename, int line, int num)
      : file_(filename),
        line_(line),
        num_(num) {}

  Location(const Location &pos)
      : file_(pos.file_),
        line_(pos.line_),
        num_(pos.num_) {}

  Location(const Location &&pos)
      : file_(std::move(pos.file_)),
        line_(pos.line_),
        num_(pos.num_) {}

  const std::string &File() const noexcept { return file_; }

  const int Line() const noexcept { return line_; }

  const int Num() const noexcept { return num_; }

 private:

  const std::string &file_;
  const int line_;
  const int num_;
};

class Exception {
 public:
  Exception(const std::string &msg, const Location &location)
      : msg_(msg),
        location_(location) {
  }

  const std::string &What() const noexcept { return msg_; }

  const Location &Pos() const noexcept { return location_; }

 private:

  std::string msg_;
  Location location_;
};

}

#endif /* EXCEPTION_H */