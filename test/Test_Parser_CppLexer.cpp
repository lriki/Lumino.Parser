#include "TestConfig.h"

class Test_Parser_CppLexer : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//-----------------------------------------------------------------------------
TEST_F(Test_Parser_CppLexer, Basic)
{
	DiagnosticsItemSet diag;
	{
		CppLexer lex;
		auto tokens = lex.Tokenize(" ", &diag);
		ASSERT_EQ(2, tokens->GetCount());
		ASSERT_EQ(CommonTokenType::SpaceSequence, tokens->GetAt(0).GetCommonType());
		ASSERT_EQ(CommonTokenType::Eof, tokens->GetAt(1).GetCommonType());
	}
	// <Test> ���s
	{
		CppLexer lex;
		auto tokens = lex.Tokenize("\n\r\r\n", &diag);
		ASSERT_EQ(4, tokens->GetCount());
		ASSERT_EQ(CommonTokenType::NewLine, tokens->GetAt(0).GetCommonType());
		ASSERT_EQ(CommonTokenType::NewLine, tokens->GetAt(1).GetCommonType());
		ASSERT_EQ(CommonTokenType::NewLine, tokens->GetAt(2).GetCommonType());
	}
	{
		CppLexer lex;
		auto tokens = lex.Tokenize("5 0x5 5.0 5.", &diag);
		ASSERT_EQ(8, tokens->GetCount());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(0).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Int32, tokens->GetAt(0).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(2).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Int32, tokens->GetAt(2).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(4).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Double, tokens->GetAt(4).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(6).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Double, tokens->GetAt(6).GetLangTokenType());
	}
	// <Test> �����T�t�B�b�N�X�̊m�F
	{
		CppLexer lex;
		auto tokens = lex.Tokenize("5u 5U    5l 5L    5ul 5Ul 5uL 5UL    5ll 5LL    5ull 5Ull 5uLL 5uLL", &diag);
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(0).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_UInt32, tokens->GetAt(0).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(2).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_UInt32, tokens->GetAt(2).GetLangTokenType());

		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(4).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Int32, tokens->GetAt(4).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(6).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Int32, tokens->GetAt(6).GetLangTokenType());

		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(8).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_UInt32, tokens->GetAt(8).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(10).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_UInt32, tokens->GetAt(10).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(12).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_UInt32, tokens->GetAt(12).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(14).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_UInt32, tokens->GetAt(14).GetLangTokenType());

		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(16).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Int64, tokens->GetAt(16).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(18).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Int64, tokens->GetAt(18).GetLangTokenType());
		
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(20).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_UInt64, tokens->GetAt(20).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(22).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_UInt64, tokens->GetAt(22).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(24).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_UInt64, tokens->GetAt(24).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(26).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_UInt64, tokens->GetAt(26).GetLangTokenType());
	}
	// <Test> �����T�t�B�b�N�X�̊m�F
	{
		CppLexer lex;
		auto tokens = lex.Tokenize("5f 5.F 5d 5.D", &diag);
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(0).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Float, tokens->GetAt(0).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(2).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Float, tokens->GetAt(2).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(4).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Double, tokens->GetAt(4).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(6).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Double, tokens->GetAt(6).GetLangTokenType());
	}
	// <Test> �w���\�L
	{
		CppLexer lex;
		auto tokens = lex.Tokenize("5e+03 5e+03F 5.e+03 5.0e+03", &diag);
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(0).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Double, tokens->GetAt(0).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(2).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Float, tokens->GetAt(2).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(4).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Double, tokens->GetAt(4).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(6).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Double, tokens->GetAt(6).GetLangTokenType());
	}
	// <Test> �������e����
	{
		CppLexer lex;
		auto tokens = lex.Tokenize("'a' '\\'' L'\\n'", &diag);
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(0).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Char, tokens->GetAt(0).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(2).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_Char, tokens->GetAt(2).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, tokens->GetAt(4).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_WideChar, tokens->GetAt(4).GetLangTokenType());
	}

	// <Test> �����񃊃e����
	{
		CppLexer lex;
		auto tokens = lex.Tokenize("\"a\" \"\\\"\" L\"\"", &diag);
		ASSERT_EQ(CommonTokenType::StringLiteral, tokens->GetAt(0).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_AsciiString, tokens->GetAt(0).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::StringLiteral, tokens->GetAt(2).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_AsciiString, tokens->GetAt(2).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::StringLiteral, tokens->GetAt(4).GetCommonType()); ASSERT_EQ(TT_NumericLitaralType_WideString, tokens->GetAt(4).GetLangTokenType());
	}
	// <Test> �u���b�N�R�����g
	{
		CppLexer lex;
		auto tokens = lex.Tokenize("/*a*//**/", &diag);
		ASSERT_EQ(CommonTokenType::Comment, tokens->GetAt(0).GetCommonType());
		ASSERT_EQ(CommonTokenType::Comment, tokens->GetAt(1).GetCommonType());
		ASSERT_EQ(CommonTokenType::Eof, tokens->GetAt(2).GetCommonType());
	}
	// <Illigal> �u���b�N�R�����g�̓r����EOF
	{
		CppLexer lex;
		auto tokens = lex.Tokenize("/*", &diag);
		ASSERT_EQ(DiagnosticsCode::UnexpectedEOFInBlockComment, diag.GetItems()->GetLast().GetCode());
	}
	// <Test> �s�R�����g
	{
		CppLexer lex;
		auto tokens = lex.Tokenize("//\r\n//a\\\na", &diag);
		ASSERT_EQ(CommonTokenType::Comment, tokens->GetAt(0).GetCommonType());
		ASSERT_EQ(CommonTokenType::NewLine, tokens->GetAt(1).GetCommonType());	// \r\n
		ASSERT_EQ(CommonTokenType::Comment, tokens->GetAt(2).GetCommonType());	// \ ���s�̏I�[�ɂ���̂łЂƑ���
		ASSERT_EQ(CommonTokenType::Eof, tokens->GetAt(3).GetCommonType());
	}
	// <Test> ���Z�q
	{
		CppLexer lex;
		auto tokens = lex.Tokenize("/ /+++++", &diag);
		ASSERT_EQ(CommonTokenType::Operator, tokens->GetAt(0).GetCommonType());
		ASSERT_EQ(CommonTokenType::SpaceSequence, tokens->GetAt(1).GetCommonType());
		ASSERT_EQ(CommonTokenType::Operator, tokens->GetAt(2).GetCommonType());
		ASSERT_EQ(CommonTokenType::Operator, tokens->GetAt(3).GetCommonType());
		ASSERT_EQ(CommonTokenType::Operator, tokens->GetAt(4).GetCommonType());
		ASSERT_EQ(CommonTokenType::Operator, tokens->GetAt(5).GetCommonType());
		ASSERT_EQ(CommonTokenType::Eof, tokens->GetAt(6).GetCommonType());
	}
	// <Test> �s���G�X�P�[�v
	{
		CppLexer lex;
		auto tokens = lex.Tokenize("\\\n", &diag);
		ASSERT_EQ(CommonTokenType::Unknown, tokens->GetAt(0).GetCommonType()); ASSERT_EQ(TT_EscapeNewLine, tokens->GetAt(0).GetLangTokenType());
		ASSERT_EQ(CommonTokenType::Eof, tokens->GetAt(1).GetCommonType());
	}
	// �}���`�o�C�g��������
	{
		byte_t buf[] = { 0xE6, 0x95, 0xB0, 0x20, 0xE6, 0x95, 0xB0, 0x00 };
		CppLexer lex;
		auto tokens = lex.Tokenize((const char*)buf, &diag);
		ASSERT_EQ(CommonTokenType::MbsSequence, tokens->GetAt(0).GetCommonType());
		ASSERT_EQ(CommonTokenType::SpaceSequence, tokens->GetAt(1).GetCommonType());
		ASSERT_EQ(CommonTokenType::MbsSequence, tokens->GetAt(2).GetCommonType());
		ASSERT_EQ(CommonTokenType::Eof, tokens->GetAt(3).GetCommonType());
	}

	{
		//ByteBuffer buf("int a = 0;");
		//CppLexer lex;
		//auto tokens = lex.Tokenize(buf);
		//ASSERT_EQ(8, tokens->GetCount());
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Parser_CppLexer, Illigal)
{
	DiagnosticsItemSet diag;
	//// <Test> �����Ȑ��l���e�����T�t�B�b�N�X
	//{
	//	CppLexer lex;
	//	auto tokens = lex.Tokenize("5a", &diag);
	//}


}
