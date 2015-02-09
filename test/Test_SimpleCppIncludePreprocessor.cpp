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
	Parser::SimpleCppIncludePreprocessor<char> preprocessor;
	preprocessor.Analyze(&lexer.GetTokenList(), setting, &err);

	FileStream file(_T("test.txt"), FileMode_Create, FileAccess_Write);
	lexer.GetTokenList().DumpText(&file);




	/*	TODO
			�E���AToken �� begin end �͌��̃o�b�t�@���Q�Ƃ��Ă��邾���B
				������^�C�~���O�Ő؂藣���ēƎ��������Ɏ����Ȃ��ƁAinclude �t�@�C�������ł��Ȃ��B
			�ETokenList �N���X����Ă܂Ƃ߂����������C������B
	 */

}
