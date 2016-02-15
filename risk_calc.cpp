
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/graph/undirected_dfs.hpp>
#include <boost/foreach.hpp>
#include <boost/progress.hpp>

#include "util/utility.h"
#include "util/zip_range.h"



int main(int argc, char **argv)
{
    
    
    /**********************************/
    /*        Program options         */
    /**********************************/
    // Inputs
    std::string map1_loc("no_file");
    std::string map2_loc("no_file");
    std::string map3_loc("no_file");
    std::string map4_loc("no_file");
    int ari_map1(0);
    int ari_map2(0);
    int ari_map3(0);
    int ari_map4(0);
   //Outputs
    std::string risk_raster_file("out-risk-raster.tif");
    std::string out_list_file("loss-by-class-list.txt");
    
    namespace prog_opt = boost::program_options;
    prog_opt::options_description desc("Allowed options");
    desc.add_options()
    ("help,h", "produce help message")
    ("map1,1", prog_opt::value<std::string>(&map1_loc), "path of a gdal capatible loss file")
    ("a1", prog_opt::value<int>(&ari_map1), "the 1 in X year flood - specify X corresponding to map1")
    ("map2,2", prog_opt::value<std::string>(&map2_loc), "path of a gdal capatible loss file")
    ("a2", prog_opt::value<int>(&ari_map2), "the 1 in X year flood - specify X corresponding to map2")
    ("map3,3", prog_opt::value<std::string>(&map3_loc), "path of a gdal capatible loss file")
    ("a3", prog_opt::value<int>(&ari_map3), "the 1 in X year flood - specify X corresponding to map3")
    ("map4,4", prog_opt::value<std::string>(&map4_loc), "path of a gdal capatible loss file")
    ("a4", prog_opt::value<int>(&ari_map4), "the 1 in X year flood - specify X corresponding to map4")
    ("out-raster,r", prog_opt::value<std::string>(&risk_raster_file), "path where output tif raster of risk (annual expected loss) is saved")
    ("out-list,o", prog_opt::value<std::string>(&out_list_file), "path where the output text file listing net risk is saved");
    
    prog_opt::variables_map vm;
    prog_opt::store(prog_opt::parse_command_line(argc, argv, desc), vm);
    prog_opt::notify(vm);
    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }
    
    namespace fs = boost::filesystem;
    
    fs::path map1_path(map1_loc);
    fs::path map2_path(map2_loc);
    fs::path map3_path(map3_loc);
    fs::path map4_path(map4_loc);
    
    
    
    // Check file exists
    if (!fs::exists(map1_path))
    {
        std::stringstream ss;
        ss << map1_loc << " does not exist";
        throw std::runtime_error(ss.str());
        return (EXIT_FAILURE);
    }
    
    if (!fs::exists(map2_path))
    {
        std::stringstream ss;
        ss << map2_loc << " does not exist";
        throw std::runtime_error(ss.str());
        return (EXIT_FAILURE);
    }
    
    if (!fs::exists(map3_path))
    {
        std::stringstream ss;
        ss << map3_loc << " does not exist";
        throw std::runtime_error(ss.str());
        return (EXIT_FAILURE);
    }
    
    if (!fs::exists(map4_path))
    {
        std::stringstream ss;
        ss << map4_loc << " does not exist";
        throw std::runtime_error(ss.str());
        return (EXIT_FAILURE);
    }

   
    // open a raster data set
    auto map1 = raster_util::open_gdal_raster<double>(map1_path, GA_ReadOnly);
    auto map2 = raster_util::open_gdal_raster<double>(map2_path, GA_ReadOnly);
    auto map3 = raster_util::open_gdal_raster<double>(map3_path, GA_ReadOnly);
    auto map4 = raster_util::open_gdal_raster<double>(map4_path, GA_ReadOnly);
    
    // create a raster data set, with same dimensions as map1
    auto risk_raster = raster_util::create_gdal_raster_from_model<double>(risk_raster_file, map1);
    risk_raster.setNoDataValue(0.0);
    double net_risk(0);
    
    double freq_1 = 1 / (double) ari_map1;
    double freq_2 = 1 / (double) ari_map2;
    double freq_3 = 1 / (double) ari_map3;
    double freq_4 = 1 / (double) ari_map4;
    
    double risk_i;
    
    auto zip = raster_util::make_zip_range(std::ref(map1), std::ref(map2), std::ref(map3), std::ref(map4), std::ref(risk_raster));
    for (auto i : zip)
    {
        const double & loss1 = std::get<0>(i);
        const double & loss2 = std::get<1>(i);
        const double & loss3 = std::get<2>(i);
        const double & loss4 = std::get<3>(i);
        auto & risk = std::get<4>(i);
        
        risk_i = ((loss1 + loss2) * std::abs(freq_1 - freq_2) + (loss2 + loss3) * std::abs(freq_3-freq_2) + (loss3+loss4) * std::abs(freq_4-freq_3)) / 2;
        risk = risk_i;
        net_risk += risk_i;
    }
    
    
    std::ofstream list_fs(out_list_file);
    if (list_fs.is_open())
    {
        list_fs << "total_risk\t" << net_risk << "\n";
    }
    
}