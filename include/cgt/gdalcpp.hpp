#pragma once


#include <gdal_priv.h>
#include <gdal_version.h>
#include <ogr_api.h>
#include <ogrsf_frmts.h>

#include <cstdint>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#if defined(_MSC_VER)
# define GDALCPP_EXPORT __declspec(dllexport)
#else
# define GDALCPP_EXPORT __attribute__ ((visibility("default")))
#endif

namespace scially {
    class Layer;
    class Dataset;
    class Feature;

#if GDAL_VERSION_MAJOR >= 2
    using gdal_driver_type = GDALDriver;
    using gdal_dataset_type = GDALDataset;
#else
    using gdal_driver_type = OGRSFDriver;
    using gdal_dataset_type = OGRDataSource;
#endif

    /**
     * Exception thrown for all errors in this class.
     */
    class GDALCPP_EXPORT gdal_error : public std::runtime_error {

        std::string m_driver;
        std::string m_dataset;
        std::string m_layer;
        std::string m_field;
        OGRErr m_error;

    public:

        gdal_error(const std::string& message,
                   OGRErr error,
                   const std::string& driver = "",
                   const std::string& dataset = "",
                   const std::string& layer = "",
                   const std::string& field = "") :
                std::runtime_error(message),
                m_driver(driver),
                m_dataset(dataset),
                m_layer(layer),
                m_field(field),
                m_error(error) {
        }

        const std::string& driver() const noexcept {
            return m_driver;
        }

        const std::string& dataset() const noexcept {
            return m_dataset;
        }

        const std::string& layer() const noexcept {
            return m_layer;
        }

        const std::string& field() const noexcept {
            return m_field;
        }

        OGRErr error() const noexcept {
            return m_error;
        }

    }; // class gdal_error

    namespace detail {

        struct init_wrapper {
#if GDAL_VERSION_MAJOR >= 2
            init_wrapper() noexcept {
                GDALAllRegister();
            }
#else
            init_wrapper() noexcept {
                OGRRegisterAll();
            }
            ~init_wrapper() noexcept {
                OGRCleanupAll();
            }
#endif
        }; // struct init_wrapper

        struct init_library {

            init_library() {
                static init_wrapper iw;
            }

        }; // struct init_library

        class Driver : private init_library {

            gdal_driver_type* m_driver;

        public:

            Driver(const std::string& driver_name) :
                    init_library(),
#if GDAL_VERSION_MAJOR >= 2
                    m_driver(GetGDALDriverManager()->GetDriverByName(driver_name.c_str())) {
#else
                m_driver(OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(driver_name.c_str())) {
#endif
                if (!m_driver) {
                    throw gdal_error{std::string{"unknown driver: '"} + driver_name + "'",
                                     OGRERR_NONE,
                                     driver_name};
                }
            }

            gdal_driver_type& get() const noexcept {
                return *m_driver;
            }

        }; // struct Driver

        struct Options {

            std::vector<std::string> m_options;
            std::unique_ptr<const char*[]> m_ptrs;

            Options(const std::vector<std::string>& options) :
                    m_options(options),
                    m_ptrs(new const char*[options.size() + 1]) {
                std::transform(m_options.begin(), m_options.end(), m_ptrs.get(), [&](const std::string& s) {
                    return s.data();
                });
                m_ptrs[options.size()] = nullptr;
            }

            char** get() const noexcept {
                return const_cast<char**>(m_ptrs.get());
            }

        }; // struct Options

    } // namespace detail

    class SRS {

        OGRSpatialReference m_spatial_reference;

    public:

        SRS() :
                m_spatial_reference() {
            const auto result = m_spatial_reference.SetWellKnownGeogCS("CRS84");
            if (result != OGRERR_NONE) {
                throw gdal_error{std::string{"can not initialize spatial reference system WGS84"},
                                 result};
            }
        }

        explicit SRS(int epsg) :
                m_spatial_reference() {
            const auto result = m_spatial_reference.importFromEPSG(epsg);
            if (result != OGRERR_NONE) {
                throw gdal_error{std::string{"can not initialize spatial reference system for EPSG:"} + std::to_string(epsg),
                                 result};
            }
        }

        explicit SRS(const char* name) :
                m_spatial_reference() {
            const auto result = m_spatial_reference.importFromProj4(name);
            if (result != OGRERR_NONE) {
                throw gdal_error{std::string{"can not initialize spatial reference system '"} + name + "'",
                                 result};
            }
        }

        explicit SRS(const std::string& name) :
                m_spatial_reference() {
            const auto result = m_spatial_reference.importFromProj4(name.c_str());
            if (result != OGRERR_NONE) {
                throw gdal_error{std::string{"can not initialize spatial reference system '"} + name + "'",
                                 result};
            }
        }

        explicit SRS(const OGRSpatialReference& spatial_reference) :
                m_spatial_reference(spatial_reference) {
        }

        OGRSpatialReference& get() noexcept {
            return m_spatial_reference;
        }

        const OGRSpatialReference& get() const noexcept {
            return m_spatial_reference;
        }

    }; // class SRS

    class Feature {
    public:
        friend class Layer;

        OGRGeometry* GetGeometry() {

            return feature_->GetGeometryRef()->clone();
        }
    private:
        struct ogr_feature_deleter {

            void operator()(OGRFeature* feature) {
                OGRFeature::DestroyFeature(feature);
            }

        }; // struct ogr_feature_deleter

        std::unique_ptr<OGRFeature, ogr_feature_deleter> feature_;

        Feature(OGRFeature* feature) : feature_(feature) {

        }
    }; // class Feature


    class Layer {
    public:
        friend class Dataset;

        OGRLayer& get() noexcept {
            return *layer_;
        }

        const OGRLayer& get() const noexcept {
            return *layer_;
        }

        const char* name() const {
            return layer_->GetName();
        }
        OGRSpatialReference* GetSpatialRef () {
            return layer_->GetSpatialRef();
        }

        Feature GetFeature(GIntBig nFID) {
            return Feature(layer_->GetFeature(nFID));
        }

    private:
        OGRLayer* layer_;

        Layer(OGRLayer* layer)
                :layer_(layer){

        }
    }; // class Layer


    class Dataset {

        struct gdal_dataset_deleter {

            void operator()(gdal_dataset_type* ds) {
                GDALClose(ds);
            }

        }; // struct gdal_dataset_deleter

        std::string m_dataset_name;
        std::unique_ptr<gdal_dataset_type, gdal_dataset_deleter> m_dataset;

    public:

        Dataset(const std::string& dataset_name) :
                m_dataset_name(dataset_name),
                m_dataset((gdal_dataset_type*)GDALOpenEx(dataset_name.c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr)) {

            if (!m_dataset) {
                throw gdal_error{std::string{"failed to open dataset '"} + dataset_name + "'",
                                 OGRERR_NONE,
                                 dataset_name};
            }
        }

        ~Dataset() noexcept {
        }


        const std::string& dataset_name() const noexcept {
            return m_dataset_name;
        }

        gdal_dataset_type& get() const noexcept {
            return *m_dataset;
        }

        Layer GetLayer(int iLayer) {
            return Layer(m_dataset->GetLayer(iLayer));
        }

        void ExecuteSQL(const std::string& sql) {
            const auto result = m_dataset->ExecuteSQL(sql.c_str(), nullptr, nullptr);
            if (result) {
                m_dataset->ReleaseResultSet(result);
            }
        }


    }; // class Dataset

}

#undef GDALCPP_EXPORT