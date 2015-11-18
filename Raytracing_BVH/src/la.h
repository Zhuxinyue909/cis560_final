#pragma once

// NOTE: This #define forces GLM to use radians (not degrees) for ALL of its
// angle arguments. The documentation may not always reflect this fact.
#define GLM_FORCE_RADIANS

#define GLM_CIS277  // Don't copy this include!
// Primary GLM library
#    include <glm/glm.hpp>
// For glm::translate, glm::rotate, and glm::scale.
#    include <glm/gtc/matrix_transform.hpp>
// For glm::to_string.
#    include <glm/gtx/string_cast.hpp>
// For glm::value_ptr.
#    include <glm/gtc/type_ptr.hpp>
#undef GLM_CIS277

#include <QMatrix4x4>


static const float PI = 3.14159265358979323846f;
static const float TWO_PI = 2 * PI;
static const float DEG2RAD = PI / 180.f;
static const float RAD2DEG = 180.f / PI;

/// 277 linear algebra namespace
namespace la
{
using namespace glm;

inline QMatrix4x4 to_qmat(const mat4 &m)
{
    return QMatrix4x4(value_ptr(transpose(m)));
}
}

/// Float approximate-equality comparison
template<typename T>
inline bool fequal(T a, T b, T epsilon = 0.0001){
    if (a == b) {
        // Shortcut
        return true;
    }

    const T diff = std::abs(a - b);
    if (a * b == 0) {
        // a or b or both are zero; relative error is not meaningful here
        return diff < (epsilon * epsilon);
    }

    return diff / (std::abs(a) + std::abs(b)) < epsilon;
}

inline float triangle_area(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3)
{
//    float area;

//    glm::mat3 matx(p1.y, p2.y, p3.y,
//                   p1.z, p2.z, p3.z,
//                      1,    1,    1);

//    glm::mat3 maty(p1.z, p2.z, p3.z,
//                   p1.x, p2.x, p3.x,
//                      1,    1,    1);

//    glm::mat3 matz(p1.x, p2.x, p3.x,
//                   p1.y, p2.y, p3.y,
//                      1,    1,    1);


//    area = 0.5*glm::sqrt(pow(glm::determinant(matx),2) + pow(glm::determinant(maty),2) + pow(glm::determinant(matz),2));
//    return area;
    return glm::length(glm::cross(p1 - p2, p3 - p2)) * 0.5f;
}

inline float randfloat()
{
    //returns a random float between 0 to 1 with a resolution of 0.001;
    return static_cast<float>(rand()%1000)/1000.0f;
}
