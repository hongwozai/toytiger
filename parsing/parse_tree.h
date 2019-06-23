#ifndef PARSE_TREE_H
#define PARSE_TREE_H

#include <list>

#include "lexer.h"

namespace tiger {

class ParseNode;

class ParseTreeListener {
 public:

  virtual void Visit(ParseNode *node) = 0;
};

/**
 * 规范型异构树
 */
class ParseNode {
 public:

  ParseNode() = default;

  virtual ~ParseNode() = 0;

  /**
   * 判断是否为
   */
  bool IsFinal() {
    return child_list_.empty();
  }

  void Visit(ParseTreeListener *vistor) {
    visitor->Visit(this);
  }

  /* 打印 */
  void Print();

  /* 添加孩子 */
  void AddChild(ParseNode *node) {
    child_list_.push_back(node);
  }

 private:

  /* 子结点 */
  std::list<ParseNode*> child_list_;
};


class VarDec : public ParseNode {
 public:

  virtual ~VarDec() override {}
};

class FuncDec : public ParseNode {
 public:
  virtual ~FuncDec() override {}
};

class TypeDec : public ParseNode {
 public:
  virtual ~TypeDec() override {}
};

class IfExpr : public ParseNode {
 public:
  virtual ~IfExpr() override {}
};

class WhileExpr : public ParseNode {
 public:
  virtual ~WhileExpr() override {}
};

class TerminalNode : public ParseNode {
 public:
  virtual ~TerminalNode() override {}
};

}

#endif /* PARSE_TREE_H */