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

	Parser::CppLexer<char>::TokenListT& tokens = lexer.GetTokenList();

}
