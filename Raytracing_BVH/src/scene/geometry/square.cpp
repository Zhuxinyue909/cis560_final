#include <scene/geometry/square.h>

glm::vec2 SquarePlane::GetUVCoordinates(const glm::vec3 &point)
{
    glm::vec2 uv_coord;
    uv_coord.x = point.x + 0.5;
    uv_coord.y = point.y + 0.5;
    return uv_coord;
}

Intersection SquarePlane::GetIntersection(Ray r)
{
    //The plane primitive has side lengths 1, cetnered at origin with a normal along z
    const float s(1.0);
    const float xy_min(-s/2);
    const float xy_max(s/2);
    float t; //distance to traverse along the ray to intersection
    glm::vec3 normal(0, 0, 1); //normal vector
    glm::vec3 P; //intersection of the ray with the plane

    //initialize some intersection XX to return later
    Intersection XX;

    //distort the Ray to check for intersection
    Ray r_transformed = r.GetTransformedCopy(transform.invT());
    //Hack! the ray intersects the plane at some point with z = 0
    t = -r_transformed.origin.z / r_transformed.direction.z;
    if(t < 0) return XX; // this case the point of intersection is behind origin
    /*correct formula (no hack)
    t = glm::dot(normal, (some_point - r_transformed.origin)) / dot(normal, r_transformed.direction);
    */
    P = r_transformed.origin + t*r_transformed.direction;

    //check if the point of intersection is outside of the plane, if so, abort
    if (P.x > xy_max || P.y > xy_max || P.x < xy_min || P.y < xy_min) return XX;


    glm::vec4 XX_normal(normal, 0);

    //fill in pixel color based on the material
    glm::vec3 color(material->base_color);


    if (material->texture != NULL)
    {
        glm::vec2 uv_coord(GetUVCoordinates(glm::vec3(P))); // find UV coordinate of the intersection
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

    //transform back to world frame
    glm::vec4 XX_point(P, 1);


    XX.point = glm::vec3(transform.T()*XX_point);
    XX.object_hit = this;
    XX.t = glm::length(XX.point - r.origin);

    return XX;
}

glm::vec3 SquarePlane::RandPTGen()
{
    glm::vec4 point(0, 0, 0, 1);
    point.x = -0.5f + static_cast<float>(rand()%1000)/1000.0f;
    point.y = -0.5f + static_cast<float>(rand()%1000)/1000.0f;
    return glm::vec3(transform.T()*point);
}

void SquarePlane::SetBBX()
{
    //this funciton sets the bounding box for the plane using the assumed primitive geometry and transform
    glm::vec4 vert_primitive[8];
    glm::vec4 vert_world;
    glm::vec3 max_bounds(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
    glm::vec3 min_bounds(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());

    vert_primitive[0] = glm::vec4(0.5f, 0.5f, 0.0f, 1);
    vert_primitive[1] = glm::vec4(-0.5f, 0.5f, 0.0f, 1);
    vert_primitive[2] = glm::vec4(0.5f, -0.5f, 0.0f, 1);
    vert_primitive[3] = glm::vec4(-0.5f, -0.5f, 0.0f, 1);

    for(int i = 0; i < 4; i++)
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

void SquarePlane::create()
{
    GLuint cub_idx[6];
    glm::vec3 cub_vert_pos[4];
    glm::vec3 cub_vert_nor[4];
    glm::vec3 cub_vert_col[4];

    cub_vert_pos[0] = glm::vec3(-0.5f, 0.5f, 0);  cub_vert_nor[0] = glm::vec3(0, 0, 1); cub_vert_col[0] = material->base_color;
    cub_vert_pos[1] = glm::vec3(-0.5f, -0.5f, 0); cub_vert_nor[1] = glm::vec3(0, 0, 1); cub_vert_col[1] = material->base_color;
    cub_vert_pos[2] = glm::vec3(0.5f, -0.5f, 0);  cub_vert_nor[2] = glm::vec3(0, 0, 1); cub_vert_col[2] = material->base_color;
    cub_vert_pos[3] = glm::vec3(0.5f, 0.5f, 0);   cub_vert_nor[3] = glm::vec3(0, 0, 1); cub_vert_col[3] = material->base_color;

    cub_idx[0] = 0; cub_idx[1] = 1; cub_idx[2] = 2;
    cub_idx[3] = 0; cub_idx[4] = 2; cub_idx[5] = 3;

    count = 6;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(cub_idx, 6 * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(cub_vert_pos, 4 * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(cub_vert_nor, 4 * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(cub_vert_col, 4 * sizeof(glm::vec3));
}
