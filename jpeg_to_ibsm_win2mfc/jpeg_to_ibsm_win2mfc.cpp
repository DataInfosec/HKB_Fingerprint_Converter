// jpeg_to_ibsm_win2mfc.cpp : Defines the entry point for the console application.
//
#define NOMINMAX
#include "stdafx.h"
#include "jpeg_to_ibsm_win2mfc.h"
#include "IBScanMatcherApi.h"
#include "base64.h"
#include "csv.h"


#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace boost::filesystem;

//#include <atlstr.h>

bool comparator(const std::string &a, const std::string &b) {
	auto a_r = a.substr(0, a.find_last_of("_"));
	auto b_r = b.substr(0, b.find_last_of("_"));
	auto compare_result = atoi(a_r.c_str()) < atoi(b_r.c_str());
	return compare_result;
}

int main()
{
	int matcher_handle;
	std::cout << '\a';
	if (IBSM_OpenMatcher(&matcher_handle) == IBSM_STATUS_OK) {

		std::string filename;
		std::string templateFile;
		std::string base;
		std::string template_path = "C:\\applab\\node_backend\\scripts\\out\\hkb_ibsm_templates\\";
		std::string image_path = "C:\\applab\\node_backend\\scripts\\out\\hkb_fingers\\";
		path p("C:\\applab\\node_backend\\scripts\\out\\hkb_fingers");

		std::vector<path> vec;
		std::vector<std::string> vec_string;
		std::cout << "about to copy" << std::endl;
		std::copy(directory_iterator(p), directory_iterator(), std::back_inserter(vec));
		vec_string.resize(vec.size());

		std::transform(vec.begin(), vec.end(), vec_string.begin(), [](const path& elem) {
			return basename(elem);
		});

		std::sort(vec_string.begin(), vec_string.end(), std::ref(comparator));

		for (std::vector<std::string>::const_iterator p = vec_string.begin(); p != vec_string.end(); p++) {
			filename = *p;
			templateFile = template_path + filename + ".fmr";
			auto filename_int = atoi(filename.c_str());
			auto image_file = image_path + filename + ".jp2";
			std::cout << filename_int << std::endl;
			if (filename_int == 10000) break;
			if (filename_int > 9677) {
				IBSM_ImageData *inputImage = (IBSM_ImageData*)malloc(sizeof(IBSM_ImageData));
				IBSM_Template* out_template = (IBSM_Template*)malloc(sizeof(IBSM_Template));;

				if (IBSM_OpenRasterImage(matcher_handle, image_file.c_str(), inputImage) == IBSM_STATUS_OK) {
					if (IBSM_ExtractTemplate(matcher_handle, *inputImage, out_template) == IBSM_STATUS_OK) {
						IBSM_SaveTemplate(matcher_handle, templateFile.c_str(), *out_template);
						//std::cout << "Saved" << templateFile << std::endl;
					}
					else {
						std::cout << '\a';
						break;
					}
				}
				else {
					std::cout << '\a';
					break;
				}
				free(inputImage);
				free(out_template);
				IBSM_FreeMemory(matcher_handle, inputImage, ENUM_IBSM_MEMBLOCK_IMAGE);
				IBSM_FreeMemory(matcher_handle, out_template, ENUM_IBSM_MEMBLOCK_TEMPLATE);
			}

		}

		//system("node  C:\\applab\\node_backend\\scripts\\db_csv_script.js");
	}

	IBSM_CloseMatcher(matcher_handle);
	std::cout << '\a'; std::cout << '\a'; std::cout << '\a'; std::cout << '\a';
	return 0;

}
