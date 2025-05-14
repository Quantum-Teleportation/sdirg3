#ifndef GENERATOR_H
#define GENERATOR_H

#include <factory.h>
#include <fillers.h>
#include <fstream>
#include <initials.h>
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
			std::unique_ptr<RectNoReflectFiller>(
				CREATE_CLASS(Filler, RectNoReflectFiller)->Init(0, 0)),
			std::unique_ptr<RectNoReflectFiller>(
				CREATE_CLASS(Filler, RectNoReflectFiller)->Init(0, 1)),
			std::unique_ptr<RectNoReflectFiller>(
				CREATE_CLASS(Filler, RectNoReflectFiller)->Init(1, 0)),
			std::unique_ptr<RectNoReflectFiller>(
				CREATE_CLASS(Filler, RectNoReflectFiller)->Init(1, 1)));

	std::unique_ptr<RectGridFactory> factory_{
		CREATE_CLASS(Factory, RectGridFactory)};

	std::vector<std::unique_ptr<Initial>> initials_ =
		makeVector<std::unique_ptr<Initial>>(
			std::unique_ptr<StructuredFileLoader>(
				CREATE_CLASS(Initial, StructuredFileLoader)
					->Init("c1", "main_vp.bin", true, 1)),
			std::unique_ptr<StructuredFileLoader>(
				CREATE_CLASS(Initial, StructuredFileLoader)
					->Init("c2", "main_vs.bin", true, 2)),
			std::unique_ptr<StructuredFileLoader>(
				CREATE_CLASS(Initial, StructuredFileLoader)
					->Init("rho", "main_rho.bin", true, 3)));

	float dt = 0.0002;
	std::size_t nsteps = 5000;
	std::string grid_id = "default_grid";
	std::string schema_name = "ElasticMatRectSchema2DRusanov3";

	// savers default values
	std::string params = "v";
	std::size_t norms = 1;

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
