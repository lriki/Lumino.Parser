
#pragma once

#include "Lexer.h"

namespace Lumino
{
namespace Parser
{

/// (bison に種別として伝えるため、細かく定数分けする)
enum CppTokenType
{
	TT_Cpp_Unknown = 0,

	TT_CppKW_asm,
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
};

template<typename TChar>
class CppLexer : public Lexer<TChar>
{
public:
	CppLexer() {}
	virtual ~CppLexer() {}

protected:

	// 以下をオーバーライドする場合、終端確認には GetBufferEnd() を使用する
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
	virtual int CheckStringEnd(const TChar* buffer, const TChar* start);
	virtual int CheckStringEscape(const TChar* buffer, const TChar* start);
	virtual int CheckOperator(const TChar* buffer);
	virtual int CheckEscNewLine(const TChar* buffer);
	virtual bool CheckCaseSensitive() { return true; }	// 大文字と小文字を区別する

private:


};

} // namespace Parser
} // namespace Lumino
