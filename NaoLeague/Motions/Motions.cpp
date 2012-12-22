#include "Motions.h"

#include <string>
#include <iostream>

using namespace std;
using namespace AL;
using boost::property_tree::ptree;

typedef pair<string, string> str_str_pair;

Motions::Motions(string naoqi_ip)
    : m_motProxy(naoqi_ip)
{
    m_motion_filenames.push_back(pair<string, string>("Motions/getupFromBack1.json", "getupFromBack1") );
    m_motion_filenames.push_back(pair<string, string>("Motions/getupFromBack2.json", "getupFromBack2") );

    // parse each motion file
    BOOST_FOREACH (str_str_pair &file_key_pair, m_motion_filenames) {
        parseMotionFile(file_key_pair.first, file_key_pair.second);
    }
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
        vector<float> angles_vector;
        vector<float> times_vector;

        // read the angle and time values as a vector
        BOOST_FOREACH (const ptree::value_type& ele, element.second.get_child("angles")) {
            angles_vector.push_back(ele.second.get<float>(""));
        }
        BOOST_FOREACH (const ptree::value_type& ele, element.second.get_child("times")) {
            times_vector.push_back(ele.second.get<float>(""));
        }

        ip_values.names.push_back( element.second.get<string>("name") );
        ip_values.angles.push_back(angles_vector);
        ip_values.times.push_back(times_vector);
    }

    m_interpolations[key] = ip_values;
}

void Motions::interpolate(string key)
{
    InterpolationValues iv = m_interpolations[key];
    
    // convert the vectors into ALValues
    // TODO: just make them ALValues in the first place
    ALValue al_angles,
            al_times;

    unsigned size = iv.angles.size();
    al_angles.arraySetSize(size);
    al_times.arraySetSize(size);

    for (unsigned i = 0; i < size; ++i) {
        al_angles[i] = ALValue::array(iv.angles[i]);
        al_times[i] = ALValue::array(iv.angles[i]);
    }

    // actuate!
    m_motProxy.angleInterpolation(iv.names, al_angles, al_times, true);
}

void Motions::stiff()
{
    m_motProxy.setStiffnesses("Body", 0.8);
}

void Motions::unstiff()
{
    m_motProxy.setStiffnesses("Body", 0);
}

int main(int argc, char const *argv[])
{
    Motions m( string("0.0.0.0") );

    // some testing
    m.stiff();
    m.unstiff();

    return 0;
}
