#include "TestConfig.h"
#include <algorithm>
#include "../include/Lumino/Parser/Cpp/CppPreprocessor.h"
using namespace Lumino;

class Test_CppPreprocessor : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//-----------------------------------------------------------------------------
TEST_F(Test_CppPreprocessor, Basic)
{

	const TCHAR* s1 = _T("#define AAA(x, ...) { x; __VA_ARGS__; }");
	RefBuffer buffer(s1, StringUtils::StrLen(s1) * sizeof(TCHAR));

	Parser::ErrorManager err;
	Parser::CppLexer<TCHAR> lexer;
	lexer.Analyze(&buffer, &err);

	Parser::CppLexer<TCHAR>::TokenListT* tokens = lexer.GetTokenList();
	Parser::CppPreprocessor<TCHAR> cppPrepro;
	cppPrepro.Analyze(*tokens, &err);



#define AAA 1
#define BBB 0
#define CCC 3
#if (AAA) ? BBB : CCC
	printf("test\n");
#endif
// �R���̓_���B�u�֐��Ăяo���̓_���v�G���[�ɂȂ�B
//#if sizeof(wchar_t) == 2
//	printf("test\n");
//#endif
// defined �͒P�����Z�q����
#if defined AAA && defined(BBB)
	printf("test\n");
#endif
#define DDD(x)x		// �󔒂͕K�v�Ȃ�
	printf("%d\n", DDD(1));


#define FUNC_MACRO(a, b) a b

	FUNC_MACRO({ int a = 1; , printf("a:%d\n", a); });	// "a:1" ���o�͂����
	//a = 10;	// �X�R�[�v�̊O�B
	// �� 6.10.3 �}�N���u�������ɂ��ƁA() �̃l�X�g�����`�F�b�N����΂悳����

//#define AAA2(x,,z)	// �ȗ��s��

//#define EMPTY
//EMPTY #include "test.h"	// ���� EMPTY �͒u���������Ȃ��̂ŃG���[�ɂȂ�

#if 'A' == 0x41
	printf("A\n");
#endif

#if (0), (1)		// , ���Z�q�͎g���Ȃ�
	printf("A\n");
#endif

}

#define TEST_FILE <time.h>
#include TEST_FILE			// �����OK
