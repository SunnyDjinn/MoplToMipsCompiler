%type <node> Program OptVarDec ListVarDec ListVarDec2 VarDec OptArrSize ListDecFuncs DecFunc ListParam OptListVarDec I IAffect IBlock ListI IIf OptElse IWhile ICall IReturn IWrite IEmpty Expression Expression2 Operator Operator2 Negation Comparison Comparison2 ArithExp ArithExp2 Term Term2 Factor Var OptIndex FuncCall ListExpr ListExpr2


%union value {
  int ival;
  astNode* node;
}
