// ReSharper disable CppTooWideScope
// ReSharper disable CppClangTidyConcurrencyMtUnsafe
#include <iostream>

int main() {
	const char *value = "Hello World!\n";
	while (true) {
		std::cout << value;
		system("pause");
	}
}
