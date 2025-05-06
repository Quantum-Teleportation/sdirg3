#ifndef GENERATOR_H
#define GENERATOR_H

#include <factory.h>
#include <fillers.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace terraformer {

template <typename T, typename... Args> inline auto makeVector(Args &&...args) {
	std::vector<T> vec;
	vec.reserve(sizeof...(Args));
	(vec.emplace_back(std::forward<Args>(args)), ...);
	return vec;
}

class generator final {
public:
	generator();
	// method for generating config from template
	void saveAs(const std::string &out_file);

	std::vector<std::unique_ptr<Filler>> fillers_ =
		makeVector<std::unique_ptr<Filler>>(
			std::make_unique<RectNoReflectFiller>(0, 0),
			std::make_unique<RectNoReflectFiller>(0, 1),
			std::make_unique<RectNoReflectFiller>(1, 0),
			std::make_unique<RectNoReflectFiller>(1, 1));

	std::unique_ptr<RectGridFactory> factory_ =
		std::make_unique<RectGridFactory>();

	float dt = 0.0002;
	std::size_t nsteps = 5000;
	std::string grid_id = "default_grid";
	std::string schema_name = "ElasticMatRectSchema2DRusanov3";

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

} // namespace terraformer

#endif
