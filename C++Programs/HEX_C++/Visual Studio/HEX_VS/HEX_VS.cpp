#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
int hex_to_int(char c)
{
	if (c >= 97)
		c = c - 32;
	int first = c / 16 - 3;
	int second = c % 16;
	int result = first * 10 + second;
	if (result > 9) result--;
	return result;
}
int hex_to_ascii(char c, char d) {
	int high = hex_to_int(c) * 16;
	int low = hex_to_int(d);
	return high + low;
}
int main() {
	std::string line, text;
	std::ifstream in("Input.file");
	while (std::getline(in, line))
	{
		text += line;
	}
	const char* st = text.c_str();
	int length = strlen(st);
	int i;
	char buf = 0;
	std::ofstream out("Output.file", std::ios::out);
	for (i = 0; i < length; i++) {
		if (i % 2 != 0) {
			out << static_cast<char>(hex_to_ascii(buf, st[i]));
		}
		else {
			buf = st[i];
		}
	}
	out.close();
	system("pause");
}