
#pragma once
#include <unordered_map>
#include "../../Common.h"
#include "../../TokenList.h"
#include "../RpnParser.h"

LN_NAMESPACE_BEGIN
namespace parser
{

template<typename TValue>
class IdentifierMap
{
public:
	IdentifierMap()
		: m_table1(nullptr)
	{
		m_table1 = LN_NEW Item[MaxKeyFirstCount * MaxKeyLastCount * MaxKeyLength];
	}

	virtual ~IdentifierMap()
	{
		LN_SAFE_DELETE_ARRAY(m_table1);
	}

	void Insert(const TokenChar* keyBegin, const TokenChar* keyEnd, TValue value)
	{
		assert(keyBegin);
		assert(keyEnd);
		assert(keyBegin < keyEnd);
		int len = keyEnd - keyBegin;
		if (len < MaxKeyLength)
		{
			Item* item = GetItem(*keyBegin, *(keyEnd - 1), len);
			if (item != nullptr)
			{
				memcpy(item->key.data(), keyBegin, len);
				item->item = value;
				return;
			}
		}

		TokenString key(keyBegin, len);
		m_table2[key] = value;
	}

	bool Find(const TokenChar* keyBegin, const TokenChar* keyEnd, TValue* outValue, CaseSensitivity cs)
	{
		int len = keyEnd - keyBegin;
		if (len < MaxKeyLength)
		{
			Item* item = GetItem(*keyBegin, *(keyEnd - 1), len);
			if (item != nullptr && item->key[0] != '\0')
			{
				if (StringTraits::Compare(keyBegin, item->key.data(), len, cs) == 0)
				{
					*outValue = item->item;
					return true;
				}
			}
		}

		auto itr = m_table2.find(TokenString(keyBegin, keyEnd - keyBegin));	// TODO: StringRef �Ō����ł���悤��
		if (itr != m_table2.end())
		{
			*outValue = itr->second;
			return true;
		}
		return false;
	}

public:

	/*
		a b c d e f g h i j k l m
		n o p q r s t u v w x y z
		A B C D E F G H I J K L M
		N O P Q R S T U V W X Y Z _

		0 1 2 3 4 5 6 7 8 9

		universal-character-name:
			\u hex-quad
			\U hex-quad hex-quad
	*/

	static const int MaxKeyFirstCount = 64;
	static const int MaxKeyLastCount = 64;
	static const int MaxKeyLength = 64;

	struct Item
	{
		std::array<TokenChar, MaxKeyLength>	key = {};	// [0] �� '\0' �Ȃ�Insert����Ă��Ȃ��Ƃ�������
		TValue								item;
	};

	//typedef Array<Item>	ItemArray;

	Item*					m_table1;	// [�擪����][�I�[����][������]
	//std::unordered_map<StringA, Item>	m_table2;
	std::map<TokenString, TValue>	m_table2;

	// �擪�����A�I�[�����A�������� Item ���擾����B���O����v���邩�͉��߂ă`�F�b�N���邱�ƁB
	Item* GetItem(TokenChar first, TokenChar last, int len)
	{
		static int charIndexTable[128] =
		{
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,			// '0'~'9'
			-1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,	// 'A'~'O'
			25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, 36,	// 'P'~'Z', '_'
			-1, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,	// 'a'~'o'
			52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, -1, -1, -1, -1, -1,
		};
		assert(first < 128);
		assert(last < 128);
		int i1 = charIndexTable[first];
		int i2 = charIndexTable[last];
		if (i1 == -1) return nullptr;
		if (i2 == -1) return nullptr;
		return &m_table1[(i1 * MaxKeyLastCount * MaxKeyLength) + (i2 * MaxKeyLength) + len];
	}
};

// �}�N����`�̎��́B�����}�N���͂��肦��(undef ��̍Ē�`���Ă��O�͕̂ێ�����)�B���̏ꍇ�͒ʏ�A��`�s���Ⴄ�B
// 
class MacroEntity
{
public:
	TokenString	name;
	TokenString	replacementContentString;	// �u���v�f�𕶎���ŕ��ׂ����� (���S��v�̊m�F�Ŏg���B�O��̋󔒂͏����Ă���)

	bool		undef = false;
};

class MacroMap
{
public:
	MacroEntity* Insert(const Token& name, const TokenChar* replacementBegin, const TokenChar* replacementEnd);

	MacroEntity* Find(const Token& name);

	bool IsDefined(const Token& name);

private:
	Array<MacroEntity>			m_allMacroList;	// �ߋ��ɒ�`���ꂽ�S�Ẵ}�N��
	IdentifierMap<MacroEntity*>	m_macroMap;		// �Ē�`���ꂽ�肵�����͈̂�ԐV�����}�N�����i�[�����
};

class RawReferenceMap
{

};

// �v���v���Z�X�����t�@�C�����B
// .c �� .h ���͖��Ȃ��B
// �~�g�[�N�����X�g�͕ێ����Ȃ��̂Œ��ӁB
/*
	�g�[�N�����X�g���ێ�����BUIColors.h �Ƃ��B
*/
class PreprocessedFileCacheItem
{
public:
	PathNameA		filePath;
	MacroMap		inputMacroMap;

	MacroMap		outputMacroMap;
	RawReferenceMap	outputRawReferenceMap;
};

class HeaderFileManager
{
public:

};
	
/**
	@brief
*/
class Preprocessor
{
public:
	Preprocessor();

	ResultState BuildPreprocessedTokenList(TokenList* tokenList, PreprocessedFileCacheItem* outFileCache, DiagnosticsItemSet* diag);

private:

	ResultState PollingDirectiveLine(Token* lineBegin, Token* lineEnd);

	bool IsInValidSection() const;

	//TokenList::iterator GetNextGenericToken(TokenList::iterator pos);



	enum class DirectiveSec
	{
		Idle,			// �������Ă��Ȃ�
		LineHead,		// �s���ł���B�܂��͏������
		FindIdent,		// ���ʎq��T���Ă���
		FindLineEnd,	// "#" ��������
	};

	// #if �` #endif �܂ł̔�����
	enum class ConditionalSectionState
	{
		None,			// ����O�A�Z�N�V�����O
		Valid,			// �L������O���[�v��
		Invalid,		// ��������O���[�v��
		Skip,			// �L����������I����A#endif�܂�skip�\
	};

	// #if �` #endif �܂ł̏��
	struct ConditionalSection
	{
		ConditionalSectionState	state = ConditionalSectionState::None;	// #if�`#endif �܂ł̌��݂̔�����
		bool					elseProcessed = false;					// #else��t���true(#else�`#else�h�~�̈�)
	};

	TokenList*					m_tokenList;
	PreprocessedFileCacheItem*	m_fileCache;
	DiagnosticsItemSet*			m_diag;

	DirectiveSec				m_seqDirective;
	Stack<ConditionalSection>	m_conditionalSectionStack;
	Token*						m_preproLineHead;	// # �̎��̃g�[�N�����w���Ă���

	TokenList					m_preproExprTokenList;	// �O�����萔���̃g�[�N����W�J����
	RpnParser					m_rpnParser;
	RpnEvaluator				m_rpnEvaluator;
};

} // namespace Parser
LN_NAMESPACE_END

