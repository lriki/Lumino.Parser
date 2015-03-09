#include "TestConfig.h"
#include <algorithm>
#include "../include/Lumino/Parser/RPN/RPNParser.h"
using namespace Lumino;
using namespace Lumino::Parser;

class Test_RPNParser : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

typedef RefPtr< Parser::TokenList<TCHAR> >				TokenListPtr;
typedef RefPtr< ArrayList< Parser::RPNToken<TCHAR> > >	RPNTokenListPtr;

//-----------------------------------------------------------------------------
TEST_F(Test_RPNParser, Parse)
{
	//int a = (1 == 2 ? 3 + 4 : 5 + 6);					// 11
	//int b = (0 ?     1 ? 1 : 2     :     1 ? 3 : 4);	// 3

	// •’Ê‚É
	{
		ByteBuffer buf1(_T("1 + 2"));
		ErrorManager err;
		TokenListPtr tokens(CppLexer<TCHAR>::Lex(&buf1, &err));
		RPNTokenListPtr rpnTokens(RPNParser<TCHAR>::ParseCppConstExpression(tokens->begin(), tokens->end(), &err));

		ASSERT_EQ(3, rpnTokens->GetCount());
		ASSERT_EQ(RPN_TT_NumericLiteral, rpnTokens->GetAt(0).Type);
		ASSERT_EQ(RPN_TT_NumericLiteral, rpnTokens->GetAt(1).Type);
		ASSERT_EQ(RPN_TT_OP_BinaryPlus, rpnTokens->GetAt(2).Type);
	}

	// —Dæ‡˜
	{
		ByteBuffer buf(_T("1 + 2 * 3"));
		ErrorManager err;
		TokenListPtr tokens(CppLexer<TCHAR>::Lex(&buf, &err));
		RPNTokenListPtr rpnTokens(RPNParser<TCHAR>::ParseCppConstExpression(tokens->begin(), tokens->end(), &err));

		ASSERT_EQ(5, rpnTokens->GetCount());
		ASSERT_EQ(RPN_TT_NumericLiteral, rpnTokens->GetAt(0).Type);
		ASSERT_EQ(RPN_TT_NumericLiteral, rpnTokens->GetAt(1).Type);
		ASSERT_EQ(RPN_TT_NumericLiteral, rpnTokens->GetAt(2).Type);
		ASSERT_EQ(RPN_TT_OP_Multiply, rpnTokens->GetAt(3).Type);
		ASSERT_EQ(RPN_TT_OP_BinaryPlus, rpnTokens->GetAt(4).Type);
	}

	// —Dæ‡˜
	{
		ByteBuffer buf(_T("1 * 2 + 3"));
		ErrorManager err;
		TokenListPtr tokens(CppLexer<TCHAR>::Lex(&buf, &err));
		RPNTokenListPtr rpnTokens(RPNParser<TCHAR>::ParseCppConstExpression(tokens->begin(), tokens->end(), &err));

		ASSERT_EQ(5, rpnTokens->GetCount());
		ASSERT_EQ(RPN_TT_NumericLiteral, rpnTokens->GetAt(0).Type);
		ASSERT_EQ(RPN_TT_NumericLiteral, rpnTokens->GetAt(1).Type);
		ASSERT_EQ(RPN_TT_OP_Multiply, rpnTokens->GetAt(2).Type);
		ASSERT_EQ(RPN_TT_NumericLiteral, rpnTokens->GetAt(3).Type);
		ASSERT_EQ(RPN_TT_OP_BinaryPlus, rpnTokens->GetAt(4).Type);
	}

	// Š‡ŒÊ
	{
		ByteBuffer buf(_T("1 * (2 + 3)"));
		ErrorManager err;
		TokenListPtr tokens(CppLexer<TCHAR>::Lex(&buf, &err));
		RPNTokenListPtr rpnTokens(RPNParser<TCHAR>::ParseCppConstExpression(tokens->begin(), tokens->end(), &err));

		ASSERT_EQ(5, rpnTokens->GetCount());
		ASSERT_EQ(RPN_TT_NumericLiteral, rpnTokens->GetAt(0).Type);
		ASSERT_EQ(RPN_TT_NumericLiteral, rpnTokens->GetAt(1).Type);
		ASSERT_EQ(RPN_TT_NumericLiteral, rpnTokens->GetAt(2).Type);
		ASSERT_EQ(RPN_TT_OP_BinaryPlus, rpnTokens->GetAt(3).Type);
		ASSERT_EQ(RPN_TT_OP_Multiply, rpnTokens->GetAt(4).Type);
	}


	// ’P€‰‰Zq

	// ğŒ‰‰Zq
	//{
	//	ByteBuffer buf(_T("1 + 2 ? 3 + 4 : 5 + 6"));
	//	ErrorManager err;
	//	TokenListPtr tokens(CppLexer<TCHAR>::Lex(&buf, &err));
	//	RPNTokenListPtr rpnTokens(RPNParser<TCHAR>::ParseCppConstExpression(tokens->begin(), tokens->end(), &err));
	//	ASSERT_EQ(11, rpnTokens->GetCount());

	//	// 1+1 ?   2 ? 1: 2  : 3
	// 1 ? 2 : (3 ? 4 : (5 ? 6 : 7))
	// 1 ? 6 ? 7 : 8 : 3 ? 4 : 5





	//}
}
