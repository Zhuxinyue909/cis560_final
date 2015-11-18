#include "cube.h"
#include <la.h>
#include <iostream>

static const int CUB_IDX_COUNT = 36;
static const int CUB_VERT_COUNT = 24;

void Cube::ComputeArea()
{
    //Extra credit to implement this
    glm::vec4 width(0.0f, 1.0f, 0.0f, 0.0f);
    glm::vec4 length(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 height(0.0f, 0.0f, 1.0f, 0.0f);

    glm::vec3 w_world(glm::vec3(transform.T()*width));
    glm::vec3 l_world(glm::vec3(transform.T()*length));
    glm::vec3 h_world(glm::vec3(transform.T()*height));

    float w = glm::length(w_world);
    float l = glm::length(l_world);
    float h = glm::length(h_world);

    area = 2*w*l + 2*w*h + 2*l*h;
}

glm::vec4 GetCubeNormal(const glm::vec4& P)
{
    int idx = 0;
    float val = -1;
    for(int i = 0; i < 3; i++){
        if(glm::abs(P[i]) > val){
            idx = i;
            val = glm::abs(P[i]);
        }
    }
    glm::vec4 N(0,0,0,0);
    N[idx] = glm::sign(P[idx]);
    return N;
}

glm::vec3 Cube::ComputeNormal(const glm::vec3 &P)
{return glm::vec3(0);}

void Cube::ComputeTangent(const glm::vec4 &P, Intersection* isx)
{
    //check which face the intersection in the primitive frame is on
    glm::vec3 pt1;
    glm::vec3 pt2;
    glm::vec3 pt3;
    glm::vec3 dp1;
    glm::vec3 dp2;
    glm::vec2 uv1;
    glm::vec2 uv2;
    glm::vec3 Tangent;

    if(fequal(P.x, -0.5f))
    {
        pt1 = glm::vec3(-0.5f, 0.0f, 0.0f);
        pt2 = glm::vec3(-0.5f, 0.0f, 0.1f);
        pt3 = glm::vec3(-0.5f, 0.1f, 0.0f);
    }
    else if(fequal(P.x, 0.5f))
    {
        pt1 = glm::vec3(0.5f, 0.0f, 0.0f);
        pt2 = glm::vec3(0.5f, 0.0f, 0.1f);
        pt3 = glm::vec3(0.5f, 0.1f, 0.0f);
    }
    else if(fequal(P.y, -0.5f))
    {
        pt1 = glm::vec3(0.0f, -0.5f, 0.0f);
        pt2 = glm::vec3(0.0f, -0.5f, 0.1f);
        pt3 = glm::vec3(0.1f, -0.5f, 0.0f);
    }
    else if(fequal(P.y, 0.5f))
    {
        pt1 = glm::vec3(0.0f, 0.5f, 0.0f);
        pt2 = glm::vec3(0.0f, 0.5f, 0.1f);
        pt3 = glm::vec3(0.1f, 0.5f, 0.0f);
    }
    else if(fequal(P.z, -0.5f))
    {
        pt1 = glm::vec3(0.0f, 0.0f, -0.5f);
        pt2 = glm::vec3(0.1f, 0.0f, -0.5f);
        pt3 = glm::vec3(0.0f, 0.1f, -0.5f);
    }
    else if(fequal(P.z, 0.5f))
    {
        pt1 = glm::vec3(0.0f, 0.0f, 0.5f);
        pt2 = glm::vec3(0.1f, 0.0f, 0.5f);
        pt3 = glm::vec3(0.0f, 0.1f, 0.5f);
    }
    //catch corner cases which my intersection is the same as my sample points on the plane
    if (glm::vec3(P) == pt1) pt1 = pt3;
    if (glm::vec3(P) == pt2) pt2 = pt3;

    dp1 = pt1 - glm::vec3(P);
    dp2 = pt2 - glm::vec3(P);

    uv1 = GetUVCoordinates(pt1);
    uv2 = GetUVCoordinates(pt2);

    isx->tangent = (uv2.y*dp1 - uv1.y*dp2)/(uv2.y*uv1.x - uv1.y*uv2.x);
    isx->bitangent = (dp2 - uv2.x*isx->tangent)/uv2.y;
}
Intersection Cube::GetIntersection(Ray r)
{
    //Transform the ray
    Ray r_loc = r.GetTransformedCopy(transform.invT());
    Intersection result;

    float t_n = -1000000;
    float t_f = 1000000;
    for(int i = 0; i < 3; i++){
        //Ray parallel to slab check
        if(r_loc.direction[i] == 0){
            if(r_loc.origin[i] < -0.5f || r_loc.origin[i] > 0.5f){
                return result;
            }
        }
        //If not parallel, do slab intersect check
        float t0 = (-0.5f - r_loc.origin[i])/r_loc.direction[i];
        float t1 = (0.5f - r_loc.origin[i])/r_loc.direction[i];
        if(t0 > t1){
            float temp = t1;
            t1 = t0;
            t0 = temp;
        }
        if(t0 > t_n){
            t_n = t0;
        }
        if(t1 < t_f){
            t_f = t1;
        }
    }
    if(t_n < t_f && t_n >= 0){
        //Lastly, transform the point found in object space by T
        glm::vec4 P = glm::vec4(r_loc.origin + t_n*r_loc.direction, 1);
        result.point = glm::vec3(transform.T() * P);
        result.normal = glm::normalize(glm::vec3(transform.invTransT() * GetCubeNormal(P)));
        result.object_hit = this;
        result.t = glm::distance(result.point, r.origin);
        result.texture_color = Material::GetImageColorInterp(GetUVCoordinates(glm::vec3(P)), material->texture);
        //TODO: Store the tangent and bitangent
        //for calculating tangent
        ComputeTangent(P, &result);
        //transform tangent and bitangent
        result.tangent = glm::vec3(transform.T()*glm::vec4(result.tangent, 0));
        result.bitangent = glm::vec3(transform.T()*glm::vec4(result.tangent, 0));
        result.tangent = glm::normalize(result.tangent);
        result.bitangent = glm::normalize(result.bitangent);
        return result;
    }
    else{//If t_near was greater than t_far, we did not hit the cube
        return result;
    }
}

glm::vec2 Cube::GetUVCoordinates(const glm::vec3 &point)
{
    glm::vec3 abs = glm::min(glm::abs(point), 0.5f);
    glm::vec2 UV;//Always offset lower-left corner
    if(abs.x > abs.y && abs.x > abs.z)
    {
        UV = glm::vec2(point.z + 0.5f, point.y + 0.5f)/3.0f;
        //Left face
        if(point.x < 0)
        {
            UV += glm::vec2(0, 0.333f);
        }
        else
        {
            UV += glm::vec2(0, 0.667f);
        }
    }
    else if(abs.y > abs.x && abs.y > abs.z)
    {
        UV = glm::vec2(point.x + 0.5f, point.z + 0.5f)/3.0f;
        //Left face
        if(point.y < 0)
        {
            UV += glm::vec2(0.333f, 0.333f);
        }
        else
        {
            UV += glm::vec2(0.333f, 0.667f);
        }
    }
    else
    {
        UV = glm::vec2(point.x + 0.5f, point.y + 0.5f)/3.0f;
        //Left face
        if(point.z < 0)
        {
            UV += glm::vec2(0.667f, 0.333f);
        }
        else
        {
            UV += glm::vec2(0.667f, 0.667f);
        }
    }
    return UV;
}

Intersection Cube::GetRandISX(float rand1, float rand2, const glm::vec3 &isx_normal)
{
    Intersection rand_isx;
    glm::vec4 point_prim(0, 0, 0, 1); // point in primitive frame
    glm::vec4 normal_prim(0, 0, 0, 0);
    const int faces = 6; // number of faces
    const float xyz_max(0.5f);
    const float xyz_min(0.5f);

    int F = rand()%faces; // determines which face the point falls on

    float x1 = rand1-0.5f;
    float x2 = rand2-0.5f;

    switch(F)
    {
    case 0: //bottom face
        point_prim.y = xyz_min;
        point_prim.x = x1;
        point_prim.z = x2;
        normal_prim.y = -1.0f;
        break;
    case 1: //top face
        point_prim.y = xyz_max;
        point_prim.x = x1;
        point_prim.z = x2;
        normal_prim.y = 1.0f;
        break;
    case 2: //front face
        point_prim.z = xyz_min;
        point_prim.x = x1;
        point_prim.y = x2;
        normal_prim.z = -1.0f;
        break;
    case 3: // back face
        point_prim.z = xyz_max;
        point_prim.x = x1;
        point_prim.y = x2;
        normal_prim.z = 1.0f;
        break;
    case 4: //left face
        point_prim.x = xyz_min;
        point_prim.y = x1;
        point_prim.z = x2;
        normal_prim.x = -1.0f;
        break;
    case 5: //right face
        point_prim.x = xyz_max;
        point_prim.y = x1;
        point_prim.z = x2;
        normal_prim.x = 1.0f;
        break;
    }

    //transform the point to world frame

    rand_isx.point = glm::vec3(transform.T()*point_prim);
    rand_isx.normal = glm::vec3(transform.invTransT()*normal_prim);
    rand_isx.object_hit = this;
    rand_isx.t = 0.0f;
    rand_isx.texture_color = Material::GetImageColorInterp(GetUVCoordinates(glm::vec3(point_prim)), material->texture);
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
