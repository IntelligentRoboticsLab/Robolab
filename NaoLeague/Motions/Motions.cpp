#include "Motions.h"

#include <string>
#include <iostream>

using namespace std;
using namespace AL;
using boost::property_tree::ptree;

Motions::Motions(string naoqi_ip)
    //: m_motProxy(naoqi_ip)
{
    parseMotionFile( string("test.js"), string("testing"));
}

Motions::~Motions()
{
}

void Motions::getupFromBack()
{
}

void Motions::parseMotionFile(string filename, string key)
{
    // read the JSON into a property tree
    ptree pt;

    try {
        boost::property_tree::read_json(filename, pt);
    } catch (boost::exception &ex) {
        cout << "File " << filename << " could not be loaded." << endl;
    }

    InterpolationValues ip_values;

    // iterate through each joint contained in the root list, and append its
    // values to the InterpolationValues struct
    BOOST_FOREACH (const ptree::value_type &element, pt.get_child("root")) {
        vector<double> angles_vector;
        vector<double> times_vector;

        // read the angle and time values as a vector
        BOOST_FOREACH (const ptree::value_type& ele, element.second.get_child("angles")) {
            angles_vector.push_back(ele.second.get<double>(""));
        }
        BOOST_FOREACH (const ptree::value_type& ele, element.second.get_child("times")) {
            times_vector.push_back(ele.second.get<double>(""));
        }

        ip_values.names.push_back( element.second.get<string>("name") );
        ip_values.angles.push_back(angles_vector);
        ip_values.times.push_back(times_vector);
    }

    m_interpolations[key] = ip_values;
}

int main(int argc, char const *argv[])
{
    Motions m( string("0.0.0.0") );
    return 0;
}
