#ifndef GENERATOR_H
#define GENERATOR_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <utility>

namespace terraformer {

class generator final {
public:
	generator();
	// method for generating config from template
	void saveAs(const std::string &out_file);

	template <typename T>
	// https://godbolt.org/z/ErejKM31E
	struct Field final {
		const char *placeholder;
		T value;

		Field(const char *ph, const T &value);
		auto to_string(T &i) -> decltype(std::to_string(i), void()) {
			std::cout << std::to_string(i);
		}
	};
private:
	std::string templ;
};

}

#endif
