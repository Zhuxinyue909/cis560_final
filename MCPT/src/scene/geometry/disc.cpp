#include <scene/geometry/disc.h>

void Disc::ComputeArea()
{
    //TODO
    glm::vec4 edge_point_prim(0.5f, 0.0f, 0.0f, 1.0f);
    glm::vec4 origin_prim(0.0f, 0.0f, 0.0f, 1.0f);
    //find some point on the edge of the disk in transformed world space
    glm::vec3 edge_point_world(transform.T()*edge_point_prim);
    //find origin in world space
    glm::vec3 origin_world(transform.T()*origin_prim);
    //calculate radius
    float r(glm::distance(edge_point_world, origin_world));
    area = r*r*PI;
}

Intersection Disc::GetIntersection(Ray r)
{
    //Transform the ray
    Ray r_loc = r.GetTransformedCopy(transform.invT());
    Intersection result;

    //Ray-plane intersection
    float t = glm::dot(glm::vec3(0,0,1), (glm::vec3(0.5f, 0.5f, 0) - r_loc.origin)) / glm::dot(glm::vec3(0,0,1), r_loc.direction);
    glm::vec4 P = glm::vec4(t * r_loc.direction + r_loc.origin, 1);
    //Check that P is within the bounds of the disc (not bothering to take the sqrt of the dist b/c we know the radius)
    float dist2 = (P.x * P.x + P.y * P.y);
    if(t > 0 && dist2 <= 0.25f)
    {
        result.point = glm::vec3(transform.T() * P);
        result.normal = glm::normalize(glm::vec3(transform.invTransT() * glm::vec4(ComputeNormal(glm::vec3(P)), 0)));
        result.object_hit = this;
        result.t = glm::distance(result.point, r.origin);
        result.texture_color = Material::GetImageColorInterp(GetUVCoordinates(glm::vec3(P)), material->texture);
        //TODO: Store the tangent and bitangent
        //for calculating tangent
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

glm::vec2 Disc::GetUVCoordinates(const glm::vec3 &point)
{
    return glm::vec2(point.x + 0.5f, point.y + 0.5f);
}

glm::vec3 Disc::ComputeNormal(const glm::vec3 &P)
{
    return glm::vec3(0,0,1);
}
/*
glm::vec3 Disc::GetRandPoint()
{
    glm::vec4 point_prim(0, 0, 0, 1); // point in primitive frame
    const int Res = 1000; //resolution
    const float r_max = 0.5f;
    const float theta_max = 2.0f*PI;

    int X1 = rand()%Res; //0 - 999
    int X2 = rand()%Res; //0 - 999

    //polar coordinate
    float r = static_cast<float>(X1)/static_cast<float>(Res-1)*r_max;
    float theta = static_cast<float>(X2)/static_cast<float>(Res-1)*theta_max;

    //polar coordinate
    point_prim.x = r*glm::cos(theta);
    point_prim.y = r*glm::sin(theta);

    //transform
    return glm::vec3(transform.T()*point_prim);
}
*/
Intersection Disc::GetRandISX(float rand1, float rand2, const glm::vec3 &isx_normal)
{
    Intersection rand_isx;
    //do this to avoid clumping
    float r(glm::sqrt(rand1)*0.5f);
    float theta(2.0f*PI*rand2);
    float x(r*glm::cos(theta));
    float y(r*glm::sin(theta));
    glm::vec4 rand_pt_prim(x, y, 0.0f, 1.0f);
    glm::vec4 normal_prim(0.0f, 0.0f, 1.0f, 1.0f);

    rand_isx.point = glm::vec3(transform.T()*rand_pt_prim);
    rand_isx.normal = glm::vec3(transform.invTransT()*normal_prim);
    rand_isx.object_hit = this;
    rand_isx.t = 0.0f;
    rand_isx.texture_color = Material::GetImageColorInterp(GetUVCoordinates(glm::vec3(rand_pt_prim)), material->texture);

    return rand_isx;
}

void Disc::create()
{
    GLuint idx[54];
    //18 tris, 54 indices
    glm::vec3 vert_pos[20];
    glm::vec3 vert_nor[20];
    glm::vec3 vert_col[20];

    //Fill the positions, normals, and colors
    glm::vec4 pt(0.5f, 0, 0, 1);
    float angle = 18.0f * DEG2RAD;
    glm::vec3 axis(0,0,1);
    for(int i = 0; i < 20; i++)
    {
        //Position
        glm::vec3 new_pt = glm::vec3(glm::rotate(glm::mat4(1.0f), angle * i, axis) * pt);
        vert_pos[i] = new_pt;
        //Normal
        vert_nor[i] = glm::vec3(0,0,1);
        //Color
        vert_col[i] = material->base_color;
    }

    //Fill the indices.
    int index = 0;
    for(int i = 0; i < 18; i++)
    {
        idx[index++] = 0;
        idx[index++] = i + 1;
        idx[index++] = i + 2;
    }

    count = 54;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(idx, 54 * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(vert_pos, 20 * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(vert_nor, 20 * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(vert_col, 20 * sizeof(glm::vec3));
}
