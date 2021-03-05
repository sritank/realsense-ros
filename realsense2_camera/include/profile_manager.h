#include <librealsense2/rs.hpp>
#include <sensor_params.h>

#define STREAM_NAME(sip) (static_cast<std::ostringstream&&>(std::ostringstream() << create_graph_resource_name(rs2_stream_to_string(sip.first)) << ((sip.second>0) ? std::to_string(sip.second) : ""))).str()

using namespace rs2;
namespace realsense2_camera
{
    typedef std::pair<rs2_stream, int> stream_index_pair;

    class ProfilesManager
    {
        public:
            ProfilesManager(std::shared_ptr<Parameters> parameters);
            virtual bool isWantedProfile(const rs2::stream_profile& profile) = 0;
            virtual void registerProfileParameters(std::vector<stream_profile> all_profiles, std::function<void()> update_sensor_func) = 0;
            bool isTypeExist();
            template<class T>
            void registerSensorUpdateParam(std::string template_name, 
                                           std::set<stream_index_pair> unique_sips, 
                                           std::map<stream_index_pair, T>& params, 
                                           T value, 
                                           std::function<void()> update_sensor_func);
            void addWantedProfiles(std::vector<rs2::stream_profile>& wanted_profiles);

        protected:
            rclcpp::Logger _logger;
            SensorParams _params;
            std::map<stream_index_pair, bool> _enabled_profiles;
            std::vector<rs2::stream_profile> _all_profiles;
    };

    class VideoProfilesManager : public ProfilesManager
    {
        public:
            VideoProfilesManager(std::shared_ptr<Parameters> parameters, const std::string& module_name);
            bool isWantedProfile(const rs2::stream_profile& profile) override;
            void registerProfileParameters(std::vector<stream_profile> all_profiles, std::function<void()> update_sensor_func) override;
            bool isTypeExist();
            int getHeight() {return _height;};
            int getWidth() {return _width;};
            double getFPS() {return _fps;};

        private:
            void registerVideoSensorParams();

        private:
            std::string _module_name;
            std::map<rs2_stream, rs2_format>  _allowed_formats;
            double      _fps;
            int _width, _height;
            bool _is_profile_exist;
    };

    class MotionProfilesManager : public ProfilesManager
    {
        public:
            using ProfilesManager::ProfilesManager;
            bool isWantedProfile(const rs2::stream_profile& profile) override;
            void registerProfileParameters(std::vector<stream_profile> all_profiles, std::function<void()> update_sensor_func) override;
            bool isTypeExist();

        protected:
            std::map<stream_index_pair, double> _fps;
    };

    class PoseProfilesManager : public MotionProfilesManager
    {
        public:
            using MotionProfilesManager::MotionProfilesManager;
            void registerProfileParameters(std::vector<stream_profile> all_profiles, std::function<void()> update_sensor_func) override;
    };

}