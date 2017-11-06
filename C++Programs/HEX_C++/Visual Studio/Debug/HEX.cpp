//
//
// Changes all HEX text in a file to ASCII text.
// Example:
// HEX: 2b5341434b3a47544842442c3046303130362c3734394224
// Will change to: +SACK:GTHBD,0F0106,749B$
//
// Look at the included batch and sed files in folder COMPLETE 
// to see how with the help of sed and batch this program can be usefull.

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
	std::ifstream in("Input.file"); // Input file
	while (std::getline(in, line))
	{
		text += line;
	}
	const char* st = text.c_str();
	int length = strlen(st);
	int i;
	char buf = 0;
	std::ofstream out("Output.file", std::ios::out); // Output file
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