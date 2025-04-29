#include <generator.h>

namespace terraformer {

generator::generator() {
	templ = std::string(
		#include <config_template.h>
	);

	

}

void generator::saveAs(const std::string &out_file) {
	float       default_dt = 0.0002;
	std::size_t default_steps = 5000;
	std::string default_grid_id = "default_grid";

	std::pair<std::size_t, std::size_t> default_factory_size = {2386, 828};
	std::pair<std::size_t, std::size_t> default_factory_origin = {0, 0};
	std::pair<float, float> defailt_factory_spacing = {1.6, 1.6};

	std::string default_schema_name = "ElasticMatRectSchema2DRusanov3";
	// TODO: add internal class for fillers information with to_string method
	std::vector<std::string> default_fillers = {
		"[filler]\nname = RectNoReflectFiller\naxis=0\nside=0\n[/filler]",
		"[filler]\nname = RectNoReflectFiller\naxis=0\nside=1\n[/filler]",
		"[filler]\nname = RectNoReflectFiller\naxis=1\nside=0\n[/filler]",
		"[filler]\nname = RectNoReflectFiller\naxis=1\nside=1\n[/filler]"
	};
	// TODO: add internal class for corrector (?) (with to_string method)
	std::vector<std::string> default_correctors = {
		"[corrector]\nname = ForceRectElasticBoundary2D\naxis=0\nside=1\n[/corrector]",
		"[corrector]\nname = PointSourceCorrector2D\ncoords = 1900, 1300, 0.0\ncompression = 1.0\naxis = 0\neps = 2\nsave = source.vtk\ngauss_w = 5\n[impulse]\nname = FileInterpolationImpulse\n[interpolator]\nname = PiceWiceInterpolator1D\nfile = ricker_30.txt\n[/interpolator]\n[/impulse]\n[/corrector]"
	};

	std::vector<std::string> default_params = {"v"};
	std::vector<std::size_t> default_norms  = {1};
	std::size_t default_nsteps_for_save = 100;

	// replacing part
	std::string placeholders[] = {
		"<__DT__>", 
	};
	
	if (std::size_t pos = 0; (pos = templ.find("<__DT__>", 0)) != std::string::npos) {
		templ.replace(pos, sizeof("<__DT__>"), std::to_string(default_dt) + "\n");
	} else { throw std::runtime_error("No <__DT__> in given template"); }

	std::cout << templ << std::endl;

}

}


