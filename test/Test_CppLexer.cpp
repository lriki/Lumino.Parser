#include "TestConfig.h"
#include <algorithm>
using namespace Lumino;

class Test_CppLexer : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//-----------------------------------------------------------------------------
TEST_F(Test_CppLexer, Basic)
{
	const char* s1 = "#include <hoge.h>";//"float a = 1.0f;";
	RefBuffer buffer;
	buffer.Reserve((const byte_t*)s1, strlen(s1));

	Parser::ErrorManager err;
	Parser::CppLexer<char> lexer;
	lexer.Analyze(&buffer, &err);

	Parser::CppLexer<char>::TokenListT* tokens = lexer.GetTokenList();

}

//-----------------------------------------------------------------------------
TEST_F(Test_CppLexer, CharOrStringLiteral)
{
	// L プレフィックス付き
	const TCHAR* s1 = _T("L\"a\"");
	RefBuffer buffer(s1, StringUtils::StrLen(s1) * sizeof(TCHAR));

	Parser::ErrorManager err;
	Parser::CppLexer<TCHAR> lexer;
	lexer.Analyze(&buffer, &err);
	Parser::CppLexer<TCHAR>::TokenListT* tokens = lexer.GetTokenList();

	ASSERT_EQ(Parser::TokenType_StringLiteral, tokens->GetAt(0).GetTokenType());
	ASSERT_EQ(Parser::TokenType_EOF, tokens->GetAt(1).GetTokenType());
}

//-----------------------------------------------------------------------------
TEST_F(Test_CppLexer, CppTokenType)
{
	// ※x+++++y is parsed as x ++ ++ + y,
	const TCHAR* s1 = _T("+++++");
	RefBuffer buffer(s1, StringUtils::StrLen(s1) * sizeof(TCHAR));

	Parser::ErrorManager err;
	Parser::CppLexer<TCHAR> lexer;
	lexer.Analyze(&buffer, &err);

	Parser::CppLexer<TCHAR>::TokenListT* tokens = lexer.GetTokenList();

	ASSERT_EQ(Parser::TT_CppOP_Increment, tokens->GetAt(0).GetLangTokenType());
	ASSERT_EQ(Parser::TT_CppOP_Increment, tokens->GetAt(1).GetLangTokenType());
	ASSERT_EQ(Parser::TT_CppOP_Plus, tokens->GetAt(2).GetLangTokenType());
	ASSERT_EQ(Parser::TokenType_EOF, tokens->GetAt(3).GetTokenType());


}
