#ifndef MOTIONS_H
#define MOTIONS_H 

#include <string>
#include <vector>
#include <map>
#include <alproxies/almotionproxy.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>


using namespace std;
using boost::property_tree::ptree;

class Motions
{
    public:
        /**
         * Constructs an instance of the class, connecting to a specific Naoqi
         * instance.
         *
         * @param naoqi_ip The IP address of the desired Naoqi instance
         */
        Motions(string naoqi_ip);
        ~Motions();

        // standup motions
        void getupFromBack();

        // stiffness functions
        void stiff();
        void unstiff();

    private:
        /**
         * Struct to hold the information for a motion interpolation.
         */
        struct InterpolationValues {
            vector<string> names;
            vector<vector<float> > angles;
            vector<vector<float> > times;
        };

        /**
         * Holds interpolationvalues to be used by motions.
         */
        map<string, InterpolationValues> m_interpolations;

        /**
         * Holds the filenames and desired keys of the json files containg moves
         */
        vector<pair<string, string> > m_motion_filenames;
        
        AL::ALMotionProxy m_motProxy;

        /**
         * Parses a JSON file containg a motion interpolation, and inserts it
         * into m_interpolations.
         *
         * @param path The path of the JSON file.
         * @param key The key to use for insertion into m_interpolations.
         */
        void parseMotionFile(string filename, string key);

        /**
         * Interpolates a motion using the values retrieved from
         * m_interpolations.
         *
         * @param key The key belonging to the interpolation values in
         * m_interpolations.
         */
        void interpolate(string key);
};

#endif
