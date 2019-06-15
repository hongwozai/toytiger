#include <utility>
#include <iostream>

#include "lexer.h"

using namespace std;
using namespace tiger;

void Lexer::WS()
{
  while(isspace(buf_.get()[bufp_])) {
    bufp_++;
    num_++;

    /* 换行，用于符号调试 */
    if (buf_.get()[bufp_] == '\n') {
      line_++;
      num_ = 0;
    }

    if (bufp_ >= buflen_) {
      if (!Reread()) {
        return;
      }
    }
  }
}

void Lexer::SetAllKeyword()
{
  /**
   * while for to break let in end function var type array
   * if then else do of nil
   */
  keyword_set_.insert("while");
  keyword_set_.insert("for");
  keyword_set_.insert("to");
  keyword_set_.insert("break");
  keyword_set_.insert("let");
  keyword_set_.insert("in");
  keyword_set_.insert("end");
  keyword_set_.insert("function");
  keyword_set_.insert("var");
  keyword_set_.insert("type");
  keyword_set_.insert("array");
  keyword_set_.insert("if");
  keyword_set_.insert("then");
  keyword_set_.insert("else");
  keyword_set_.insert("do");
  keyword_set_.insert("of");
  keyword_set_.insert("nil");
}

bool Lexer::IsKeyword(Token &token)
{
  return keyword_set_.find(token.GetValue()) != keyword_set_.end();
}

Token Lexer::Symbol()
{
  Token token(Token::kIdentifier, input_->InputSource(), line_, num_);
  int start = bufp_;

  while (true) {
    char c = buf_.get()[bufp_];

    if (!(isalnum(c) || c == '_')) {
      /* 单词 */
      if (bufp_ > start) {
        token.GetValue().append((char*)buf_.get() + start, bufp_ - start);
      }
      return std::move(token);
    }

    bufp_++;
    num_++;

    /* 补充 */
    if (bufp_ >= buflen_) {
      token.GetValue().append(buf_.get() + start, bufp_ - start);

      /* 到结尾了 */
      if (!Reread()) {
        return std::move(token);
      }
      start = bufp_;
    }
  }
}

Token Lexer::Number()
{
  Token token(Token::kNumber, input_->InputSource(), line_, num_);
  int start = bufp_;

  /* 这里暂时只处理正整数 */
  while (isdigit(buf_.get()[bufp_])) {
    bufp_++;
    num_++;

    if (bufp_ >= buflen_) {
      token.GetValue().append(buf_.get() + start, bufp_ - start);

      if (!Reread()) {
        return std::move(token);
      }
      start = bufp_;
    }
  }

  token.GetValue().append(buf_.get() + start, bufp_ - start);
  return std::move(token);
}

char Lexer::Trans(char c)
{
  switch (c) {
    case 'a': return '\a';
    case 'b': return '\b';
    case 'f': return '\f';
    case 'n': return '\n';
    case 'r': return '\r';
    case 't': return '\t';
    case 'v': return '\v';
    case '0': return '\0';
    default: return c;
  }
}

Token Lexer::String()
{
  Token token(Token::kString, input_->InputSource(), line_, num_);
  bool  state = false;
  string bufstr;
  int    bufstrp = 0;
  char   c;

  bufstr.reserve(kBufferSize);

  c = buf_.get()[bufp_];
  if (c != '"') {
    ERR(tag_, "Lexer String Usable Error");
  }

  do {
    bufp_++;
    num_++;

    if (bufp_ >= buflen_) {
      if (!Reread()) {
        throw Exception("String lack last '\"'",
                        Location(input_->InputSource(), line_, num_));
      }
    }

    c = buf_.get()[bufp_];
    if (state == false && c == '\\') {
      /* 进入转义状态 */
      state = true;
    } else if (state == true) {
      /* 转义状态下，读取转义的字符 */
      bufstr += Trans(c);
      state = false;
    } else if (state == false && c == '"') {
      /* 完结 */
      token.GetValue().append(bufstr);
      bufp_++;
      num_++;
      return std::move(token);
    } else {
      /* 正常无转义下的字符串 */
      bufstr += c;
    }
    /* 当缓存用完时，重新读取 */
    if (bufstr.size() >= bufstr.capacity()) {
      token.GetValue().append(bufstr);
      bufstr.clear();
    }
  } while (true);

  ERR(tag_, "Not Be Here");
  assert(0);
}

void Lexer::Comment()
{
  while (buf_.get()[bufp_] != '\n') {
    num_++;
    bufp_++;

    if (bufp_ >= buflen_) {
      if (!Reread()) {
        return;
      }
    }
  }
  num_ = 0;
  line_++;
}

bool Lexer::IsPunctuation(char c)
{
  return (c == ',' || c == ':' || c == ';' || c == '(' || c == ')' ||
          c == '[' || c == ']' || c == '{' || c == '}' || c == '.' ||
          c == '+' || c == '-' || c == '*' || c == '/' || c == '=' ||
          c == '&' || c == '|' || c == '<' || c == '>' || c == ':');
}

Token Lexer::Punctuation()
{
  char punc[2] = {0};
  Token token(Token::kPunctuation1,
              std::move(input_->InputSource()), line_, num_);

  punc[0] = buf_.get()[bufp_];

  bufp_++;
  num_++;

  if (punc[0] == '<' || punc[0] == '>' || punc[0] == ':') {
    /* 两字节符号标点 */
    if (bufp_ >= buflen_) {
      if (!Reread()) {
        token.GetValue().assign(punc, 1);
        return token;
      }
    }

    punc[1] = buf_.get()[bufp_];
    if (punc[1] == '=') {
      bufp_++;
      num_++;
      token.SetType(Token::kPunctuation2);
      token.GetValue().assign(punc, 2);
      return token;
    }
  }
  token.GetValue().assign(punc, 1);
  return token;
}

Token Lexer::Next()
{
  while (true) {
    /* 缓冲区没有数据，或者数据已经用完 */
    if (buflen_ == 0 || bufp_ >= buflen_) {
      if (!Reread()) {
        return Token(Token::kEof, std::move(input_->InputSource()), line_);
      }
      /* 重读之后没有eof，但是长度仍旧等于0的，属于input内部错误 */
      if (buflen_ == 0) {
        ERR(tag_, "InputStream have a problem!");
      }
    }

    /* 还有内容 */
    char c = buf_.get()[bufp_];
    if (isspace(c)) {
      /* 空白符 */
      WS();
    } else if (c == '_' || isalpha(c)) {
      /* 关键字与标识符 */
      Token token = Symbol();
      if (IsKeyword(token)) {
        token.SetType(Token::kKeyword);
      }
      return token;
    } else if (isdigit(c)) {
      /* 数字 */
      return Number();
    } else if (c == '"') {
      /* 字符串 */
      return String();
    } else if (c == '#') {
      /* 注释 */
      Comment();

    } else if (IsPunctuation(c)) {
      /* 标点符号 */
      return Punctuation();
    } else {
      throw Exception("Not Support Char",
                      Location(input_->InputSource(), line_, num_));
    }
  }
}

#if 1
int main(int argc, char *argv[])
{
  shared_ptr<InputStream> input = MakeStringInputStream("while 111 fuck  #asdfasd\nffff \"fuck\" = : := > >=");
  Lexer *lexer = new Lexer(input);

  while (true) {
    Token token = std::move(lexer->Next());

    cout << "type:" << token.GetType() << ","
         << "value:" << token.GetValue() << "," << endl;

    if (token.GetType() == Token::kEof) {
      break;
    }
  }
  return 0;
}
#endif