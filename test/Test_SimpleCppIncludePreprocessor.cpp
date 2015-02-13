#include "TestConfig.h"
#include "../include/Lumino/Parser/Tools/SimpleCppIncludePreprocessor.h"
using namespace Lumino;

class Test_SimpleCppIncludePreprocessor : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//-----------------------------------------------------------------------------
TEST_F(Test_SimpleCppIncludePreprocessor, Basic)
{
	RefPtr<RefBuffer> buffer(
		FileUtils::ReadAllBytes(LN_TEST_GET_FILE_PATH("TestData/SimpleCppIncludePreprocessor.txt")));

	PathName curDir(LN_TEST_GET_FILE_PATH("TestData"));

	Parser::ErrorManager err;
	Parser::CppLexer<char> lexer;
	lexer.Analyze(buffer, &err);

	Parser::SimpleCppIncludePreprocessor<char>::SettingData setting;
	setting.CurrentDirectory = curDir;
	setting.ErrorManager = &err;
	Parser::SimpleCppIncludePreprocessor<char> preprocessor;
	preprocessor.Analyze(lexer.GetTokenList(), setting);

	MemoryStream buf;
	//FileStream buf(_T("test.txt"), FileMode_Create, FileAccess_Write);
	lexer.GetTokenList()->DumpText(&buf);

	ASSERT_TRUE(TestUtils::CheckArrays(buf.GetBuffer(), "int a;\nint c;\nint b;\nint d;", 27));
}

//-----------------------------------------------------------------------------
// 行頭や # の後にスペースがあるパターン
TEST_F(Test_SimpleCppIncludePreprocessor, LineHeadSpace)
{
	StringA text = "  #  include   \"SimpleCppIncludePreprocessor.txt\" ";

	Parser::ErrorManager err;
	Parser::SimpleCppIncludePreprocessor<char>::SettingData settings;
	settings.CurrentDirectory = PathName(LN_TEST_GET_FILE_PATH("TestData"));
	settings.ErrorManager = &err;
	StringA result = Parser::SimpleCppIncludePreprocessor<char>::AnalyzeStringToString(text, settings);

	ASSERT_STREQ("int a;\nint c;\nint b;\nint d;", result.GetCStr());




	{

		RefPtr<RefBuffer> code(
			FileUtils::ReadAllBytes(_T("C:/Proj/DynaVision/DT10_ResDivSln/DYNAVISION_LIB/libhld/include/HLD.h")));

		Parser::ErrorManager err;

		// lex
		Parser::CppLexer<char> lexer;
		lexer.Analyze(code, &err);

		Parser::SimpleCppIncludePreprocessor<char>::SettingData settings;
		settings.CurrentDirectory = PathName(LN_TEST_GET_FILE_PATH("TestData"));
		settings.ErrorManager = &err;
		Parser::SimpleCppIncludePreprocessor<char> prepro;
		prepro.Analyze(lexer.GetTokenList(), settings);

		FileStream stream(_T("hld.h"), FileMode_Create, FileAccess_Write);
		lexer.GetTokenList()->DumpText(&stream);

	}

}
