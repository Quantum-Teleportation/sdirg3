#include <generator.h>
#include <string_view>
#include <utility>

namespace terraformer {

generator::generator() {
	templ = std::string(
#include <config_template.templ>
	);
}

void generator::saveAs(const std::string &out_file) {
	// TODO: add internal class for fillers information with to_string method
	auto replacer = [this](std::string_view placeholder,
						   std::string_view subs) {
		if (std::size_t pos = 0;
			(pos = templ.find(placeholder, 0)) != std::string::npos) {
			templ.replace(pos, placeholder.size(), subs);
		} else {
			std::stringstream ss;
			ss << "No \"" << placeholder << "\" in given template";
			throw std::runtime_error(ss.str());
		}
	};

	// dt
	do {
		replacer("<__DT__>", std::to_string(dt));
	} while (0);

	// steps count
	do {
		replacer("<__STEPS__>", std::to_string(nsteps));
	} while (0);

	// grid_id
	do {
		replacer("<__GRID_ID__>", grid_id);
	} while (0);

	// schema_name
	do {
		replacer("<__SCHEMA_NAME__>", schema_name);
	} while (0);

	// fillers
	do {
		std::string fillerOut = "";
		for (const auto &f : fillers_) {
			fillerOut += f->Serialize();
		}
		replacer("<__FILLERS__>", fillerOut);

	} while (0);

	// factory
	do {
		replacer("<__FACTORY__>", factory_->Serialize());
	} while (0);


	// initials
	do {
		std::string initialsOut = "";
		for (const auto &i : initials_) {
			initialsOut += i->Serialize();
		}
		replacer("<__INITIALS__>", initialsOut);
	} while (0);


	// TODO: add internal class for corrector (?) (with to_string method)
	std::vector<std::string> default_correctors = {
		"[corrector]\nname = "
		"ForceRectElasticBoundary2D\naxis=0\nside=1\n[/corrector]",
		"[corrector]\nname = PointSourceCorrector2D\ncoords = 1900, 1300, "
		"0.0\ncompression = 1.0\naxis = 0\neps = 2\nsave = source.vtk\ngauss_w "
		"= 5\n[impulse]\nname = FileInterpolationImpulse\n[interpolator]\nname "
		"= PiceWiceInterpolator1D\nfile = "
		"ricker_30.txt\n[/interpolator]\n[/impulse]\n[/corrector]"};

	std::vector<std::string> default_params = {"v"};
	std::vector<std::size_t> default_norms = {1};
	[[maybe_unused]] std::size_t default_nsteps_for_save = 100;

	std::cout << templ << std::endl;

	std::ofstream file2write(out_file, std::ios::out);
	file2write << templ;
	file2write.close();
}

} // namespace terraformer
