#include "cube.h"
#include <la.h>
#include <iostream>

static const int CUB_IDX_COUNT = 36;
static const int CUB_VERT_COUNT = 24;

glm::vec2 Cube::GetUVCoordinates(const glm::vec3 &point)
{
    const float side(1.0f);
    const float xyz_min(-side/2);
    const float xyz_max(-side/2);
    glm::vec2 uv_coord;
    //unwrap the cube
    if (fequal(point.z, xyz_min))
    {
        uv_coord.x = point.x/4.0f + 0.375f;
        uv_coord.y = point.y/4.0f + 0.375f;
    }
    else if (fequal(point.x, xyz_min))
    {
        uv_coord.x = 0.125f - point.z/4.0f;
        uv_coord.y = point.y/4.0f + 0.375f;
    }
    else if (fequal(point.x, xyz_max))
    {
        uv_coord.x = 0.625f + point.z/4.0f;
        uv_coord.y = point.y/4.0f + 0.375f;
    }
    else if (fequal(point.z, xyz_max))
    {
        uv_coord.x = point.x/4.0f + 0.375f;
        uv_coord.y = point.y/4.0f + 0.625f;
    }
    else if (fequal(point.y, xyz_max))
    {
        uv_coord.x = point.x/4.0f + 0.625;
        uv_coord.y = 0.375f + point.z/4.0f;
    }
    else
    {
        uv_coord.x = point.x/4.0f + 0.375;
        uv_coord.y = point.z/4.0f + 0.125;
    }

    return uv_coord;
}

Intersection Cube::GetIntersection(Ray r)
{
    //assume side size is 1.0
    const float s(1.0f);
    const float xyz_min(-s/2);
    const float xyz_max(s/2);
    float t0; //near intersection
    float t1; //far intersection
    //start with t_near and t_far at negative and positive infinity
    float t_near(-std::numeric_limits<float>::infinity());
    float t_far(std::numeric_limits<float>::infinity());

    //initialize some intersection XX to return later
    Intersection XX;

    //distort the Ray to check for intersection
    Ray r_transformed = r.GetTransformedCopy(transform.invT());

    //short cut! if I am parallel to x, just check if I am within x slab's boundary!
    if (r_transformed.direction.x == 0)
    {
        if(r_transformed.origin.x < xyz_min || r_transformed.origin.x > xyz_max)
        {
            return XX;
        }
    }
    //calculate t0,t1
    t0 = (xyz_min - r_transformed.origin.x) / r_transformed.direction.x;
    t1 = (xyz_max - r_transformed.origin.x) / r_transformed.direction.x;
    //make appropriate swaps
    if (t0 > t1)
    {
        float temp(t0);
        t0 = t1;
        t1 = temp;
    }

    if (t0 > t_near) t_near = t0;
    if (t1 < t_far) t_far = t1;

    //repeat for y
    if (r_transformed.direction.y == 0)
    {
        if(r_transformed.origin.y < xyz_min || r_transformed.origin.y > xyz_max)
        {
            return XX;
        }
    }
    //calculate t0,t1
    t0 = (xyz_min - r_transformed.origin.y) / r_transformed.direction.y;
    t1 = (xyz_max - r_transformed.origin.y) / r_transformed.direction.y;
    //make appropriate swaps
    if (t0 > t1)
    {
        float temp(t0);
        t0 = t1;
        t1 = temp;
    }

    if (t0 > t_near) t_near = t0;
    if (t1 < t_far) t_far = t1;

    //check for miss
    if(t_near > t_far) return XX;

    //repeat for z
    if (r_transformed.direction.z == 0)
    {
        if(r_transformed.origin.z < xyz_min || r_transformed.origin.z > xyz_max)
        {
            return XX;
        }
    }
    //calculate t0,t1
    t0 = (xyz_min - r_transformed.origin.z) / r_transformed.direction.z;
    t1 = (xyz_max - r_transformed.origin.z) / r_transformed.direction.z;
    //make appropriate swaps
    if (t0 > t1)
    {
        float temp(t0);
        t0 = t1;
        t1 = temp;
    }

    if (t0 > t_near) t_near = t0;
    if (t1 < t_far) t_far = t1;

    //check for miss
    if(t_near > t_far) return XX;
    //check for behind the ray
    if(t_near < 0) return XX;

    //if we didnt miss, find the intersection
    glm::vec4 XX_point(r_transformed.origin + t_near*r_transformed.direction, 1);
    glm::vec4 XX_normal;
    //find normals by checking if either value of the intersection point is at x_max or x_min
    if (fequal(XX_point.x, xyz_max)) XX_normal = glm::vec4(1, 0, 0, 0);
    if (fequal(XX_point.y, xyz_max)) XX_normal = glm::vec4(0, 1, 0, 0);
    if (fequal(XX_point.z, xyz_max)) XX_normal = glm::vec4(0, 0, 1, 0);

    if (fequal(XX_point.x, xyz_min)) XX_normal = glm::vec4(-1,  0,  0, 0);
    if (fequal(XX_point.y, xyz_min)) XX_normal = glm::vec4( 0, -1,  0, 0);
    if (fequal(XX_point.z, xyz_min)) XX_normal = glm::vec4( 0,  0, -1, 0);
    //fill in pixel color based on the material
    glm::vec3 color(material->base_color);
    //modify color if there is texture
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
    //rotate both point and normal vector back to the world frame
    //then fill in the fields of intersection XX
    XX.point = glm::vec3(transform.T()*XX_point);
    XX.object_hit = this;
    XX.t = glm::length(XX.point - r.origin);

    return XX;
}

glm::vec3 Cube::RandPTGen()
{
    const float xyzmax(0.5);
    const float xyzmin(-0.5);
    int face(rand()%6);
    glm::vec4 point(0, 0, 0, 1);

    switch(face)
    {
    case 0:
        point.x = -0.5f + static_cast<float>(rand()%1000)/1000.0f;
        point.y = -0.5f + static_cast<float>(rand()%1000)/1000.0f;
        point.z = xyzmin;
        break;
    case 1:
        point.x = -0.5f + static_cast<float>(rand()%1000)/1000.0f;
        point.y = -0.5f + static_cast<float>(rand()%1000)/1000.0f;
        point.z = xyzmax;
        break;
    case 2:
        point.x = xyzmin;
        point.y = -0.5f + static_cast<float>(rand()%1000)/1000.0f;
        point.z = -0.5f + static_cast<float>(rand()%1000)/1000.0f;
        break;
    case 3:
        point.x = xyzmax;
        point.y = -0.5f + static_cast<float>(rand()%1000)/1000.0f;
        point.z = -0.5f + static_cast<float>(rand()%1000)/1000.0f;
        break;
    case 4:
        point.x = -0.5f + static_cast<float>(rand()%1000)/1000.0f;
        point.y = xyzmin;
        point.z = -0.5f + static_cast<float>(rand()%1000)/1000.0f;
        break;
    case 5:
        point.x = -0.5f + static_cast<float>(rand()%1000)/1000.0f;
        point.y = xyzmax;
        point.z = -0.5f + static_cast<float>(rand()%1000)/1000.0f;
        break;
    }

    return glm::vec3(transform.T()*point);
}

void Cube::SetBBX()
{
    //this funciton sets the bounding box for the cube using the assumed primitive geometry and transform
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

//These are functions that are only defined in this cpp file. They're used for organizational purposes
//when filling the arrays used to hold the vertex and index data.
void createCubeVertexPositions(glm::vec3 (&cub_vert_pos)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);

    //Right face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);

    //Left face
    //UR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);

    //Back face
    //UR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);

    //Top face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);

    //Bottom face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
}


void createCubeVertexNormals(glm::vec3 (&cub_vert_nor)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,1);
    }
    //Right
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(1,0,0);
    }
    //Left
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(-1,0,0);
    }
    //Back
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,-1);
    }
    //Top
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,1,0);
    }
    //Bottom
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,-1,0);
    }
}

void createCubeIndices(GLuint (&cub_idx)[CUB_IDX_COUNT])
{
    int idx = 0;
    for(int i = 0; i < 6; i++){
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+1;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4+3;
    }
}

void Cube::create()
{
    GLuint cub_idx[CUB_IDX_COUNT];
    glm::vec3 cub_vert_pos[CUB_VERT_COUNT];
    glm::vec3 cub_vert_nor[CUB_VERT_COUNT];
    glm::vec3 cub_vert_col[CUB_VERT_COUNT];

    createCubeVertexPositions(cub_vert_pos);
    createCubeVertexNormals(cub_vert_nor);
    createCubeIndices(cub_idx);

    for(int i = 0; i < CUB_VERT_COUNT; i++){
        cub_vert_col[i] = material->base_color;
    }

    count = CUB_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(cub_idx, CUB_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(cub_vert_pos,CUB_VERT_COUNT * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(cub_vert_nor, CUB_VERT_COUNT * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(cub_vert_col, CUB_VERT_COUNT * sizeof(glm::vec3));

}
