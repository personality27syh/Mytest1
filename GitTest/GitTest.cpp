#include <iostream>
#include <string>

void f1() {
	int a = 1;
	int& ref = a;
	ref = 2;
	std::cout << "ref: " << ref << " " << "a: " << a << std::endl;

	int* p = &ref;
	*p = 3;
	std::cout << "ref: " << ref << " " << "a: " << a << std::endl;

}

int f2(int a) {
	int b = 0, c = 1;
	b++; c++;
	return int(a + pow(double(b), 2) + c);
}

void f3() {
	int n = 0;
	std::cin >> n;
	std::string* p = new std::string[n];

	for (int i = 0; i < n; ++i) {
		std::cin >> p[i];
	}

	std::string longstr, shortstr;
	int strlen;
	for (int j = 0; j < n; ++j) {
		std::cout << p[j] << std::endl;

		if (p[j].length() > longstr.length())
			longstr = p[j];
		else if (p[j].length() < shortstr.length())
			shortstr = p[j];
		strlen += p[j].length();
	}

	std::cout << strlen / n << std::endl;
	delete[] p;
}

int main()
{
    std::cout << "Git Test!\n";

	std::cout << "Git add.\n";

	f1();

	for (int i = 0; i < 3; ++i) {
		std::cout << f2(i) << std::endl;
	}

	f3();

	printf("hello\n");

	return 0;
}

