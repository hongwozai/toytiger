#ifndef LEXER_H
#define LEXER_H

#include <set>

#include "input_stream.h"
#include "utils/log.h"
#include "utils/exception.h"

namespace tiger {

class Token {
 public:

  /* 类型 */
  enum Type {
    /* 关键字 */
    kKeyword = 1,
    /* 标识符 */
    kIdentifier,
    /* 注释 */
    kComment,
    /* 整型字面量 */
    kNumber,
    /* 字符串 */
    kString,
    /* 一个字节标点符号 */
    kPunctuation1,
    /* 两个字节标点符号 */
    kPunctuation2,
    /* 结束符 */
    kEof,
  };

 public:

  Token(Type type_, const std::string &file, int line)
      : type_(type_),
        pos_(file, line) {}

  Token(Type type_, const std::string &file, int line, int num)
      : type_(type_),
        pos_(file, line, num) {}

  Token(Token &&token)
      : type_(token.type_),
        value_(std::move(token.value_)),
        pos_(std::move(token.pos_)) {}

  void SetType(Type type) { type_ = type; }

  const Type &GetType() const noexcept { return type_; }

  std::string &GetValue() noexcept { return value_; }

  const Location &GetPos() const noexcept { return pos_; }

 private:

  Type type_;

  std::string value_;

  /* debug */
  Location pos_;
};

class Lexer {
 public:

  enum { kBufferSize = 512 };

public:

  Lexer(std::shared_ptr<InputStream> &input)
      : tag_("Lexer"),
        input_(input),
        buf_(new char[kBufferSize]),
        buflen_(0),
        bufp_(0) {
    Log::Instance().Set(tag_, Log::kDebug);
    SetAllKeyword();
  }

  ~Lexer() {}

  /**
   * 结束的时候，会返回EofToken
   */
  Token Next();

 private:

  void SetAllKeyword();

  bool IsKeyword(Token &token);

  /* 跳过空白符 */
  void WS();

  /* 数字 */
  Token Number();

  /* 字符串 */
  Token String();

  /* 字符串中转换 */
  char  Trans(char c);

  /* 是否是标点的第一个字符 */
  bool IsPunctuation(char c);

  /* 标点 */
  Token Punctuation();

  /* 注释 */
  void Comment();

  /* 符号：是否是关键字，这里不做区分 */
  Token Symbol();

  /* 重新填满缓存，并重置指针 */
  bool Reread() {
    return input_->Read(buf_.get(), kBufferSize, buflen_);
  }

 private:

  /* 用于日志输出 */
  const std::string tag_;

  /* 字符流 */
  std::shared_ptr<InputStream> input_;

  /* 字符缓存，以及缓存总长度 */
  std::unique_ptr<char> buf_;
  int buflen_;

  /* 缓存使用指针 */
  int bufp_;

  /* 总字节数，用于debug */
  int num_;
  int line_;

  /* 用于关键字识别 */
  std::set<std::string> keyword_set_;
};

}

#endif /* LEXER_H */