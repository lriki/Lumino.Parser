
#if 0
#include <Lumino/Parser/CppLexer.h>
#include <Lumino/Parser/RPN/RPNParser.h>

LN_NAMESPACE_BEGIN
namespace Parser
{

//template<typename TChar>
//class RPNTokenizer
//	: public ParserObject<TChar>
//{
//public:
//	typename typedef RPNElement<TChar>	RPNElementT;
//	typename typedef Array<RPNElementT>	RPNElementListT;
//
//public:
//	static void Tokenize(Position exprBegin, Position exprEnd, RPNElementListT* outElementList);
//};

/*
	C++ �̉��Z�q�ƗD�揇��
	http://ja.cppreference.com/w/cpp/language/operator_precedence
	1+2+3	�� (1+2)+3	�c ������ (Left-to-right)
	a=b=1	�� a=(b=1)	�c �E���� (Right-to-left)
*/

/*
	�� �������Z�q�̉��

		1 ? 6 ? 7 : 8 : 3 ? 4 : 5		������₷�����ʂ�t����Ɓ� 1 ? (6 ? 7 : 8) : (3 ? 4 : 5)

	�Ƃ������́AParse �ł͈ȉ��̂悤�ɓW�J�ł���Ηǂ��B

		idx   >  0  1  2  3  4  5  6  7  8  9 10 11 12
		token >  1  ?  6  ?  7  :  8  :  3  ?  4  :  5
		goto  >     8     6    13    13    12    13

	Eval �ł́A
	�E�g�[�N���͍�����E�֓ǂ�ł����B
	�E? �� goto �́A������(�I�y����stack top)�� false �������ꍇ�ɃW�����v���� idx �������B
	�E: �� goto �́A�����ɂ��ǂ蒅�����Ƃ��ɃW�����v���� idx �������B(�ꗥ�A���̏I�[(��)�ł���)
	���u���̏I�[�v�Ƃ́A')' �܂��̓o�b�t�@�I�[�ł���B

	?: �ȉ��̗D��x�̉��Z�q�͑���n��throw�A, �����A�������l�����Ȃ��ꍇ��
	���Ƃ��ǂꂾ�� : �ŋ�؂��悤�Ƃ� : �ŃW�����v����͕̂K�����̏I�[�ł���B

*/

/*
	�� �֐��Ăяo���̉��

	"1+2" �̂悤�Ȋ֐��Ăяo���̖����P���Ȏ��ł��A��ԊO���ɂ̓_�~�[�̊֐��Ăяo����������̂Ƃ��ĉ�͂���B
	�C���[�W�Ƃ��Ă� "_(1+2)"�B

	���̏I�[�� ) �ł���ƍl���邱�ƂŁA�������Z�q�� , ���Z�q�Ƃ̌��ˍ����������y�ɂȂ�B�����B
	��
	, �͑S�āA�����̋�؂�ƍl����B�J���}���Z�q���B
	�����āA�_�~�[�֐��͈�Ԍ��̈����̕]�����ʂ�Ԃ��֐��Ƃ���B

	"A(1+1,2)" �̃p�[�X���ʂ́A
	
		1 1 + 2 A _

	�ƂȂ�A�g�[�N�� A �� _ �̎�ʂ́u�֐��v�B�v���p�e�B�Ƃ��āA�����̐������B(_ �̓_�~�[�֐��ł���_�ɒ���)
	�]���ł́A���̈����̐������I�y�����h(�]���ςݒ萔)�����o���A�֐��̏������s���B
	A �̓��[�U�[��`�B_ �͍Ō�̈�����Ԃ��B


	"A(1+2,3+4,5+6)" ���p�[������Ƃ��́E�E�E
	, �������邽�тɁAopStack �ɂ��� FuncCall �܂ł��o�̓��X�g�Ɉڂ��B
	) ������������AFuncCall �܂ł��ڂ��ς�����AFuncCall �������B
	


	
*/


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
ArrayList< RPNToken<TChar> >* RPNParser<TChar>::ParseCppConstExpression(Position exprBegin, Position exprEnd, ErrorManager* errorInfo)
{
	RPNParser<TChar> parser;
	parser.TokenizeCppConst(exprBegin, exprEnd);
	parser.Parse();
	parser.m_rpnTokenList.SafeAddRef();
	return parser.m_rpnTokenList;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void RPNParser<TChar>::TokenizeCppConst(Position exprBegin, Position exprEnd)
{
	// �Ƃ肠�������̓g�[�N�����Ń������m�� (�X�y�[�X���܂܂�Ă���� Tokenize ��̎g�p�ʂ͏��Ȃ��Ȃ�͂�)
	m_tokenList.Attach(LN_NEW RPNTokenListT());
	m_tokenList->Reserve(exprEnd - exprBegin + 2);

	// ���������X�g��͏����� , ���Z�q�̉�͂����˂���悤�ɁA���X�g�擪�Ƀ_�~�[�� FuncCall �����Ă���
	RPNTokenT headToken;
	headToken.Type = RPN_TT_OP_FuncCall;
	m_tokenList->Add(headToken);

	Position pos = exprBegin;

	for (; pos != exprEnd; ++pos)
	{
		switch (pos->GetTokenType())
		{
		case TokenType_SpaceSequence:
		case TokenType_NewLine:
		case TokenType_Comment:
		case TokenType_EscNewLine:
			break;	// �����͋󔒈����B������������
		case TokenType_Identifier:
		{
			RPNTokenT token;
			token.Type = RPN_TT_Identifier;
			token.SourceToken = &(*pos);
			m_tokenList->Add(token);
			break;
		}
		case TokenType_NumericLiteral:
		{
			RPNTokenT token;
			token.Type = RPN_TT_NumericLiteral;
			token.SourceToken = &(*pos);
			m_tokenList->Add(token);
			break;
		}
		case TokenType_Operator:
		{
			static const RPNTokenType CppTypeToRPNType[] =
			{
				/* TT_CppOP_SeparatorBegin		*/	RPN_TT_Unknown,
				/* TT_CppOP_SharpSharp  	##	*/	RPN_TT_Unknown,
				/* TT_CppOP_Sharp			#	*/	RPN_TT_Unknown,
				/* TT_CppOP_ArrowAsterisk	->*	*/	RPN_TT_Unknown,
				/* TT_CppOP_Arrow			->	*/	RPN_TT_Unknown,
				/* TT_CppOP_Comma			,	*/	RPN_TT_OP_Comma,
				/* TT_CppOP_Increment		++	*/	RPN_TT_Unknown,
				/* TT_CppOP_Decrement		--	*/	RPN_TT_Unknown,
				/* TT_CppOP_LogicalAnd		&&	*/	RPN_TT_OP_LogicalAnd,
				/* TT_CppOP_LogicalOr		||	*/	RPN_TT_OP_LogicalOr,
				/* TT_CppOP_LessThenEqual	<=	*/	RPN_TT_OP_CompLessThanEqual,
				/* TT_CppOP_GreaterThenEqual>=	*/	RPN_TT_OP_CompGreaterThenEqual,
				/* TT_CppOP_CmpEqual		==	*/	RPN_TT_OP_CompEqual,
				/* TT_CppOP_LeftShiftEqual	<<=	*/	RPN_TT_Unknown,
				/* TT_CppOP_RightShiftEqual	>>=	*/	RPN_TT_Unknown,
				/* TT_CppOP_PlusEqual		+=	*/	RPN_TT_Unknown,
				/* TT_CppOP_MinusEqual		-=	*/	RPN_TT_Unknown,
				/* TT_CppOP_MulEqual		*=	*/	RPN_TT_Unknown,
				/* TT_CppOP_DivEqual		/=	*/	RPN_TT_Unknown,
				/* TT_CppOP_ModEqual		%=	*/	RPN_TT_Unknown,
				/* TT_CppOP_AndEqual		&=	*/	RPN_TT_Unknown,
				/* TT_CppOP_OrEqual			|=	*/	RPN_TT_Unknown,
				/* TT_CppOP_NotEqual		!=	*/	RPN_TT_OP_CompNotEqual,
				/* TT_CppOP_Equal			=	*/	RPN_TT_Unknown,
				/* TT_CppOP_LeftShift		<<	*/	RPN_TT_OP_LeftShift,
				/* TT_CppOP_RightShift		>>	*/	RPN_TT_OP_RightShift,
				/* TT_CppOP_Plus			+	*/	RPN_TT_OP_BinaryPlus,
				/* TT_CppOP_Minul			-	*/	RPN_TT_OP_BinaryMinus,
				/* TT_CppOP_Asterisk		*	*/	RPN_TT_OP_Multiply,
				/* TT_CppOP_Slash			/	*/	RPN_TT_OP_Divide,
				/* TT_CppOP_Parseint		%	*/	RPN_TT_OP_Modulus,
				/* TT_CppOP_Ampersand		&	*/	RPN_TT_OP_BitwiseAnd,
				/* TT_CppOP_Pipe			|	*/	RPN_TT_OP_BitwiseOr,
				/* TT_CppOP_Tilde			~	*/	RPN_TT_OP_BitwiseNot,
				/* TT_CppOP_Caret,			^	*/	RPN_TT_OP_BitwiseXor,
				/* TT_CppOP_Exclamation		!	*/	RPN_TT_OP_LogicalNot,
				/* TT_CppOP_Ellipsis		...	*/	RPN_TT_Unknown,
				/* TT_CppOP_DotAsterisk		.*	*/	RPN_TT_Unknown,
				/* TT_CppOP_Dot				.	*/	RPN_TT_Unknown,
				/* TT_CppOP_DoubleColon		::	*/	RPN_TT_Unknown,
				/* TT_CppOP_Question		?	*/	RPN_TT_OP_CondTrue,
				/* TT_CppOP_Colon			:	*/	RPN_TT_OP_CondFalse,
				/* TT_CppOP_Semicolon		;	*/	RPN_TT_Unknown,
				/* TT_CppOP_LeftBrace		{	*/	RPN_TT_Unknown,
				/* TT_CppOP_RightBrace		}	*/	RPN_TT_Unknown,
				/* TT_CppOP_LeftBracket		[	*/	RPN_TT_Unknown,
				/* TT_CppOP_RightBracket	]	*/	RPN_TT_Unknown,
				/* TT_CppOP_LeftParen		(	*/	RPN_TT_OP_GroupStart,
				/* TT_CppOP_RightParen		)	*/	RPN_TT_OP_GroupEnd,
				/* TT_CppOP_LeftAngle		<	*/	RPN_TT_OP_CompLessThan,
				/* TT_CppOP_RightAngle		>	*/	RPN_TT_OP_CompGreaterThen,
			};
			assert(LN_ARRAY_SIZE_OF(CppTypeToRPNType) == (TT_CppOP_SeparatorEnd - TT_CppOP_SeparatorBegin));

			struct CppTokenInfo
			{
				int					Precedence;		///< �D�揇��
				OpeatorAssociation	Association;	///< ��������
			};
			static const CppTokenInfo TokenInfoTable[] =
			{
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_Unknown,				// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_Identifier,			// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLiteral,		// (Dummy)
				{ 2,	OpeatorAssociation_Left },	// RPN_TT_OP_GroupStart,		// (
				{ 2,	OpeatorAssociation_Left },	// RPN_TT_OP_GroupEnd,			// )
				{ 3,	OpeatorAssociation_Right },	// RPN_TT_OP_UnaryPlus,			// +
				{ 3,	OpeatorAssociation_Right },	// RPN_TT_OP_UnaryMinus,		// -
				{ 3,	OpeatorAssociation_Right },	// RPN_TT_OP_LogicalNot,		// !
				{ 3,	OpeatorAssociation_Right },	// RPN_TT_OP_BitwiseNot,		// ~
				{ 5,	OpeatorAssociation_Left },	// RPN_TT_OP_Multiply,			// *
				{ 5,	OpeatorAssociation_Left },	// RPN_TT_OP_Divide,			// /
				{ 5,	OpeatorAssociation_Left },	// RPN_TT_OP_IntegerDivide,		// %
				{ 6,	OpeatorAssociation_Left },	// RPN_TT_OP_BinaryPlus,		// + (Binary)
				{ 6,	OpeatorAssociation_Left },	// RPN_TT_OP_BinaryMinus,		// - (Binary)
				{ 7,	OpeatorAssociation_Left },	// RPN_TT_OP_LeftShift,			// <<
				{ 7,	OpeatorAssociation_Left },	// RPN_TT_OP_RightShift,		// >>
				{ 8,	OpeatorAssociation_Left },	// RPN_TT_OP_CompLessThan,		// <
				{ 8,	OpeatorAssociation_Left },	// RPN_TT_OP_CompLessThanEqual,	// <=
				{ 8,	OpeatorAssociation_Left },	// RPN_TT_OP_CompGreaterThen,	// >
				{ 8,	OpeatorAssociation_Left },	// RPN_TT_OP_CompGreaterThenEqual,// >=
				{ 9,	OpeatorAssociation_Left },	// RPN_TT_OP_CompEqual,			// ==
				{ 9,	OpeatorAssociation_Left },	// RPN_TT_OP_CompNotEqual,		// !=
				{ 10,	OpeatorAssociation_Left },	// RPN_TT_OP_BitwiseAnd,		// &
				{ 11,	OpeatorAssociation_Left },	// RPN_TT_OP_BitwiseXor,		// ^
				{ 12,	OpeatorAssociation_Left },	// RPN_TT_OP_BitwiseOr,			// |
				{ 13,	OpeatorAssociation_Left },	// RPN_TT_OP_LogicalAnd,		// &&
				{ 14,	OpeatorAssociation_Left },	// RPN_TT_OP_LogicalOr,			// ||
				{ 15,	OpeatorAssociation_Right },	// RPN_TT_OP_CondTrue,			// ? (�������Z�q)
				{ 15,	OpeatorAssociation_Left },	// RPN_TT_OP_CondFalse,			// : (�������Z�q)
				{ 17,	OpeatorAssociation_Left },	// RPN_TT_OP_Comma,				// , (�J���}���Z�q or ��������؂蕶��)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_OP_FuncCall,			// (Dummy)
			};
			assert(LN_ARRAY_SIZE_OF(TokenInfoTable) == RPN_TT_Max);

			RPNTokenT token;
			token.Type = CppTypeToRPNType[pos->GetLangTokenType() - TT_CppOP_SeparatorBegin];

			// ( ���ЂƂO�����ʎq�̏ꍇ�͊֐��Ăяo���Ƃ���
			if (token.Type == RPN_TT_OP_GroupStart &&
				!m_tokenList->IsEmpty() &&
				m_tokenList->GetLast().Type == RPN_TT_Identifier)
			{
				// Identifer �̎�ނ� FuncCall �ɕύX���A( �̓g�[�N���Ƃ��Ē��o���Ȃ�
				m_tokenList->GetLast().Type = RPN_TT_OP_FuncCall;
			}
			else
			{
				// + or - �̏ꍇ�͒P�����Z�q�ł��邩�������Ŋm�F����B
				// �ЂƂO�̗L���g�[�N�������Z�q�ł���ΒP�����Z�q�ł���B
				if (token.Type == RPN_TT_OP_BinaryPlus || token.Type == RPN_TT_OP_BinaryMinus)
				{
					if (!m_tokenList->IsEmpty() && m_tokenList->GetLast().IsOperator())
					{
						if (token.Type == RPN_TT_OP_BinaryPlus) {
							token.Type = RPN_TT_OP_UnaryPlus;
						}
						else {	// if (token.Type == RPN_TT_OP_BinaryMinus)
							token.Type = RPN_TT_OP_UnaryMinus;
						}
					}
				}

				token.Precedence = TokenInfoTable[token.Type].Precedence;
				token.Association = TokenInfoTable[token.Type].Association;
				token.SourceToken = &(*pos);
				m_tokenList->Add(token);
			}
			break;
		}
		default:
			// TODO:error
			break;
		}
	}

	// ���������X�g��͏����� , ���Z�q�̉�͂����˂���悤�ɁA���X�g�I�[�Ƀ_�~�[�� GroupEnd �����Ă���
	RPNTokenT tailToken;
	tailToken.Type = RPN_TT_OP_GroupEnd;
	m_tokenList->Add(tailToken);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void RPNParser<TChar>::Parse()
{
	m_tmpRPNTokenList.Reserve(m_tokenList->GetCount());
	m_lastToken = NULL;

	LN_FOREACH(RPNTokenT& token, *m_tokenList)
	{
		// ���݂� () �[����U���Ă���
		token.GroupLevel = m_groupStack.GetCount();

		// �萔�͏o�̓��X�g�֐ς�ł���
		if (token.Type == RPN_TT_NumericLiteral)
		{
			m_tmpRPNTokenList.Add(&token);
		}
		else if (token.IsOperator())
		{
			switch (token.Type)
			{
			case RPN_TT_OP_FuncCall:
				m_groupStack.Push(&token);
				m_opStack.Push(&token);
				break;
			case RPN_TT_OP_GroupStart:
				m_groupStack.Push(&token);
				m_opStack.Push(&token);
				break;
			case RPN_TT_OP_GroupEnd:			// m_tokenList �̍Ō�̓_�~�[�� ) �����Ă��邽�߁A�Ō��1�x�K���ʂ�
				if (m_lastToken->Type != RPN_TT_OP_GroupStart) {
					m_groupStack.GetTop()->ElementCount++;	// () �����̈����̐��𑝂₷�B�������A"Func()" �̂悤�Ɏ�������������Α��₳�Ȃ�
				}
				PopOpStackGroupEnd(false);		// opStack �� GroupStart �܂��� FuncCall �܂ł̓��e���o�̓��X�g�Ɉڂ��BGroupStart �܂��� FuncCall �͍폜����B
				CloseGroup(false);				// �����x�� () ���� ':' �S�Ă� CondGoto ��U��B�Ō�Ɍ��݂̃O���[�v�����폜����
				break;
			case RPN_TT_OP_CondTrue:
				PushOpStack(&token);
				m_tmpRPNTokenList.Add(&token);
				break;
			case RPN_TT_OP_CondFalse:
			{
				RPNTokenT* condTrue = PopOpStackCondFalse();
				if (!condTrue) { return; }		// Error : ':' �ɑΉ����� ? ��������Ȃ�����
				m_tmpRPNTokenList.Add(&token);
				m_condStack.Push(&token);
				condTrue->CondGoto = m_tmpRPNTokenList.GetCount();	// �W�����v��Ƃ��� ':' �̎����w��
				break;
			}
			case RPN_TT_OP_Comma:
				// ���݂̃O���[�v�����̎������̐����C���N�������g�B�ŏ��� , �̏ꍇ�� 1 �]����++����B
				//if (m_groupStack.GetTop()->ElementCount == 0) {	
				//	m_groupStack.GetTop()->ElementCount++;
				//}
				m_groupStack.GetTop()->ElementCount++;	

				PopOpStackGroupEnd(true);				// ( �` , �܂ł� opStack �̓��e���o�̓��X�g�Ɉڂ��B�������A( �͎c�����܂܂ɂ���
				CloseGroup(true);						// �O���[�v���̏������Z�q�̏������s���B�������A���̌�O���[�v���͍폜���Ȃ�
				break;
			// �ʏ�̉��Z�q
			default:
				PushOpStack(&token);
				break;
			}
		}

		m_lastToken = &token;
	}

	PopOpStackCondFalse();

	// �����x�� () ���� ':' �S�Ă� CondGoto ��U�� (��ԊO���� GroupEnd �����Ƃ����l��)
	//CloseGroup();

	// ���Z�q�p�X�^�b�N�Ɏc���Ă���v�f��S�ďo�̓��X�g�Ɉڂ�
	while (!m_opStack.IsEmpty())
	{
		RPNTokenT* top;
		m_opStack.Pop(&top);
		if (top->Type == RPN_TT_OP_GroupStart) {
			//TODO: error���ʂ����Ă��Ȃ�
			break;
		}
		else if (top->Type == RPN_TT_OP_CondTrue) {
			//TODO: error�������Z�q�����Ă��Ȃ�
			break;
		}
		m_tmpRPNTokenList.Add(top);
	}

	// �o�͗p�̃��X�g�ցA�g�[�N���̃R�s�[���쐬���ڂ�
	m_rpnTokenList.Attach(LN_NEW RPNTokenListT());
	m_rpnTokenList->Reserve(m_tmpRPNTokenList.GetCount());
	LN_FOREACH(RPNTokenT* token, m_tmpRPNTokenList)
	{
		m_rpnTokenList->Add(*token);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void RPNParser<TChar>::PushOpStack(RPNTokenT* token)	// Operator �܂��� CondTrue ���� PushOpStack �����
{
	// �X�^�b�N�ɂ�����̂���菜����ƁB
	// ���ꂩ�����悤�Ƃ��Ă�����̂��Atop �̗D��x�̕���������Ύ�菜���B
	// �����ꍇ�͎�菜���Ȃ��B
	// �X�^�b�N�ɂ͗D��x�̒Ⴂ���̂��c�葱���邱�ƂɂȂ�B
	while (!m_opStack.IsEmpty())
	{
		RPNTokenT* top = m_opStack.GetTop();
		if (top->Type == RPN_TT_OP_GroupStart || top->Type == RPN_TT_OP_FuncCall) {
			// '(' �͓��ʈ����B�Ƃɂ������Z�q�X�^�b�N�̐擪�ɐςށB(���Z�q�̗D��x�łǂ������ł��Ȃ�)
			// �ʓr�A')' �����������Ƃ��A�Ή����� '(' �܂ł̃X�^�b�N�v�f��S�Ă��o�̓��X�g�ֈڂ��B
			break;
		}

		if (top->Precedence < token->Precedence)	// [+ * �� +] �ƌ�����ԂȂ�A*(5) +(6) �Ȃ̂� * ����菜��
		{
			// �o�̓��X�g��
			m_tmpRPNTokenList.Add(top);
			m_opStack.Pop();
		}
		else {
			break;
		}
	}
	m_opStack.Push(token);
}

//-----------------------------------------------------------------------------
// GroupEnd (')') �܂��� , ������������Ă΂��
//-----------------------------------------------------------------------------
template<typename TChar>
RPNToken<TChar>* RPNParser<TChar>::PopOpStackGroupEnd(bool fromArgsSeparator)
{
	// �Ή����� GroupStart ('(') ��������܂ŃX�^�b�N�̉��Z�q���o�̓��X�g�ֈڂ��Ă����B
	RPNTokenT* top = NULL;
	while (!m_opStack.IsEmpty())
	{
		top = m_opStack.GetTop();
		if (top->Type == RPN_TT_OP_GroupStart)
		{
			m_opStack.Pop();	// GroupStart �͎̂Ă�
			break;
		}
		else if (top->Type == RPN_TT_OP_FuncCall)
		{
			// FuncCall �͏o�̓��X�g�̖����ɐς݁A�I������B
			// �������A, �̏ꍇ�͐ς܂Ȃ��B
			if (!fromArgsSeparator)
			{
				m_tmpRPNTokenList.Add(top);	
				m_opStack.Pop();
			}
			break;
		}

		// �o�̓��X�g��
		m_tmpRPNTokenList.Add(top);
		m_opStack.Pop();
		top = NULL;
	}

	if (!top) {
		// TODO: error �K�� GroupStart ��������΂Ȃ�Ȃ�
		// ���~�B�Ō�ɂ�1�x�Ă΂�邽�߃G���[�ɂ��Ă͂����Ȃ�
		return NULL;
	}
	return top;
}

//-----------------------------------------------------------------------------
// CondFalse (':') ������������Ă΂��
//-----------------------------------------------------------------------------
template<typename TChar>
RPNToken<TChar>* RPNParser<TChar>::PopOpStackCondFalse()
{
	// �Ή����� CondStart ('?') ��������܂ŃX�^�b�N�̉��Z�q���o�̓��X�g�ֈڂ��Ă����B
	RPNTokenT* top = NULL;
	while (!m_opStack.IsEmpty())
	{
		top = m_opStack.GetTop();
		if (top->Type == RPN_TT_OP_CondTrue) {
			m_opStack.Pop();	// CondTrue �͎̂Ă�
			break;
		}

		// �o�̓��X�g��
		m_tmpRPNTokenList.Add(top);
		m_opStack.Pop();
	}
	/* 
		�����O�� ? ��T���ɍs�������ŗǂ��B
		( ) �Ɠ����l�����B
		x    ?    x ? x : x    :    x ? x : x
	*/

	if (!top) {
		// TODO: error �K�� CondTrue ��������΂Ȃ�Ȃ�
		return NULL;
	}
	return top;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void RPNParser<TChar>::CloseGroup(bool fromArgsSeparator)
{
	// ���݂� () ���x���� ':' �S�Ă� CondGoto ��U��A�X�^�b�N�����菜��
	while (!m_condStack.IsEmpty())
	{
		RPNTokenT* condFalse = m_condStack.GetTop();
		if (condFalse->GroupLevel < m_groupStack.GetCount()) {
			break;
		}
		condFalse->CondGoto = m_tmpRPNTokenList.GetCount();
		m_condStack.Pop();
	}

	// �O���[�v����1�폜����B�������A, �̏ꍇ�͎c���Ă����B
	if (!fromArgsSeparator) {
		m_groupStack.Pop();
	}
}

// �e���v���[�g�̃C���X�^���X��
template class RPNParser<char>;
template class RPNParser<wchar_t>;

} // namespace Parser
LN_NAMESPACE_END
#endif
