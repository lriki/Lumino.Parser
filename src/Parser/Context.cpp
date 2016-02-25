
#include "Internal.h"
#include "Frontend\Cpp\CppLexer.h"
#include "DiagnosticsManager.h"
#include "UnitFile.h"
#include "Context.h"

LN_NAMESPACE_BEGIN
namespace parser
{

//=============================================================================
// Context
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ResultState Context::LookupPreprocessedIncludeFile(const TokenPathName& basePath, const TokenPathName& filePath, const Array<TokenPathName>* additionalIncludePaths, const MacroMapContainer& parentMacroMap, DiagnosticsItemSet* parentDiag, UnitFile** outFile)
{
	LN_CHECK_ARGS_RETURNV(additionalIncludePaths != nullptr, ResultState::Error);
	//LN_CHECK_ARGS_RETURNV(macroMap != nullptr, ResultState::Error);

	uint64_t key = MakeCacheFileKey(additionalIncludePaths, parentMacroMap.GetConst());

	auto itr = m_codeFileMap.find(key);
	if (itr != m_codeFileMap.end())
	{
		*outFile = itr->second;
		return ResultState::Success;	// TODO: CodeFile �ɉ�͌��ʂ�����������
	}
	else
	{
		PathName includeFilePath;
		// ��΃p�X�Ȃ炻�̂܂܎g��
		if (filePath.IsAbsolute())
		{
			includeFilePath = filePath;
		}
		// �e�R�[�h�t�@�C������̑��΃p�X�ɑ��݂��邩�H
		if (basePath.ExistsFileInDirectory(filePath))
		{
			includeFilePath = PathName(basePath, filePath);
		}
		// -I �Ŏw�肳�ꂽ�t�H���_�ɑ��݂��邩�H
		else
		{
			for (auto& path : *additionalIncludePaths)
			{
				if (path.ExistsFileInDirectory(filePath))
				{
					includeFilePath = PathName(path, filePath);
					break;
				}
			}
		}

		// Error: include �t�@�C����������Ȃ�����
		if (includeFilePath.IsEmpty())
		{
			parentDiag->Report(DiagnosticsCode::Preprocessor_IncludeFileNotFount, includeFilePath.GetString());
			return ResultState::Error;
		}

		// �t�@�C�����e����x�S�ēǂݍ���
		// TODO: �����R�[�h
		FileStream file(includeFilePath, FileOpenMode::Read);
		ByteBuffer buffer(file.GetLength(), false);
		file.Read(buffer.GetData(), buffer.GetSize());

		auto codeFile = RefPtr<UnitFile>::MakeRef();
		codeFile->Initialize(includeFilePath);

		// �g�[�N������
		CppLexer lex;
		TokenListPtr tokens = lex.Tokenize(buffer, parentDiag);	// TODO: �G���[
		codeFile->SetTokenList(tokens);				// �g�[�N���X�g�͑S���o���Ă��� (define �̒u���v�f�ƃG���e�B�e�B�Q�Ƃ����o����������Ă��ǂ�)

		// �v���v���Z�X
		Preprocessor preprocessor;
		LN_RESULT_CALL(preprocessor.BuildPreprocessedTokenList(this, tokens, codeFile, additionalIncludePaths, parentMacroMap, parentDiag));
		//codeFile->GetMacroMap()->SetFreeze(true);	// �m��B���Ƃ͕ω����Ȃ�

		m_codeFileMap.insert({ key, codeFile });
		*outFile = codeFile;
		return ResultState::Success;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint64_t Context::MakeCacheFileKey(const Array<TokenPathName>* additionalIncludePaths, const MacroMap* macroMap)
{
	uint64_t key = 0;
	for (auto& path : *additionalIncludePaths)
	{
		key += Hash::CalcHash(path.c_str(), path.GetLength());
	}
	key += macroMap->GetHashCode();
	return key;
}

} // namespace Parser
LN_NAMESPACE_END

