
#pragma once

#include <Lumino/IO/PathName.h>
#include "../ParserObject.h"

namespace Lumino
{
namespace Parser
{
class ErrorManager;

enum RPNTokenType
{
	RPN_TT_Unknown = 0,

	RPN_TT_NumericLiteral,			///< ���l���e����

	RPN_TT_OP_LeftParen,			// (
	RPN_TT_OP_RightParen,			// )

	RPN_TT_OP_LogicalNot,			// !
	RPN_TT_OP_BitwiseNot,			// ~

	RPN_TT_OP_Multiply,				// *
	RPN_TT_OP_Divide,				// /
	RPN_TT_OP_IntegerDivide,		// %

	RPN_TT_OP_Plus,					// + (Unary or Binary)
	RPN_TT_OP_Minus,				// - (Unary or Binary)

	RPN_TT_OP_LeftShift,			// <<
	RPN_TT_OP_RightShift,			// >>

	RPN_TT_OP_CompLessThan,			// <
	RPN_TT_OP_CompLessThanEqual,	// <=
	RPN_TT_OP_CompGreaterThen,		// >
	RPN_TT_OP_CompGreaterThenEqual,	// >=

	RPN_TT_OP_CompEqual,			// ==
	RPN_TT_OP_CompNotEqual,			// !=

	RPN_TT_OP_BitwiseAnd,			// &

	RPN_TT_OP_BitwiseXor,			// ^

	RPN_TT_OP_BitwiseOr,			// |

	RPN_TT_OP_LogicalAnd,			// &&

	RPN_TT_OP_LogicalOr,			// ||

	RPN_TT_OP_CondTrue,				// ? (�������Z�q)
	RPN_TT_OP_CondFalse,			// : (�������Z�q)

	//RPN_TT_OP_Negation,
	//RPN_TT_OP_Exponent,			// **

	RPN_TT_Max,	
};

/// ���Z�q�̌�������
enum OpeatorAssociation
{
	OpeatorAssociation_Left,
	OpeatorAssociation_Right,
};

/**
	@brief	�t�|�[�����h�����\������v�f�ł��B
*/
template<typename TChar>
class RPNToken
{
public:
	RPNTokenType		Type;
	int					Precedence;		///< �D�揇��
	OpeatorAssociation	Association;	///< ��������
	const Token<TChar>*	SourceToken;
};

template<typename TChar>
class RPNParser
	: public ParserObject<TChar>
{
public:
	typename typedef RPNToken<TChar>	RPNTokenT;
	typename typedef Array<RPNTokenT>	RPNTokenListT;

public:
	static void ParseCppConstExpression(Position exprBegin, Position exprEnd, RPNTokenListT* outElementList, ErrorManager* errorInfo);

private:
	void TokenizeCppConst(Position exprBegin, Position exprEnd);

private:
	RPNTokenListT	m_tokenList;
};

} // namespace Parser
} // namespace Lumino
