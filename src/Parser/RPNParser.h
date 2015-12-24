
#pragma once
#include <Lumino/Base/Stack.h>
#include <Lumino/IO/PathName.h>
#include "Common.h"
#include "Token.h"
#include "TokenList.h"

LN_NAMESPACE_BEGIN
namespace parser
{
enum RPNTokenType
{
	RPN_TT_Unknown = 0,

	RPN_TT_Identifier,				///< ���ʎq
	RPN_TT_NumericLiteral,			///< ���l���e����

	RPN_TT_OP_GroupStart,			// (
	RPN_TT_OP_GroupEnd,				// )

	RPN_TT_OP_UnaryPlus,			// + (Unary)
	RPN_TT_OP_UnaryMinus,			// - (Unary)
	RPN_TT_OP_LogicalNot,			// !
	RPN_TT_OP_BitwiseNot,			// ~

	RPN_TT_OP_Multiply,				// *
	RPN_TT_OP_Divide,				// /
	RPN_TT_OP_Modulus,				// %

	RPN_TT_OP_BinaryPlus,			// + (Binary)
	RPN_TT_OP_BinaryMinus,			// - (Binary)

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

	RPN_TT_OP_Comma,				// , (�J���}���Z�q or ��������؂蕶��)



	RPN_TT_OP_FuncCall,				///< �֐��Ăяo�� (���ʎq�������w��)


	RPN_TT_Max,	
};

enum class RpnTokenGroup
{
	Unknown,
	Literal,
	Constant,
	Identifier,
	Operator,
	CondTrue,
	CondFalse,
	Function,
	Assignment,
};

/** ���Z�q�̃O���[�v (�G���[�����Ȃǂ�����) */
enum class RpnOperatorGroup
{
	Unknown,
	Arithmetic,
	Comparison,
	Logical,
	Bitwise,
	Conditional,
	Assignment,
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
class RPNToken
{
public:
	RPNToken()
	{
		SourceToken = NULL;
		CondGoto = 0;
		ElementCount = 0;
	}

public:
	bool IsOperator() const { return RPN_TT_OP_GroupStart <= Type && Type <= RPN_TT_OP_FuncCall; }

	RpnTokenGroup GetTokenGroup() const;
	RpnOperatorGroup GetOperatorGroup() const;

public:
	RPNTokenType		Type;
	int					Precedence;		///< �D�揇��
	OpeatorAssociation	Association;	///< ��������
	const Token*		SourceToken;
	int					GroupLevel;		///< () �l�X�g�̐[���B���[�g�� 0

	int					CondGoto;		///< (Type �� CondTrue �܂��� CondFalse �̂Ƃ��Ɏg�p����)
	int					ElementCount;	///< , �ŋ�؂�ꂽ�v�f�� (Type �� GroupStart �܂��� FuncCall �̂Ƃ��Ɏg�p����)


};

class RPNTokenList
	: public RefObject
	, public Collection<RPNToken>
{
};

class RPNParser
{
public:
	typedef Collection<Token>::const_iterator Position;

public:
	// TODO: RefPtr
	static RefPtr<RPNTokenList> ParseCppConstExpression(Position exprBegin, Position exprEnd, DiagnosticsItemSet* diag);

private:
	void TokenizeCppConst(Position exprBegin, Position exprEnd);
	void Parse();
	void PushOpStack(RPNToken* token);
	RPNToken* PopOpStackGroupEnd(bool fromArgsSeparator);
	RPNToken* PopOpStackCondFalse();
	void CloseGroup(bool fromArgsSeparator);

private:
	RefPtr<RPNTokenList>	m_tokenList;
	RefPtr<RPNTokenList>	m_rpnTokenList;

	Array<RPNToken*>		m_tmpRPNTokenList;
	Stack<RPNToken*>		m_opStack;			// ���Z�q�p�̍�ƃX�^�b�N
	Stack<RPNToken*>		m_condStack;		// �������Z�q�p�̍�ƃX�^�b�N�B: ���i�[���Ă���
	Stack<RPNToken*>		m_groupStack;		// () �̍�ƃX�^�b�N�B( �܂��� FuncCall ���i�[���Ă���
	RPNToken*				m_lastToken;
};





enum class RpnOperandType
{
	Double,
};


class RpnOperand
{
public:
	RpnOperandType	type;

	union
	{
		double	valueDouble;
	};

	//RpnOperand(const RPNToken& rpnToken)
	//{
	//	if (rpnToken.Type == RPN_TT_NumericLiteral)
	//	
	//}
};

class RpnEvaluator
{
public:
	bool TryEval(const RPNTokenList* tokenList, DiagnosticsItemSet* diag, RpnOperand* outValue);

private:
	bool MakeOperand(const RPNToken& token, RpnOperand* outOperand);

private:
	DiagnosticsItemSet*	m_diag;
};

} // namespace parser
LN_NAMESPACE_END
