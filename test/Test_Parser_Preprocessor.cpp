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
	PreprocessedFileCacheItem m_fileCache;
	DiagnosticsItemSet m_diag;

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
		m_buffer = ByteBuffer(code);
		auto tokens = m_lex.Tokenize(m_buffer, &m_diag);
		return m_prepro.BuildPreprocessedTokenList(tokens, &m_fileCache, &m_diag) == ResultState::Success;
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
TEST_F(Test_Parser_Preprocessor, Illigal)
{
}
