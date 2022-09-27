#include <cgt/cgtmodel.h>

#include <cgt/cgtexception.h>
#include <tinyxml2/tinyxml2.h>

#include <cassert>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>

namespace scially {
    namespace fs = std::filesystem;

    std::vector<std::string> split(const std::string& str, const std::string& delimiters) {
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

    void osg_modeldata::load(const std::string &srs, const std::string &srs_origin) {
        srs_ = srs;
        origin_ = parse_origin(srs_origin);
    }

    void osg_modeldata::load_from_dir(const std::string &input) {
        std::string metadata_location = (fs::path(input) / "metadata.xml").string();
        load_from_file(metadata_location);
    }

    void osg_modeldata::load_from_file(const std::string &input) {
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLError err = doc.LoadFile(input.c_str());
        if (err != tinyxml2::XML_SUCCESS) {
            throw cgt_exception("could not parse metadata.xml!");
        }

        tinyxml2::XMLElement *root = doc.RootElement();

        if (strcmp(root->Name(), "ModelMetadata")) {
            throw cgt_exception("could not find ModelMetaData node in metadata.xml");
        }

        tinyxml2::XMLElement *srs_element = root->FirstChildElement("SRS");
        tinyxml2::XMLElement *srs_origin_element = root->FirstChildElement("SRSOrigin");
        const std::string srs = srs_element->GetText();
        const std::string srs_origin = srs_origin_element == nullptr ? "0,0,0": srs_origin_element->GetText();
        load(srs, srs_origin);
    }

    void osg_modeldata::write(const std::string &output){
        tinyxml2::XMLDocument doc;
        doc.Parse("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
        tinyxml2::XMLElement *root = doc.NewElement("ModelMetadata");
        doc.InsertEndChild(root);

        tinyxml2::XMLElement *srs = doc.NewElement("SRS");
        srs->SetText(srs_.c_str());
        root->InsertFirstChild(srs);

        tinyxml2::XMLElement *srs_origin = doc.NewElement("SRSOrigin");
        const std::string xyz_format = std::to_string(origin_.x())
                + "," + std::to_string(origin_.y())
                + "," + std::to_string(origin_.z());
        srs_origin->SetText(xyz_format.c_str());

        root->InsertEndChild(srs_origin);
        doc.SaveFile(output.c_str());
    }

    osg::Vec3 osg_modeldata::parse_origin(const std::string& srs_origin) const noexcept{
        std::vector<std::string> srs_split = split(srs_origin, ",");
        std::vector<double> xyz = {0 ,0, 0};
        std::transform(srs_split.begin(), srs_split.end(), xyz.begin(), [](const auto& str) {
            return std::atof(str.c_str());
            });

        return osg::Vec3(xyz[0], xyz[1], xyz[2]);
    }
}
