#include "TestConfig.h"
#include <algorithm>

class Test_Parser_RpnEvaluator : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//-----------------------------------------------------------------------------
TEST_F(Test_Parser_RpnEvaluator, Basic)
{
	ByteBuffer buffer("7");
	CppLexer lex;
	DiagnosticsItemSet diag;
	auto tokens = lex.Tokenize(buffer, &diag);
	auto rpnTokens = RPNParser::ParseCppConstExpression(tokens->cbegin(), tokens->cend(), &diag);
	RpnEvaluator eval;
	RpnOperand value;
	ASSERT_EQ(true, eval.TryEval(rpnTokens, &diag, &value));
	ASSERT_EQ(RpnOperandType::Double, value.type);
	ASSERT_EQ(7, value.valueDouble);


}


