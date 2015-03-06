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
// コレはダメ。「関数呼び出しはダメ」エラーになる。
//#if sizeof(wchar_t) == 2
//	printf("test\n");
//#endif
// defined は単項演算子扱い
#if defined AAA && defined(BBB)
	printf("test\n");
#endif
#define DDD(x)x		// 空白は必要ない
	printf("%d\n", DDD(1));


#define FUNC_MACRO(a, b) a b

	FUNC_MACRO({ int a = 1; , printf("a:%d\n", a); });	// "a:1" が出力される
	//a = 10;	// スコープの外。
	// ↑ 6.10.3 マクロ置き換えによると、() のネストだけチェックすればよさそう

//#define AAA2(x,,z)	// 省略不可

//#define EMPTY
//EMPTY #include "test.h"	// この EMPTY は置き換えられないのでエラーになる

#if 'A' == 0x41
	printf("A\n");
#endif

#if (0), (1)		// , 演算子は使えない
	printf("A\n");
#endif

}

#define TEST_FILE <time.h>
#include TEST_FILE			// これはOK
