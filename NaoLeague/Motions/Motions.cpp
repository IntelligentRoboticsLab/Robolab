#include "Motions.h"

#include <string>
#include <iostream>

#include <unistd.h>
#include <assert.h>

using namespace std;
using namespace AL;
using boost::property_tree::ptree;

typedef pair<string, string> str_str_pair;

Motions::Motions(const char *naoqi_ip)
    : m_motProxy(naoqi_ip)
{
    // TODO: figure out a better organisation for loading the files, this is
    // terrible. Maybe just load every file in the folder and use the filename
    // as key.
    m_motion_filenames.push_back(pair<string, string>("Motions/backToStand0.json", "backToStand0") );
    m_motion_filenames.push_back(pair<string, string>("Motions/backToStand1.json", "backToStand1") );
    m_motion_filenames.push_back(pair<string, string>("Motions/backToStand2.json", "backToStand2") );
    m_motion_filenames.push_back(pair<string, string>("Motions/backToStand3.json", "backToStand3") );
    m_motion_filenames.push_back(pair<string, string>("Motions/backToStand4.json", "backToStand4") );
    m_motion_filenames.push_back(pair<string, string>("Motions/backToStand5.json", "backToStand5") );
    m_motion_filenames.push_back(pair<string, string>("Motions/backToStand6.json", "backToStand6") );
    m_motion_filenames.push_back(pair<string, string>("Motions/backToStand7.json", "backToStand7") );
    //m_motion_filenames.push_back(pair<string, string>("/home/maarten/Projects/Robolab/NaoLeague/Motions/Motions/bellyToStand0.json", "bellyToStand0") );

    // parse each motion file
    BOOST_FOREACH (str_str_pair &file_key_pair, m_motion_filenames) {
        parseMotionFile(file_key_pair.first, file_key_pair.second);
    }
}

Motions::~Motions()
{
}

// Stand up from the back
void Motions::backToStand()
{
    // lie down on back, move arms towards pushing position and legs upwards
    Motions::setAngles(2, 0.4, "HeadPitch", "HeadYaw", -0.4, 0.0);
    interpolate("backToStand0");
    Motions::setAngles(7, 0.3, "LHipYawPitch", "RKneePitch", "LKneePitch", "RHipRoll", "LHipRoll", "RAnkleRoll", "LAnkleRoll", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    Motions::setAngles(4, 0.3, "LHipPitch", "LAnklePitch", "RHipPitch", "RAnklePitch", -1.5, 0.8, -1.5, 0.8);
    usleep(1000000);

    // move legs down, arms down to push
    Motions::setAngles(4, 0.9, "LShoulderPitch", "RShoulderPitch", "RHipPitch", "LHipPitch", 2.0, 2.0, -0.7, -0.7);
    usleep(100000);

    // reset legs
    Motions::setAngles(2, 0.3, "RHipPitch", "LHipPitch", -1.5, -1.5);
    usleep(200000);

    // push up with arms
    Motions::setAngles(2, 0.5, "RShoulderRoll", "LShoulderRoll", -0.25, 0.25);
    Motions::setAngles(2, 0.5, "LElbowRoll", "RElbowRoll", 0.0, 0.0);
    usleep(400000);

    // twist legs around to sit with legs wide
    interpolate("backToStand2");

    // move one arm backwards, one arm upwards, move legs towards body
    Motions::setAngles(2, 0.4, "HeadPitch", "HeadYaw", 0.5, 0.0);
    interpolate("backToStand3");

    // move legs further towards body
    interpolate("backToStand4");

    // Lift arm from ground, move right leg towards body
    interpolate("backToStand5");

    // lift right leg further towards left
    interpolate("backToStand6");

    // move legs closer to eachother (stance)
    interpolate("backToStand7");
}

// Stand up from belly
void Motions::bellyToStand()
{
    interpolateWithBezier("bellyToStand0");
}

void Motions::setAngles(int numJoints, double speed, ...)
{
    // the varargs list contains first the joint names, then the joint angles,
    // so its length is 2*numJoints
    va_list args;
    int list_length = 2 * numJoints;
    va_start(args, speed);
    ALValue names, angles;

    // first put the joint names in an array
    for (int i = 0; i < numJoints; ++i) {
        names.arrayPush( va_arg(args, char*) );
    }

    // and now the angles
    for (int i = numJoints; i < list_length; ++i) {
        angles.arrayPush( va_arg(args, double) );
    }

    va_end(args);

    /*
    for (int i = 0; i < names.getSize(); ++i) {
        cout << "(" << names[i] << ", " << angles[i] << "), ";
    }
    */

    cout << endl << endl;
    m_motProxy.setAngles(names, angles, speed);
}

void Motions::parseMotionFile(string filename, string key)
{
    // read the JSON into a property tree
    ptree pt;

    try {
        boost::property_tree::read_json(filename, pt);
    } catch (boost::exception &ex) {
        cout << "File " << filename << " could not be loaded." << endl;
        exit(0);
    }

    InterpolationValues ip_values;

    // iterate through each joint contained in the root list, and append its
    // values to the InterpolationValues struct
    BOOST_FOREACH (const ptree::value_type &element, pt.get_child("root")) {
        ALValue angles_value;
        ALValue times_value;

        // read the angle and time values as a vector
        BOOST_FOREACH (const ptree::value_type& ele, element.second.get_child("angles")) {
            angles_value.arrayPush(ele.second.get<float>(""));
        }
        BOOST_FOREACH (const ptree::value_type& ele, element.second.get_child("times")) {
            times_value.arrayPush(ele.second.get<float>(""));
        }

        ip_values.names.arrayPush( element.second.get<string>("name") );
        ip_values.angles.arrayPush(angles_value);
        ip_values.times.arrayPush(times_value);
    }

    m_interpolations[key] = ip_values;
}

void Motions::interpolate(string key)
{
    InterpolationValues iv = m_interpolations[key];
    
    m_motProxy.angleInterpolation(iv.names, iv.angles, iv.times, true);
}

void Motions::interpolateWithBezier(string key)
{
    InterpolationValues iv = m_interpolations[key];
    
    m_motProxy.angleInterpolationBezier(iv.names, iv.times, iv.angles);
}

void Motions::stiff()
{
    ALValue names,
            stiffnesses;

    names = ALValue::array("RArm", "LArm", "LLeg", "RLeg", "Head");
    stiffnesses.arrayPush(0.6);
    stiffnesses.arrayPush(0.6);
    stiffnesses.arrayPush(0.6);
    stiffnesses.arrayPush(0.6);
    stiffnesses.arrayPush(0.6);
    stiffnesses.arrayPush(0.6);
    stiffnesses.arrayPush(0.6);
    stiffnesses.arrayPush(0.6);
    stiffnesses.arrayPush(0.95);
    stiffnesses.arrayPush(0.95);
    stiffnesses.arrayPush(0.95);
    stiffnesses.arrayPush(0.95);
    stiffnesses.arrayPush(0.95);
    stiffnesses.arrayPush(0.95);
    stiffnesses.arrayPush(0.95);
    stiffnesses.arrayPush(0.95);
    stiffnesses.arrayPush(0.95);
    stiffnesses.arrayPush(0.95);
    stiffnesses.arrayPush(0.95);
    stiffnesses.arrayPush(0.95);
    stiffnesses.arrayPush(0.6);
    stiffnesses.arrayPush(0.6);

    m_motProxy.setStiffnesses(names, stiffnesses);
}

void Motions::unstiff()
{
    m_motProxy.setStiffnesses("Body", 0);
}

int main(int argc, const char *argv[])
{
    // read the IP from the commandline args
    const char *ip;
    if (argc >= 2) {
        ip = argv[1];
    } else {
        ip = "0.0.0.0";
    }

    Motions m(ip);

    // some testing
    m.stiff();
    m.backToStand();
    usleep(10000000);
    m.unstiff();

    return 0;
}
