
#pragma once
#include <unordered_map>
#include "../../Common.h"
#include "../../TokenList.h"

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
		m_table1 = LN_NEW ItemArray[MaxKeyFirstCount * MaxKeyLastCount * MaxKeyLength];
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
				item->value = value;
				return;
			}
		}

		StringA key(keyBegin, len);
		m_table2[key] = value;
	}

	bool Find(const TokenChar* keyBegin, const TokenChar* keyEnd, TValue* outValue)
	{
		if (len < MaxKeyLength)
		{
			Item* item = GetItem(*keyBegin, *(keyEnd - 1), len);
			if (item != nullptr)
			{
				if (StringTraits::Compare(keyBegin)
			}
		}

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
		std::array<TokenChar, MaxKeyLength>	key = {};
		TValue								item;
	};

	typedef Array<Item>	ItemArray;

	ItemArray*					m_table1;	// [�擪����][�I�[����][������]
	std::unordered_map<StringA, Item>	m_table2;

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
};

class MacroMap
{
public:
	MacroEntity* Insert(const Token& name, const TokenChar* replacementBegin, const TokenChar* replacementEnd);

	MacroEntity* Find(const Token& name);

private:
	Array<MacroEntity>			m_allMacroList;	// �ߋ��ɒ�`���ꂽ�S�Ẵ}�N��
	IdentifierMap<MacroEntity*>	m_macroMap;		// �Ē�`���ꂽ�肵�����͈̂�ԐV�����}�N�����i�[�����
};

class RawReferenceMap
{

};

// �v���v���Z�X�����t�@�C�����B
// .c �� .h ���͖��Ȃ��B
// �g�[�N�����X�g�͕ێ����Ȃ��̂Œ��ӁB
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

	//TokenList::iterator GetNextGenericToken(TokenList::iterator pos);


	enum class DirectiveSec
	{
		Idle,			// �������Ă��Ȃ�
		LineHead,		// �s���ł���B�܂��͏������
		FindIdent,		// ���ʎq��T���Ă���
		FindLineEnd,	// "#" ��������
	};

	TokenList*					m_tokenList;
	PreprocessedFileCacheItem*	m_fileCache;
	DiagnosticsItemSet*			m_diag;
	DirectiveSec				m_seqDirective;
	Token*						m_preproLineHead;	// # �̎��̃g�[�N�����w���Ă���
};

} // namespace Parser
LN_NAMESPACE_END

