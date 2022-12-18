#include <string>
#include <windows.h>

#include "main.h"

struct TextShaping_dll { 
	HMODULE dll;
	FARPROC OrignalBuildOtlCache;
	FARPROC OrignalFreeOtlResources;
	FARPROC OrignalGetOtlFeatureDefs;
	FARPROC OrignalGetOtlGlyphAlternates;
	FARPROC OrignalGetOtlLangSysList;
	FARPROC OrignalGetOtlScriptList;
	FARPROC OrignalGetOtlVersion;
	FARPROC OrignalOtlAssertFailed;
	FARPROC OrignalRepositionOtlSingleGlyph;
	FARPROC OrignalShapingCreateFontCacheData;
	FARPROC OrignalShapingDrawGlyphs;
	FARPROC OrignalShapingGetBreakingProperties;
	FARPROC OrignalShapingGetCombiningOptions;
	FARPROC OrignalShapingGetGlyphPositions;
	FARPROC OrignalShapingGetGlyphs;
	FARPROC OrignalShapingLoadScriptEngine;
	FARPROC OrignalShapingSetAssertFunction;
	FARPROC OrignalShapingSetInvariantAssertFunction;
	FARPROC OrignalSubstituteOtlSingleGlyph;
	FARPROC OrignalValidateLogClust;
} TextShaping;

extern "C"
{
	__declspec(naked) void __stdcall FakeBuildOtlCache() { _asm { jmp[TextShaping.OrignalBuildOtlCache] } }
	__declspec(naked) void __stdcall FakeFreeOtlResources() { _asm { jmp[TextShaping.OrignalFreeOtlResources] } }
	__declspec(naked) void __stdcall FakeGetOtlFeatureDefs() { _asm { jmp[TextShaping.OrignalGetOtlFeatureDefs] } }
	__declspec(naked) void __stdcall FakeGetOtlGlyphAlternates() { _asm { jmp[TextShaping.OrignalGetOtlGlyphAlternates] } }
	__declspec(naked) void __stdcall FakeGetOtlLangSysList() { _asm { jmp[TextShaping.OrignalGetOtlLangSysList] } }
	__declspec(naked) void __stdcall FakeGetOtlScriptList() { _asm { jmp[TextShaping.OrignalGetOtlScriptList] } }
	__declspec(naked) void __stdcall FakeGetOtlVersion() { _asm { jmp[TextShaping.OrignalGetOtlVersion] } }
	__declspec(naked) void __stdcall FakeOtlAssertFailed() { _asm { jmp[TextShaping.OrignalOtlAssertFailed] } }
	__declspec(naked) void __stdcall FakeRepositionOtlSingleGlyph() { _asm { jmp[TextShaping.OrignalRepositionOtlSingleGlyph] } }
	__declspec(naked) void __stdcall FakeShapingCreateFontCacheData() { _asm { jmp[TextShaping.OrignalShapingCreateFontCacheData] } }
	__declspec(naked) void __stdcall FakeShapingDrawGlyphs() { _asm { jmp[TextShaping.OrignalShapingDrawGlyphs] } }
	__declspec(naked) void __stdcall FakeShapingGetBreakingProperties() { _asm { jmp[TextShaping.OrignalShapingGetBreakingProperties] } }
	__declspec(naked) void __stdcall FakeShapingGetCombiningOptions() { _asm { jmp[TextShaping.OrignalShapingGetCombiningOptions] } }
	__declspec(naked) void __stdcall FakeShapingGetGlyphPositions() { _asm { jmp[TextShaping.OrignalShapingGetGlyphPositions] } }
	__declspec(naked) void __stdcall FakeShapingGetGlyphs() { _asm { jmp[TextShaping.OrignalShapingGetGlyphs] } }
	__declspec(naked) void __stdcall FakeShapingLoadScriptEngine() { _asm { jmp[TextShaping.OrignalShapingLoadScriptEngine] } }
	__declspec(naked) void __stdcall FakeShapingSetAssertFunction() { _asm { jmp[TextShaping.OrignalShapingSetAssertFunction] } }
	__declspec(naked) void __stdcall FakeShapingSetInvariantAssertFunction() { _asm { jmp[TextShaping.OrignalShapingSetInvariantAssertFunction] } }
	__declspec(naked) void __stdcall FakeSubstituteOtlSingleGlyph() { _asm { jmp[TextShaping.OrignalSubstituteOtlSingleGlyph] } }
	__declspec(naked) void __stdcall FakeValidateLogClust() { _asm { jmp[TextShaping.OrignalValidateLogClust] } }
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	char path[MAX_PATH];
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
				wchar_t path[MAX_PATH];
				GetSystemDirectory(path, MAX_PATH);
				std::wstring pathname = path + std::wstring(L"\\TextShaping.dll");
				TextShaping.dll = LoadLibrary(pathname.c_str());
				if (TextShaping.dll == nullptr)
				{
					MessageBox(0, L"Cannot load original TextShaping.dll library", L"Proxy", MB_ICONERROR);
					ExitProcess(0);
				}
		TextShaping.OrignalBuildOtlCache = GetProcAddress(TextShaping.dll, "BuildOtlCache");
		TextShaping.OrignalFreeOtlResources = GetProcAddress(TextShaping.dll, "FreeOtlResources");
		TextShaping.OrignalGetOtlFeatureDefs = GetProcAddress(TextShaping.dll, "GetOtlFeatureDefs");
		TextShaping.OrignalGetOtlGlyphAlternates = GetProcAddress(TextShaping.dll, "GetOtlGlyphAlternates");
		TextShaping.OrignalGetOtlLangSysList = GetProcAddress(TextShaping.dll, "GetOtlLangSysList");
		TextShaping.OrignalGetOtlScriptList = GetProcAddress(TextShaping.dll, "GetOtlScriptList");
		TextShaping.OrignalGetOtlVersion = GetProcAddress(TextShaping.dll, "GetOtlVersion");
		TextShaping.OrignalOtlAssertFailed = GetProcAddress(TextShaping.dll, "OtlAssertFailed");
		TextShaping.OrignalRepositionOtlSingleGlyph = GetProcAddress(TextShaping.dll, "RepositionOtlSingleGlyph");
		TextShaping.OrignalShapingCreateFontCacheData = GetProcAddress(TextShaping.dll, "ShapingCreateFontCacheData");
		TextShaping.OrignalShapingDrawGlyphs = GetProcAddress(TextShaping.dll, "ShapingDrawGlyphs");
		TextShaping.OrignalShapingGetBreakingProperties = GetProcAddress(TextShaping.dll, "ShapingGetBreakingProperties");
		TextShaping.OrignalShapingGetCombiningOptions = GetProcAddress(TextShaping.dll, "ShapingGetCombiningOptions");
		TextShaping.OrignalShapingGetGlyphPositions = GetProcAddress(TextShaping.dll, "ShapingGetGlyphPositions");
		TextShaping.OrignalShapingGetGlyphs = GetProcAddress(TextShaping.dll, "ShapingGetGlyphs");
		TextShaping.OrignalShapingLoadScriptEngine = GetProcAddress(TextShaping.dll, "ShapingLoadScriptEngine");
		TextShaping.OrignalShapingSetAssertFunction = GetProcAddress(TextShaping.dll, "ShapingSetAssertFunction");
		TextShaping.OrignalShapingSetInvariantAssertFunction = GetProcAddress(TextShaping.dll, "ShapingSetInvariantAssertFunction");
		TextShaping.OrignalSubstituteOtlSingleGlyph = GetProcAddress(TextShaping.dll, "SubstituteOtlSingleGlyph");
		TextShaping.OrignalValidateLogClust = GetProcAddress(TextShaping.dll, "ValidateLogClust");

		break;
	}
	case DLL_PROCESS_DETACH:
	{
		FreeLibrary(TextShaping.dll);
	}
	break;
	}
	return main(hModule, ul_reason_for_call, lpReserved);
}
