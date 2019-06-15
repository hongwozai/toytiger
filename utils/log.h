#ifndef LOG_H
#define LOG_H

#include <cassert>
#include <cstdarg>
#include <unordered_map>

namespace tiger {

class Log {
 public:

  enum {
    kSilence = 0,
    kError = 1,
    kLog = 2,
    kDebug,
  };

 public:

  static Log &Instance() {
    static Log log;
    return log;
  }

  void Set(const std::string &tag, int level) {
    access_[tag] = level;
  }

  /**
   * 1. 无设置默认新模块输入打开Log级别日志
   * 2. 等级越小，输出越少
   * 3. 0级不输出
   * @param tag 模块名
   * @param level 等级
   */
  void Print(const std::string &tag, int level, const char *fmt, ...) {
    va_list ap;

    /* 模块控制 */
    auto it = access_.find(tag);
    if (it == access_.end()) {
      /* 新模块 */
      access_[tag] = kLog;
    }

    /* 权限控制 */
    if (level > access_[tag]) {
      return;
    }

    /* 打印 */
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
  }

 private:

  Log() = default;

 private:

  /* 模块 -> 打开等级(等级之上不输出) */
  std::unordered_map<std::string, int> access_;
};

}

#define ERR(module, fmt, ...)                                       \
  do {                                                              \
    Log::Instance().Print(module, Log::kError, fmt, ##__VA_ARGS__); \
    assert(0);                                                      \
  } while (0);

#define LOG(module, fmt, ...)                                   \
  Log::Instance().Print(module, Log::kLog, fmt, ##__VA_ARGS__)

#define DBG(module, fmt, ...)                                           \
  Log::Instance().Print(module, Log::kDebug,                            \
                        "[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#endif /* LOG_H */