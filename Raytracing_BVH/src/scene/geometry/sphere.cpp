#include "sphere.h"

#include <iostream>

#include <la.h>

static const int SPH_IDX_COUNT = 2280;  // 760 tris * 3
static const int SPH_VERT_COUNT = 382;

glm::vec2 Sphere::GetUVCoordinates(const glm::vec3 &point)
{
    const float radius(0.5);
    float theta;
    float phi;
    glm::vec2 uv_coord;
    //get spherical coordinate
    theta = glm::atan(point.y, point.x);
    phi = glm::acos(point.z/radius);
    uv_coord.x = (theta+PI)/2/PI;
    uv_coord.y = phi/PI;

    return uv_coord;
}

Intersection Sphere::GetIntersection(Ray r)
{
    // assume radius = 0.5 for primitive type
    const float radius(0.5);

    //initialize some intersection XX to return later
    Intersection XX;

    //distort the Ray to check for intersection
    Ray r_transformed = r.GetTransformedCopy(transform.invT());

    //assume center at <0, 0, 0> from primitive type,
    //I can leave out the some terms in the intersection formula

    //get parameters A, B and C to determine intersection
    float A = glm::dot(r_transformed.direction, r_transformed.direction);
    float B = 2*glm::dot(r_transformed.direction, r_transformed.origin);
    float C = glm::dot(r_transformed.origin, r_transformed.origin) - static_cast<float>(pow(radius, 2.0f));

    float discriminant = glm::pow(B, 2.0f) - 4*A*C;

    // if no intersection do nothing to XX
    if (discriminant < 0)
    {
        return XX;
    }
    else
    {
        //find the intersection, first try the closer one
        float t0 = (-B - sqrt(discriminant)) / (2 * A);

        if (t0 > 0)
        {
            //if the closer intersection is valid, Hooray!
            XX.t = t0;
        }
        else
        {
            //use the other point
            float t1 = (-B + sqrt(discriminant)) / (2 * A);
            XX.t = t1;
        }
        //if both t0 and t1 are negative, the intersection is behind the ray thus shouldn't show up
        if (XX.t < 0) return XX;


        // calculate the intersection in world frame
        glm::vec4 XX_point((r_transformed.origin + XX.t*r_transformed.direction),1);
        glm::vec4 XX_normal((r_transformed.origin + XX.t*r_transformed.direction), 0);

        //fill in pixel color based on the material
        glm::vec3 color(material->base_color);


        if (material->texture != NULL)
        {
            glm::vec2 uv_coord(GetUVCoordinates(glm::vec3(XX_point))); // find UV coordinate of the intersection
            color.r = color.r * Material::GetImageColor(uv_coord, material->texture).r;
            color.g = color.g * Material::GetImageColor(uv_coord, material->texture).g;
            color.b = color.b * Material::GetImageColor(uv_coord, material->texture).b;

            if (material->normal_map!= NULL)
            {
                glm::vec3 mapped_normal(Material::GetImageColor(uv_coord, material->normal_map));
                glm::vec3 tangent(glm::cross(glm::vec3(0, 1, 0), mapped_normal));
                glm::vec3 bitangent(glm::cross(tangent, mapped_normal));
                glm::mat4 norm_mat = glm::mat4(glm::vec4(tangent, 0), glm::vec4(bitangent, 0), glm::vec4(mapped_normal, 0), glm::vec4(0,0,0,1));

                XX.normal = glm::normalize(glm::vec3(norm_mat*transform.invTransT()*XX_normal));
            }
            else
            {
                XX.normal = glm::normalize(glm::vec3(transform.invTransT()*XX_normal));
            }
        }
        else
        {
            XX.normal = glm::normalize(glm::vec3(transform.invTransT()*XX_normal));
        }

        XX.rgb = color;

        // fill in XX's properties
        XX.point = glm::vec3(transform.T()*XX_point);
        XX.object_hit = this;
        XX.t = glm::length(XX.point - r.origin);
    }

    return XX;
}

glm::vec3 Sphere::RandPTGen()
{
    const float radius(0.5f);
    float theta(randfloat()*2*PI);
    float phi(randfloat()*PI);
    glm::vec4 point(0, 0, 0, 1);
    point.z = radius*glm::cos(phi);
    point.x = radius*glm::sin(phi)*glm::cos(theta);
    point.y = radius*glm::sin(phi)*glm::sin(theta);
    return glm::vec3(transform.T()*point);
}

void Sphere::SetBBX()
{
    //this funciton sets the bounding box for the sphere using the assumed primitive geometry and transform
    glm::vec4 vert_primitive[8];
    glm::vec4 vert_world;
    glm::vec3 max_bounds(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
    glm::vec3 min_bounds(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());

    vert_primitive[0] = glm::vec4(0.5f, 0.5f, 0.5f, 1);
    vert_primitive[1] = glm::vec4(-0.5f, 0.5f, 0.5f, 1);
    vert_primitive[2] = glm::vec4(0.5f, -0.5f, 0.5f, 1);
    vert_primitive[3] = glm::vec4(0.5f, 0.5f, -0.5f, 1);
    vert_primitive[4] = glm::vec4(-0.5f, -0.5f, 0.5f, 1);
    vert_primitive[5] = glm::vec4(-0.5f, 0.5f, -0.5f, 1);
    vert_primitive[6] = glm::vec4(0.5f, -0.5f, -0.5f, 1);
    vert_primitive[7] = glm::vec4(-0.5f, -0.5f, -0.5f, 1);

    for(int i = 0; i < 8; i++)
    {
        vert_world = transform.T()*vert_primitive[i];

        max_bounds.x = glm::max(vert_world.x, max_bounds.x);
        max_bounds.y = glm::max(vert_world.y, max_bounds.y);
        max_bounds.z = glm::max(vert_world.z, max_bounds.z);

        min_bounds.x = glm::min(vert_world.x, min_bounds.x);
        min_bounds.y = glm::min(vert_world.y, min_bounds.y);
        min_bounds.z = glm::min(vert_world.z, min_bounds.z);
    }

    BBX = new BoundingBox(max_bounds, min_bounds);
}

// These are functions that are only defined in this cpp file. They're used for organizational purposes
// when filling the arrays used to hold the vertex and index data.
void createSphereVertexPositions(glm::vec3 (&sph_vert_pos)[SPH_VERT_COUNT])
{
    // Create rings of vertices for the non-pole vertices
    // These will fill indices 1 - 380. Indices 0 and 381 will be filled by the two pole vertices.
    glm::vec4 v;
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            v = glm::rotate(glm::mat4(1.0f), j / 20.f * TWO_PI, glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1.0f), -i / 18.0f * PI, glm::vec3(0, 0, 1))
                * glm::vec4(0, 0.5f, 0, 1);
            sph_vert_pos[(i - 1) * 20 + j + 1] = glm::vec3(v);
        }
    }
    // Add the pole vertices
    sph_vert_pos[0] = glm::vec3(0, 0.5f, 0);
    sph_vert_pos[381] = glm::vec3(0, -0.5f, 0);  // 361 - 380 are the vertices for the bottom cap
}


void createSphereVertexNormals(glm::vec3 (&sph_vert_nor)[SPH_VERT_COUNT])
{
    // Unlike a cylinder, a sphere only needs to be one normal per vertex
    // because a sphere does not have sharp edges.
    glm::vec4 v;
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            v = glm::rotate(glm::mat4(1.0f), j / 20.0f * TWO_PI, glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1.0f), -i / 18.0f * PI, glm::vec3(0, 0, 1))
                * glm::vec4(0, 1.0f, 0, 0);
            sph_vert_nor[(i - 1) * 20 + j + 1] = glm::vec3(v);
        }
    }
    // Add the pole normals
    sph_vert_nor[0] = glm::vec3(0, 1.0f, 0);
    sph_vert_nor[381] = glm::vec3(0, -1.0f, 0);
}


void createSphereIndices(GLuint (&sph_idx)[SPH_IDX_COUNT])
{
    int index = 0;
    // Build indices for the top cap (20 tris, indices 0 - 60, up to vertex 20)
    for (int i = 0; i < 19; i++) {
        sph_idx[index] = 0;
        sph_idx[index + 1] = i + 1;
        sph_idx[index + 2] = i + 2;
        index += 3;
    }
    // Must create the last triangle separately because its indices loop
    sph_idx[57] = 0;
    sph_idx[58] = 20;
    sph_idx[59] = 1;
    index += 3;

    // Build indices for the body vertices
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            sph_idx[index] = (i - 1) * 20 + j + 1;
            sph_idx[index + 1] = (i - 1) * 20 +  j + 2;
            sph_idx[index + 2] = (i - 1) * 20 +  j + 22;
            sph_idx[index + 3] = (i - 1) * 20 +  j + 1;
            sph_idx[index + 4] = (i - 1) * 20 +  j + 22;
            sph_idx[index + 5] = (i - 1) * 20 +  j + 21;
            index += 6;
        }
    }

    // Build indices for the bottom cap (20 tris, indices 2220 - 2279)
    for (int i = 0; i < 19; i++) {
        sph_idx[index] = 381;
        sph_idx[index + 1] = i + 361;
        sph_idx[index + 2] = i + 362;
        index += 3;
    }
    // Must create the last triangle separately because its indices loop
    sph_idx[2277] = 381;
    sph_idx[2278] = 380;
    sph_idx[2279] = 361;
    index += 3;
}

void Sphere::create()
{
    GLuint sph_idx[SPH_IDX_COUNT];
    glm::vec3 sph_vert_pos[SPH_VERT_COUNT];
    glm::vec3 sph_vert_nor[SPH_VERT_COUNT];
    glm::vec3 sph_vert_col[SPH_VERT_COUNT];

    createSphereVertexPositions(sph_vert_pos);
    createSphereVertexNormals(sph_vert_nor);
    createSphereIndices(sph_idx);
    for (int i = 0; i < SPH_VERT_COUNT; i++) {
        sph_vert_col[i] = material->base_color;
    }

    count = SPH_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(sph_idx, SPH_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(sph_vert_pos, SPH_VERT_COUNT * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(sph_vert_col, SPH_VERT_COUNT * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(sph_vert_nor, SPH_VERT_COUNT * sizeof(glm::vec3));
}
