#include <scene/geometry/square.h>

void SquarePlane::ComputeArea()
{
    //TODO
    //find coordinate of 3 of the corners in world frame
    glm::vec4 bottom_right_prim(0.5f, -0.5f, 0.0f, 1.0f);
    glm::vec4 top_right_prim(0.5f, 0.5f, 0.0f, 1.0f);
    glm::vec4 bottom_left_prim(-0.5f, -0.5f, 0.0f, 1.0f);

    glm::vec3 bottom_right_world(transform.T()*bottom_right_prim);
    glm::vec3 top_right_world(transform.T()*top_right_prim);
    glm::vec3 bottom_left_world(transform.T()*bottom_left_prim);
    //find the distance from top right to bottom right
    float l1(glm::distance(bottom_right_world, top_right_world));
    //find the distance from bottom left to bottom right
    float l2(glm::distance(bottom_right_world, bottom_left_world));

    area = l1*l2;
}

Intersection SquarePlane::GetIntersection(Ray r)
{
    //Transform the ray
    Ray r_loc = r.GetTransformedCopy(transform.invT());
    Intersection result;

    //Ray-plane intersection
    float t = glm::dot(glm::vec3(0,0,1), (glm::vec3(0.5f, 0.5f, 0) - r_loc.origin)) / glm::dot(glm::vec3(0,0,1), r_loc.direction);
    glm::vec4 P = glm::vec4(t * r_loc.direction + r_loc.origin, 1);
    //Check that P is within the bounds of the square
    if(t > 0 && P.x >= -0.5f && P.x <= 0.5f && P.y >= -0.5f && P.y <= 0.5f)
    {
        result.point = glm::vec3(transform.T() * P);
        result.normal = glm::normalize(glm::vec3(transform.invTransT() * glm::vec4(ComputeNormal(glm::vec3(P)), 0)));
        result.object_hit = this;
        result.t = glm::distance(result.point, r.origin);
        result.texture_color = Material::GetImageColorInterp(GetUVCoordinates(glm::vec3(P)), material->texture);
        //TODO: Store the tangent and bitangent
        glm::vec4 tangent_prim(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec4 bitangent_prim(0.0f, 1.0f, 0.0f, 0.0f);
        //transform tangent and bitangent
        result.tangent = glm::vec3(transform.T()*tangent_prim);
        result.bitangent = glm::vec3(transform.T()*bitangent_prim);
        result.tangent = glm::normalize(result.tangent);
        result.bitangent = glm::normalize(result.bitangent);
        return result;
    }
    return result;
}


glm::vec2 SquarePlane::GetUVCoordinates(const glm::vec3 &point)
{
    return glm::vec2(point.x + 0.5f, point.y + 0.5f);
}

glm::vec3 SquarePlane::ComputeNormal(const glm::vec3 &P)
{
        return glm::vec3(0,0,1);
}
/*
glm::vec3 SquarePlane::GetRandPoint()
{
    glm::vec4 point_prim(0,0,0,1);
    const int Res(1000);

    point_prim.x = static_cast<float>(rand()%Res + 1)/static_cast<float>(Res) - 0.5f;
    point_prim.y = static_cast<float>(rand()%Res + 1)/static_cast<float>(Res) - 0.5f;

    return glm::vec3 (transform.T()*point_prim);

}
*/
Intersection SquarePlane::GetRandISX(float rand1, float rand2, const glm::vec3 &isx_normal)
{
    Intersection rand_isx;
    float x(rand1 - 0.5f);
    float y(rand2 - 0.5f);
    glm::vec4 rand_pt_prim(x, y, 0.0f, 1.0f);
    glm::vec4 normal_prim(0.0f, 0.0f, 1.0f, 1.0f);

    rand_isx.point = glm::vec3(transform.T()*rand_pt_prim);
    rand_isx.normal = glm::vec3(transform.invTransT()*normal_prim);
    rand_isx.object_hit = this;
    rand_isx.t = 0.0f;
    rand_isx.texture_color = Material::GetImageColorInterp(GetUVCoordinates(glm::vec3(rand_pt_prim)), material->texture);

    return rand_isx;
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
