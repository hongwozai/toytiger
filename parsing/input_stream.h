#ifndef INPUT_STREAM_H
#define INPUT_STREAM_H

#include <string>
#include <memory>

namespace tiger {

class InputStream {
 public:

  InputStream() = default;

  virtual ~InputStream() {}

  /**
   * 异常情况下，抛出异常
   * 该接口需要在读取结束后每次读取都能返回false
   * @param buf 读取的数据会放入buf中
   * @param bufsize buf的大小
   * @param len 当读取结束时，buf中有多少数据
   * @return bool 读取到结尾了返回true，否则返回false
   */
  virtual bool Read(char *buf, int bufsize, int &len) = 0;

  /* 用于debug */
  virtual std::string InputSource() = 0;
};

std::shared_ptr<InputStream> MakeFileInputStream(const std::string &filename);

std::shared_ptr<InputStream> MakeStringInputStream(const std::string &content);

}

#endif /* INPUT_STREAM_H */