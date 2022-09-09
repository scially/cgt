#include <cgt/cgtmodel.h>

#include <cgt/cgtexception.h>
#include <tinyxml2/tinyxml2.h>

#include <cassert>
#include <vector>
#include <string>
#include <algorithm>

namespace scially {

    std::vector<std::string> split(std::string_view str, std::string_view delimiters) {
        std::vector<std::string> tokens;

        size_t last_pos = str.find_first_not_of(delimiters, 0);
        size_t pos = str.find_first_of(delimiters, last_pos);
        while (std::string::npos != pos || std::string::npos != last_pos) {
            tokens.emplace_back(str.substr(last_pos, pos - last_pos));
            last_pos = str.find_first_not_of(delimiters, pos);
            pos = str.find_first_of(delimiters, last_pos);
        }

        return tokens;
    }

    void osg_modeldata::load(const std::string& srs, const std::string& srs_origin) {
        parse_srs_origin(srs, srs_origin);
    }

    void osg_modeldata::load_from_file(const std::string& input){
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLError err = doc.LoadFile(input.c_str());
        if (err != tinyxml2::XML_SUCCESS) {
            throw cgt_exception("parse metadata.xml error!");
        }

        tinyxml2::XMLElement* root = doc.RootElement();

        if (strcmp(root->Name(), "ModelMetadata")) {
            throw cgt_exception("Don't find ModelMetaData node in metadata.xml");
        }

        tinyxml2::XMLElement *srs_element = root->FirstChildElement("SRS");
        tinyxml2::XMLElement *srs_origin_element = root->FirstChildElement("SRSOrigin");
        std::string srs = srs_element->GetText();
        std::string srs_origin = srs_origin_element == nullptr ? "0,0,0": srs_origin_element->GetText();
        parse_srs_origin(srs, srs_origin);
    }

    void osg_modeldata::write(const std::string &output){
        tinyxml2::XMLDocument doc;
        doc.Parse("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
        tinyxml2::XMLElement *root = doc.NewElement("ModelMetadata");
        doc.InsertEndChild(root);

        tinyxml2::XMLElement *srs = doc.NewElement("SRS");
        if(srs_ == "EPSG:4326"){
            std::string enu_format = "ENU:" + std::to_string(x_) + "," + std::to_string(y_) + "," + std::to_string(z_);
            srs->SetText(enu_format.c_str());
        }else{
            srs->SetText(srs_.c_str());
        }
        root->InsertFirstChild(srs);

        tinyxml2::XMLElement *srs_origin = doc.NewElement("SRSOrigin");
        if(srs_ == "EPSG:4326") {
            srs_origin->SetText("0,0,0");
        }else{
            std::string xyz_format = std::to_string(x_) + "," + std::to_string(y_) + "," + std::to_string(z_);
            srs_origin->SetText(xyz_format.c_str());
        }
        root->InsertEndChild(srs_origin);

        doc.SaveFile(output.c_str());
    }

    void osg_modeldata::parse_srs_origin(const std::string& srs, const std::string& srs_origin) {
        if (srs.substr(0, 3) == "ENU") {
            srs_ = "EPSG:4326";
            parse_origin(srs.substr(4));
        }else{
            srs_ = srs;
            parse_origin(srs_origin);
        }

    }

    void osg_modeldata::parse_origin(const std::string& srs_origin) {
        std::vector<std::string> srs_split = split(srs_origin, ",");
        std::vector<double> xyz = {0 ,0, 0};
        std::transform(srs_split.begin(), srs_split.end(), xyz.begin(), [](const auto& str) {
            return std::atof(str.c_str());
            });

        x_ = xyz[0];
        y_ = xyz[1];
        z_ = xyz[2];
    }
}
