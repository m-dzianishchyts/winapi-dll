#include "pch.h"

#include <algorithm>

#include "CppUnitTest.h"
#include "MemoryFindReplace.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests {

	TEST_CLASS(Tests) {
	public:
		TEST_METHOD(FindReplace_CharArray) {
			char const *actual = "The quick, brown fox jumps over a lazy dog.";
			const char *expected = "The quick, brown === jumps over a lazy dog.";
			std::string target = "fox";
			std::string replacement = std::string(target.length(), '=');
			MemoryFindReplace::FindReplace(target.data(), target.length(), replacement.data(), replacement.length());

			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(FindReplace_ConstCharArray) {
			const char *actual = "Abilities forfeited situation extremely my to he resembled";
			const char *expected = "Abilities forfeited ========= extremely my to he resembled";
			std::string target = "situation";
			std::string replacement = std::string(target.length(), '=');
			MemoryFindReplace::FindReplace(target.data(), target.length(), replacement.data(), replacement.length());

			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(FindReplace_StdString) {
			std::string actual = "On no twenty spring of in esteem spirit likely estate.";
			const char* expected = "On no twenty SPRING of in esteem spirit likely estate.";
			std::string target = "spring";
			std::string replacement = target;
			std::transform(target.begin(), target.end(), replacement.begin(), std::toupper);
			MemoryFindReplace::FindReplace(target.data(), target.length(), replacement.data(), replacement.length());

			Assert::AreEqual(expected, actual.c_str());
		}

		TEST_METHOD(FindReplace_ConstStdString) {
			const std::string actual = "Offices parties lasting outward nothing age few resolve";
			const std::string expected(actual.length(), '=');
			const std::string target = actual;  // NOLINT(performance-unnecessary-copy-initialization)
			const std::string replacement = expected;  // NOLINT(performance-unnecessary-copy-initialization)
			MemoryFindReplace::FindReplace(target.data(), target.length(), replacement.data(), replacement.length());

			Assert::AreEqual(expected.c_str(), actual.c_str());
		}
	};
}
