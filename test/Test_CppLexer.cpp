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


	std::vector< std::string > vv;
	vv.push_back("L");
	vv.push_back("U");
	vv.push_back("M");
	vv.push_back("I");
	vv.push_back("N");
	vv.push_back("O");
	vv.push_back("U");
	vv.push_back("S");
	srand(time(0));
	for (int i = 0; i < 20; i++)
	{
		std::random_shuffle(vv.begin(), vv.end());
		LN_FOREACH(std::string& s, vv)
		{
			std::cout << s;
		}
		std::cout << std::endl;
	}



	const char* s1 = "#include <hoge.h>";//"float a = 1.0f;";
	RefBuffer buffer;
	buffer.Reserve((const byte_t*)s1, strlen(s1));

	Parser::ErrorManager err;
	Parser::CppLexer<char> lexer;
	lexer.Analyze(&buffer, &err);

	Parser::CppLexer<char>::TokenList& tokens = lexer.GetTokenList();

}
