#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>

#include "input_stream.h"

#include "utils/log.h"
#include "utils/exception.h"

using namespace std;

namespace tiger {

class FileInputStream : public InputStream {
 public:

  FileInputStream(const string &filename)
      : filename_(filename) {
    file_.open(filename_, ios_base::in);
    if ((file_.rdstate() & ifstream::failbit) != 0) {
      throw Exception(strerror(errno), Location(__FILE__, __LINE__));
    }
  }

  virtual ~FileInputStream() override {
    file_.close();
  }

  virtual bool Read(char *buf, int bufsize, int &len) override {
    if (file_.eof()) {
      len = 0;
      return false;
    }

    len = file_.read(buf, bufsize).gcount();
    return true;
  }

  virtual string InputSource() override { return filename_; }

 private:

  ifstream file_;

  string filename_;
};

class StringInputStream : public InputStream {
 public:

  StringInputStream(const string &content)
      : content_(content),
        pointer_(0) {}

  virtual ~StringInputStream() override {}

  virtual bool Read(char *buf, int bufsize, int &len) override {
    if (pointer_ >= content_.size()) {
      len = 0;
      return false;
    }

    len = std::min(static_cast<size_t>(bufsize), content_.size() - pointer_);
    memcpy(buf, content_.data() + pointer_, len);
    pointer_ += len;
    return true;
  }

  virtual string InputSource() override { return "-"; }

 private:

  int pointer_;

  string content_;
};

shared_ptr<InputStream> MakeFileInputStream(const string &filename)
{
  return make_shared<FileInputStream>(filename);
}

shared_ptr<InputStream> MakeStringInputStream(const string &content)
{
  return make_shared<StringInputStream>(content);
}

}

#if 0
int main(int argc, char *argv[])
{
  using namespace tiger;

  bool flag = false;
  int len = 0;
  unique_ptr<char> buf(new char[123]);
  shared_ptr<InputStream> stream = MakeStringInputStream("fuck you");

  while (flag = stream->Read(buf.get(), 1, len)) {
    cout << "flag: " << flag << ", "
         << "buf: (" << buf.get() << ")" << ", "
         << "len: " << len << endl;
  }
  cout << "flag: " << flag << ", "
       << "buf: (" << buf.get() << ")" << ", "
       << "len: " << len << endl;

  try {
    string filename = "parsing/input_stream.h";
    shared_ptr<InputStream> filestream = MakeFileInputStream(filename);

    cout << "file: " << filename << endl;
    while (flag = filestream->Read(buf.get(), 10, len)) {
      cout << "flag: " << flag << ", "
           << "buf: (" << buf.get() << ")" << ", "
           << "len: " << len << endl;
    }
  } catch (Exception &e) {
    cout << "[" << e.Pos().File()
         << "," << e.Pos().Line() << "]: "
         << e.What() << endl;
  }

  return 0;
}
#endif