
#pragma once

#include "Lexer.h"

namespace Lumino
{
namespace Parser
{

/// (bison �Ɏ�ʂƂ��ē`���邽�߁A�ׂ����萔��������)
enum CppTokenType
{
	TT_Cpp_Unknown = 0,

	/* 1�`255 �� ASCII �����Ƃ��� (bison �ŕ������e���� '*' �����g����悤�ɂȂ�) */

	/* �ȉ��͑S�� bison ��`�t�@�C������ %token �Ƃ��Ă���`���� */

	TT_CppKW_asm = 256,
	TT_CppKW_auto,
	TT_CppKW_bool,
	TT_CppKW_break,
	TT_CppKW_case,
	TT_CppKW_catch,
	TT_CppKW_char,
	TT_CppKW_class,
	TT_CppKW_const,
	TT_CppKW_const_cast,
	TT_CppKW_continue,
	TT_CppKW_default,
	TT_CppKW_delete,
	TT_CppKW_do,
	TT_CppKW_double,
	TT_CppKW_dynamic_cast,
	TT_CppKW_else,
	TT_CppKW_enum,
	TT_CppKW_explicit,
	TT_CppKW_export,
	TT_CppKW_extern,
	TT_CppKW_false,
	TT_CppKW_float,
	TT_CppKW_for,
	TT_CppKW_friend,
	TT_CppKW_goto,
	TT_CppKW_if,
	TT_CppKW_inline,
	TT_CppKW_int,
	TT_CppKW_long,
	TT_CppKW_mutable,
	TT_CppKW_namespace,
	TT_CppKW_new,
	TT_CppKW_operator,
	TT_CppKW_private,
	TT_CppKW_protected,
	TT_CppKW_public,
	TT_CppKW_register,
	TT_CppKW_reinterpret_cast,
	TT_CppKW_return,
	TT_CppKW_short,
	TT_CppKW_signed,
	TT_CppKW_sizeof,
	TT_CppKW_static,
	TT_CppKW_static_cast,
	TT_CppKW_struct,
	TT_CppKW_switch,
	TT_CppKW_template,
	TT_CppKW_this,
	TT_CppKW_throw,
	TT_CppKW_true,
	TT_CppKW_try,
	TT_CppKW_typedef,
	TT_CppKW_typeid,
	TT_CppKW_typename,
	TT_CppKW_union,
	TT_CppKW_unsigned,
	TT_CppKW_using,
	TT_CppKW_virtual,
	TT_CppKW_void,
	TT_CppKW_volatile,
	TT_CppKW_wchar_t,
	TT_CppKW_while,

	TT_CppOP_SeparatorBegin,	///< �ȍ~�͉��Z�q
	// operator
	// ��͎��͊�{�I�ɕ������̑傫��������g�[�N���������Ȃ��ƁA�Ⴆ�� + �� ++ ���ԈႦ�邱�Ƃ�����B
	// ��x+++++y is parsed as x ++ ++ + y,
	TT_CppOP_SharpSharp,		// ## (concat)
	TT_CppOP_Sharp,				// # (Prepro/stringize)
	TT_CppOP_ArrowAsterisk,		// ->*
	TT_CppOP_Arrow,				// ->
	TT_CppOP_Comma,				// ,
	TT_CppOP_Increment,			// ++
	TT_CppOP_Decrement,			// --
	TT_CppOP_LogicalAnd,		// &&
	TT_CppOP_LogicalOr,			// ||
	TT_CppOP_LessThenEqual,		// <=
	TT_CppOP_GreaterThenEqual,	// >=
	TT_CppOP_CmpEqual,			// ==
	TT_CppOP_LeftShiftEqual,	// <<=
	TT_CppOP_RightShiftEqual,	// >>=
	TT_CppOP_PlusEqual,			// +=
	TT_CppOP_MinusEqual,		// -=
	TT_CppOP_MulEqual,			// *=
	TT_CppOP_DivEqual,			// /=
	TT_CppOP_ModEqual,			// %=
	TT_CppOP_AndEqual,			// &=
	TT_CppOP_OrEqual,			// |=
	TT_CppOP_NotEqual,			// !=
	TT_CppOP_Equal,				// =
	TT_CppOP_LeftShift,			// <<
	TT_CppOP_RightShift,		// >>
	TT_CppOP_Plus,				// +
	TT_CppOP_Minul,				// -
	TT_CppOP_Asterisk,			// *
	TT_CppOP_Slash,				// /
	TT_CppOP_Parseint,			// %
	TT_CppOP_Ampersand,			// &
	TT_CppOP_Pipe,				// |
	TT_CppOP_Tilde,				// ~
	TT_CppOP_Caret,				// ^
	TT_CppOP_Exclamation,		// !
	TT_CppOP_Ellipsis,			// ...
	TT_CppOP_DotAsterisk,		// .*
	TT_CppOP_Dot,				// .
	TT_CppOP_DoubleColon,		// ::
	TT_CppOP_Question,			// ?
	TT_CppOP_Colon,				// :
	TT_CppOP_Semicolon,			// ;
	TT_CppOP_LeftBrace,			// {
	TT_CppOP_RightBrace,		// }
	TT_CppOP_LeftBracket,		// [
	TT_CppOP_RightBracket,		// ]
	TT_CppOP_LeftParen,			// (
	TT_CppOP_RightParen,		// )
	TT_CppOP_LeftAngle,			// <
	TT_CppOP_RightAngle,		// >

	TT_CppOP_SeparatorEnd,			///< �ȍ~�͉��Z�q

	//<: :> <% %> %: %:%: ?= 
	//and and_eq bitand bitor compl not not_eq
	//or or_eq xor xor_eq

	// new delete �̓L�[���[�h�Ƃ��Ĉ���

	/* ���Ȃ݂� C++11 �̉��Z�q�͈ȉ��̒ʂ�B�g���O���t���܂܂Ȃ���� C++ �Ɠ���
	{ } [ ] # ## ( )
	<: :> <% %> %: %:%: ; : ...
	new delete ? :: . .*
	+ - * / % ^ & | ~
	! = < > += -= *= /= %=
	^= &= |= << >> >>= <<= == !=
	<= >= && || ++ -- , ->* ->
	and and_eq bitand bitor compl not not_eq
	or or_eq xor xor_eq
	*/
};

template<typename TChar>
class CppLexer : public Lexer<TChar>
{
public:
	CppLexer();
	virtual ~CppLexer() {}

protected:

	// �ȉ����I�[�o�[���C�h����ꍇ�A�I�[�m�F�ɂ� GetBufferEnd() ���g�p����
	virtual int CheckSpaceChar(const TChar* buffer);
	virtual int CheckCommentStart(const TChar* buffer);
	virtual int CheckCommentEnd(const TChar* buffer);
	virtual int CheckCommentLine(const TChar* buffer);
	virtual int CheckIdentStart(const TChar* buffer);
	virtual int CheckIdentLetter(const TChar* buffer);
	virtual int CheckKeyword(const TChar* buffer, int* langTokenType);
	virtual int CheckHexLiteralStart(const TChar* buffer);
	virtual int CheckIntegerSuffix(const TChar* buffer);
	virtual int CheckRealSuffix(const TChar* buffer);
	virtual int CheckExponentStart(const TChar* buffer);
	virtual int CheckStringStart(const TChar* buffer);
	virtual int CheckStringEnd(const TChar* buffer, const TChar* start, bool* outIsChar);
	virtual int CheckStringEscape(const TChar* buffer, const TChar* start);
	virtual int CheckOperator(const TChar* buffer, int* langTokenType);
	virtual int CheckEscNewLine(const TChar* buffer);
	virtual bool CheckCaseSensitive() { return true; }	// �啶���Ə���������ʂ���

	virtual void PollingToken(Token<TChar>& token);

private:

	struct WordData
	{
		const TChar*	Word;
		int				Length;
		CppTokenType	Type;
	};

	// #include �̃V�[�P���X
	enum PreProIncludeSeq
	{
		PreProIncludeSeq_Idle = 0,		///< �������Ă��Ȃ�
		PreProIncludeSeq_LineHead,		///< �s���ł���B�܂��͏������
		PreProIncludeSeq_FoundSharp,	///< "#" ��������
		PreProIncludeSeq_FoundInclude,	///< "include" ��������
	};

	PreProIncludeSeq	m_seqPreProInclude;

};

} // namespace Parser
} // namespace Lumino
