#include "TestConfig.h"
#include <algorithm>

class Test_Parser_RpnEvaluator : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}



	ByteBuffer m_buffer;
	CppLexer m_lex;
	DiagnosticsItemSet m_diag;
	RpnEvaluator m_eval;
	RpnOperand m_value;
	void Eval(const char* exp)
	{
		m_buffer = ByteBuffer(exp);

		auto tokens = m_lex.Tokenize(m_buffer, &m_diag);
		auto rpnTokens = RPNParser::ParseCppConstExpression(tokens->cbegin(), tokens->cend(), &m_diag);

		bool r = m_eval.TryEval(rpnTokens, &m_diag, &m_value);
		if (!r || !m_diag.GetItems()->IsEmpty()) {
			LN_THROW(0, InvalidOperationException);
		}
	}
};

//-----------------------------------------------------------------------------
TEST_F(Test_Parser_RpnEvaluator, Basic)
{
	// <Test> ƒŠƒeƒ‰ƒ‹1‚Â‚¾‚¯
	{
		Eval("true");
		ASSERT_EQ(RpnOperandType::Boolean, m_value.type);
		ASSERT_EQ(true, m_value.valueBoolean);
		Eval("false");
		ASSERT_EQ(RpnOperandType::Boolean, m_value.type);
		ASSERT_EQ(false, m_value.valueBoolean);
		Eval("7");
		ASSERT_EQ(RpnOperandType::Int32, m_value.type);
		ASSERT_EQ(7, m_value.valueInt32);
		Eval("7u");
		ASSERT_EQ(RpnOperandType::UInt32, m_value.type);
		ASSERT_EQ(7, m_value.valueUInt32);
		Eval("7ll");
		ASSERT_EQ(RpnOperandType::Int64, m_value.type);
		ASSERT_EQ(7, m_value.valueInt64);
		Eval("7ull");
		ASSERT_EQ(RpnOperandType::UInt64, m_value.type);
		ASSERT_EQ(7, m_value.valueUInt64);
		Eval("7.f");
		ASSERT_EQ(RpnOperandType::Float, m_value.type);
		ASSERT_EQ(7, m_value.valueFloat);
		Eval("7.0");
		ASSERT_EQ(RpnOperandType::Double, m_value.type);
		ASSERT_EQ(7, m_value.valueDouble);
	}
	// <Test> + (Unary)
	{
		Eval("+2");
		ASSERT_EQ(2, m_value.valueInt32);
	}
	// <Test> - (Unary)
	{
		Eval("-2");
		ASSERT_EQ(-2, m_value.valueInt32);
	}
}


