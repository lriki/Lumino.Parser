#include "TestConfig.h"
#include <algorithm>
#include "../src/Parser/Cpp/CppParserCoreBase.h"
using namespace Lumino;

class Test_CppParser : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};


class MyParser : public CppParserCoreBase
{
public:
	Parser::CppLexer<TCHAR>::TokenListT* m_tokens;
	Parser::CppLexer<TCHAR>::TokenListT::iterator m_pos;

	virtual void yyerror(char * msg)
	{
		std::cout << msg;
	}


	virtual int yylex()
	{
		while (m_pos->IsGenericSpace())
		{
			++m_pos;
		}

		switch (m_pos->GetLangTokenType())
		{
		case Parser::TT_CppOP_Plus:
			++m_pos;
			return PLUS;
		case Parser::TT_CppOP_Equal:
			++m_pos;
			return EQUALS;
		}

		switch (m_pos->GetTokenType())
		{
		case Parser::TokenType_NumericLiteral:
			yylval.i_type = 10;
			++m_pos;
			return NUMBER;
		case Parser::TokenType_EOF:
			return 0;
		}

		
		return UNKNOWN;
	}
};


//-----------------------------------------------------------------------------
TEST_F(Test_CppParser, Basic)
{
	const TCHAR* s1 = _T("10 + 20 + 10 =");
	RefBuffer buffer(s1, StringUtils::StrLen(s1) * sizeof(TCHAR));

	Parser::ErrorManager err;
	Parser::CppLexer<TCHAR> lexer;
	lexer.Analyze(&buffer, &err);
	Parser::CppLexer<TCHAR>::TokenListT* tokens = lexer.GetTokenList();


	MyParser parser;
	parser.m_tokens = tokens;
	parser.m_pos = tokens->begin();
	int result = parser.yyparse();	// http://www.bookshelf.jp/texi/bison/bison-ja_7.html#SEC60

	printf("");
}
