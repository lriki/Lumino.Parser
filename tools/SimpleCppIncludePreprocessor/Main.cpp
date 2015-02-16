
#include "../../external/Lumino.Core/include/LuminoCore.h"
#include "../../include/Lumino/Parser/ErrorInfo.h"
#include "../../include/Lumino/Parser/Tools/SimpleCppIncludePreprocessor.h"
using namespace Lumino;

void PrintHelp()
{
	printf("SimpleCppIncludePreprocessor filepath");
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		return 1;
	}

	Parser::ErrorManager errorManager;


	Parser::SimpleCppIncludePreprocessor<char> prepro;
	prepro.Analyze(PathName(argv[1]), &errorManager);

	FileStream stream(_T("out.h"), FileMode_Create, FileAccess_Write);
	prepro.GetTokenList()->DumpText(&stream);

	return 0;
}

