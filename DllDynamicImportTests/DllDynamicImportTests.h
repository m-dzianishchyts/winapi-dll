#pragma once
#include "CppUnitTest.h"
#include "Windows.h"


typedef BOOL (__cdecl* FIND_REPLACE)(CONST VOID *target, SIZE_T targetSize,
                                     CONST VOID *replacement, SIZE_T replacementSize);

namespace Tests {

	TEST_CLASS(DllDynamicImportTests) {
		static HINSTANCE _dllInstance;
		static FIND_REPLACE _findReplace;

	public:
		TEST_CLASS_INITIALIZE(BeforeAll);

		TEST_CLASS_CLEANUP(AfterAll);

		TEST_METHOD(FindReplace_CharArray);

		TEST_METHOD(FindReplace_ConstCharArray);

		TEST_METHOD(FindReplace_StdString);

		TEST_METHOD(FindReplace_ConstStdString);
	};
}
