#include "TestConfig.h"
#include "../src/Parser/Frontend/Cpp/Preprocessor.h"

class Test_Parser_Preprocessor : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}

	ByteBuffer m_buffer;
	CppLexer m_lex;
	Preprocessor m_prepro;
	std::shared_ptr<PreprocessedFileCacheItem> m_fileCache;
	DiagnosticsItemSet m_diag;
	TokenListPtr m_tokens;

	void Preprocess(const char* code)
	{
		bool r = TryPreprocess(code);
		if (!r) {
			LN_THROW(0, InvalidOperationException);
		}
	}

	bool TryPreprocess(const char* code)
	{
		m_diag.ClearItems();
		m_fileCache.reset(LN_NEW PreprocessedFileCacheItem());
		m_buffer = ByteBuffer(code);
		m_tokens = m_lex.Tokenize(m_buffer, &m_diag);
		return m_prepro.BuildPreprocessedTokenList(m_tokens, m_fileCache.get(), &m_diag) == ResultState::Success;
	}
};
/*
	defined()

	#defnie
	#error
	#endif
	#else
	#elif
	#include
	#ifndef
	#ifdef
	#line
	#pragma
	#undef
	#warning
	#
*/


//-----------------------------------------------------------------------------
TEST_F(Test_Parser_Preprocessor, Basic)
{
	{
		const char* code = "#define AAA";
		Preprocess(code);
	}
	{
		const char* code = "#define AAA 1";
		Preprocess(code);
	}
	{
		const char* code = "#define AAA 1 ";
		Preprocess(code);
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Parser_Preprocessor, Basic_if)
{
	// <Test> #if 1
	{
		const char* code =
			"#if 1\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(true, m_tokens->GetAt(5).IsValid());	// '1'
		ASSERT_EQ(true, m_tokens->GetAt(6).IsValid());	// '\n'
	}
	// <Test> #if 0
	{
		const char* code =
			"#if 0\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(false, m_tokens->GetAt(5).IsValid());	// '1'
		ASSERT_EQ(false, m_tokens->GetAt(6).IsValid());	// '\n'
	}
	// <Test> #if true
	{
		const char* code =
			"#if true\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(true, m_tokens->GetAt(5).IsValid());	// '1'
	}
	// <Test> #if false
	{
		const char* code =
			"#if false\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(false, m_tokens->GetAt(5).IsValid());	// '1'
	}
	// <Test> #if �� (���� 0 �ȊO)
	{
		const char* code =
			"#if 1*2-1\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(true, m_tokens->GetAt(9).IsValid());	// '1'
	}
	// <Test> #if �� (���� 0)
	{
		const char* code =
			"#if 1-1\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(false, m_tokens->GetAt(7).IsValid());	// '1'
	}


	// <Test> #if defined
	{
		const char* code =
			"#define AAA\n"
			"#if defined AAA\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(false, m_tokens->GetAt(7).IsValid());	// '1'
	}
	// <Test> #if defined
	{
		const char* code =
			"#if defined AAA\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(false, m_tokens->GetAt(7).IsValid());	// '1'
	}
	// <Test> #if defined()
	{
		const char* code =
			"#define AAA\n"
			"#if defined(AAA)\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(true, m_tokens->GetAt(13).IsValid());
	}
	// <Test> #if defined()
	{
		const char* code =
			"#if !defined(AAA)\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(true, m_tokens->GetAt(9).IsValid());
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Parser_Preprocessor, Basic_ifdef)
{
	// <Test> �����̈�
	{
		const char* code =
			"#define AAA\n"
			"#ifdef AAAB\n"
			"1\n"
			"#endif\n";
		Preprocess(code);

		// ���l 1 �͖����̈�
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, m_tokens->GetAt(10).GetCommonType());
		ASSERT_EQ(false, m_tokens->GetAt(10).IsValid());
	}
	// <Test> �L���̈�
	{
		const char* code =
			"#define AAA\n"
			"#ifdef AAA\n"
			"1\n"
			"#endif\n";
		Preprocess(code);

		// ���l 1 �͗L���̈�
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, m_tokens->GetAt(10).GetCommonType());
		ASSERT_EQ(true, m_tokens->GetAt(10).IsValid());
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Parser_Preprocessor, Basic_else)
{
	// <Test> �L�� �� ����
	{
		const char* code =
			"#define AAA\n"
			"#ifdef AAA\n"
			"1\n"
			"#else\n"
			"2\n"
			"#endif\n";
		Preprocess(code);

		// 1 �͗L���̈�
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, m_tokens->GetAt(10).GetCommonType());
		ASSERT_EQ(true, m_tokens->GetAt(10).IsValid());
		// 2 �͖����̈�
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, m_tokens->GetAt(15).GetCommonType());
		ASSERT_EQ(false, m_tokens->GetAt(15).IsValid());
	}
	// <Test> ���� �� �L��
	{
		const char* code =
			"#ifdef AAA\n"
			"1\n"
			"#else\n"
			"2\n"
			"#endif";
		Preprocess(code);

		// 1 �͖����̈�
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, m_tokens->GetAt(5).GetCommonType());
		ASSERT_EQ(false, m_tokens->GetAt(5).IsValid());
		// 2 �͗L���̈�
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, m_tokens->GetAt(10).GetCommonType());
		ASSERT_EQ(true, m_tokens->GetAt(10).IsValid());

	}
}

#define AAA 1-

#if AAA 1
#error AA
#endif



//-----------------------------------------------------------------------------
TEST_F(Test_Parser_Preprocessor, Illigal)
{
	// �ǂ����בւ��Ă���Ԃ������̂�
#define BBB2	AAA2
#define AAA2	100
	printf("%d\n", BBB2);
#undef AAA2
#define AAA2	200
	printf("%d\n", BBB2);

	// <Illigal> #if �̒萔��������
	{
		const char* code = "#if";
		ASSERT_EQ(false, TryPreprocess(code));
		ASSERT_EQ(DiagnosticsCode::Preprocessor_InvalidConstantExpression, m_diag.GetItems()->GetAt(0).GetCode());
	}
	// <Illigal> #if �̒萔���������ł͂Ȃ�
	{
		const char* code = "#if 1.5 + 2";
		ASSERT_EQ(false, TryPreprocess(code));
		ASSERT_EQ(DiagnosticsCode::Preprocessor_InvalidConstantExpression, m_diag.GetItems()->GetAt(0).GetCode());
	}
	// <Illigal> #if �̂܂��� #else ������
	{
		const char* code = "#else\n";
		ASSERT_EQ(false, TryPreprocess(code));
		ASSERT_EQ(DiagnosticsCode::Preprocessor_UnexpectedElse, m_diag.GetItems()->GetAt(0).GetCode());
	}
	// <Test> defined �̌�낪���ʎq�� ( �ȊO
	{
		const char* code = "#if defined + AAA\n";
		ASSERT_EQ(false, TryPreprocess(code));
		ASSERT_EQ(DiagnosticsCode::Preprocessor_ExpectedDefinedId, m_diag.GetItems()->GetAt(0).GetCode());
	}
	// <Illigal> #else ���A���ŗ���
	{
		const char* code =
			"#ifdef AAA\n"
			"#else\n"
			"#else\n";
		ASSERT_EQ(false, TryPreprocess(code));
		ASSERT_EQ(DiagnosticsCode::Preprocessor_UnexpectedElse, m_diag.GetItems()->GetAt(0).GetCode());
	}
	// <Illigal> #if �̂܂��� #endif ������
	{
		const char* code =
			"#endif";
		ASSERT_EQ(false, TryPreprocess(code));
		ASSERT_EQ(DiagnosticsCode::Preprocessor_UnexpectedEndif, m_diag.GetItems()->GetAt(0).GetCode());
	}
}
