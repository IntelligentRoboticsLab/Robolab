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

    private:
        /**
         * Struct to hold the information for a motion interpolation.
         */
        struct InterpolationValues {
            vector<string> names;
            vector<vector<double> > angles;
            vector<vector<double> > times;
        };

        /**
         * Holds interpolationvalues to be used by motions.
         */
        map<string, InterpolationValues> m_interpolations;
        
        //AL::ALMotionProxy m_motProxy;

        /**
         * Parses a JSON file containg a motion interpolation, and inserts it
         * into m_interpolations.
         *
         * @param path The path of the JSON file.
         * @param key The key to use for insertion into m_interpolations.
         */
        void parseMotionFile(string filename, string key);
};

#endif
