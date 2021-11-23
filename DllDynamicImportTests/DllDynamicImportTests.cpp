// ReSharper disable CppMemberFunctionMayBeConst
#include "pch.h"
#include <algorithm>

#include "DllDynamicImportTests.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests {
	HINSTANCE DllDynamicImportTests::_dllInstance;
	FIND_REPLACE DllDynamicImportTests::_findReplace;

	void DllDynamicImportTests::BeforeAll() {
		_dllInstance = LoadLibraryW(L"MemoryFindReplace");
		if (_dllInstance == NULL) {
			Logger::WriteMessage(L"DLL was not found!");
			Logger::WriteMessage(std::to_string(GetLastError()).c_str());
			return;
		}
		_findReplace = reinterpret_cast<FIND_REPLACE>(GetProcAddress(_dllInstance, "FindReplace"));
	}

	void DllDynamicImportTests::AfterAll() {
		if (_dllInstance != NULL) {
			FreeLibrary(_dllInstance);
		}
	}

	void DllDynamicImportTests::FindReplace_CharArray() {
		char const *actual = "The quick, brown fox jumps over a lazy dog.";
		const char *expected = "The quick, brown === jumps over a lazy dog.";
		std::string target = "fox";
		std::string replacement = std::string(target.length(), '=');
		_findReplace(target.data(), target.length(), replacement.data(), replacement.length());

		Assert::AreEqual(expected, actual);
	}

	void DllDynamicImportTests::FindReplace_ConstCharArray() {
		const char* actual = "Abilities forfeited situation extremely my to he resembled";
		const char* expected = "Abilities forfeited ========= extremely my to he resembled";
		std::string target = "situation";
		std::string replacement = std::string(target.length(), '=');
		_findReplace(target.data(), target.length(), replacement.data(), replacement.length());

		Assert::AreEqual(expected, actual);
	}

	void DllDynamicImportTests::FindReplace_StdString() {
		std::string actual = "On no twenty spring of in esteem spirit likely estate.";
		const char* expected = "On no twenty SPRING of in esteem spirit likely estate.";
		std::string target = "spring";
		std::string replacement = target;
		std::transform(target.begin(), target.end(), replacement.begin(), std::toupper);
		_findReplace(target.data(), target.length(), replacement.data(), replacement.length());

		Assert::AreEqual(expected, actual.c_str());
	}

	void DllDynamicImportTests::FindReplace_ConstStdString() {
		const std::string actual = "Offices parties lasting outward nothing age few resolve";
		const std::string expected(actual.length(), '=');
		const std::string target = actual;        // NOLINT(performance-unnecessary-copy-initialization)
		const std::string replacement = expected; // NOLINT(performance-unnecessary-copy-initialization)
		_findReplace(target.data(), target.length(), replacement.data(), replacement.length());

		Assert::AreEqual(expected.c_str(), actual.c_str());
	}
}
