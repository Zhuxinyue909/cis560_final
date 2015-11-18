#include "boundingbox.h"

static const int BBX_IDX_COUNT = 24;
static const int BBX_VERT_COUNT = 8;

BoundingBox::BoundingBox()
{
    BoundingBox(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
}

BoundingBox::BoundingBox(glm::vec3 max, glm::vec3 min)
{
    max_bounds = max;
    min_bounds = min;
}

glm::vec3 BoundingBox::max_bound()
{
    return max_bounds;
}

glm::vec3 BoundingBox::min_bound()
{
    return min_bounds;
}


glm::vec3 BoundingBox::GetCenter()
{
    return (max_bounds+min_bounds)/2.0f;
}

void createBBXVertexPositions(glm::vec3 (&bbx_vert_pos)[BBX_VERT_COUNT], glm::vec3 max_bounds, glm::vec3 min_bounds)
{
    int idx = 0;
    //first four
    bbx_vert_pos[idx++] = glm::vec3(min_bounds.x, min_bounds.y, min_bounds.z);
    bbx_vert_pos[idx++] = glm::vec3(max_bounds.x, min_bounds.y, min_bounds.z);
    bbx_vert_pos[idx++] = glm::vec3(max_bounds.x, min_bounds.y, max_bounds.z);
    bbx_vert_pos[idx++] = glm::vec3(min_bounds.x, min_bounds.y, max_bounds.z);
    //last four
    bbx_vert_pos[idx++] = glm::vec3(min_bounds.x, max_bounds.y, min_bounds.z);
    bbx_vert_pos[idx++] = glm::vec3(max_bounds.x, max_bounds.y, min_bounds.z);
    bbx_vert_pos[idx++] = glm::vec3(max_bounds.x, max_bounds.y, max_bounds.z);
    bbx_vert_pos[idx++] = glm::vec3(min_bounds.x, max_bounds.y, max_bounds.z);
}


void createBBXIndices(GLuint (&bbx_idx)[BBX_IDX_COUNT])
{
    int idx = 0;
    //front face
    bbx_idx[idx++] = 0; bbx_idx[idx++] = 1;
    bbx_idx[idx++] = 1; bbx_idx[idx++] = 2;
    bbx_idx[idx++] = 2; bbx_idx[idx++] = 3;
    bbx_idx[idx++] = 3; bbx_idx[idx++] = 0;
    //back face
    bbx_idx[idx++] = 4; bbx_idx[idx++] = 5;
    bbx_idx[idx++] = 5; bbx_idx[idx++] = 6;
    bbx_idx[idx++] = 6; bbx_idx[idx++] = 7;
    bbx_idx[idx++] = 7; bbx_idx[idx++] = 4;
    //sides
    bbx_idx[idx++] = 2; bbx_idx[idx++] = 6;
    bbx_idx[idx++] = 1; bbx_idx[idx++] = 5;
    bbx_idx[idx++] = 0; bbx_idx[idx++] = 4;
    bbx_idx[idx++] = 3; bbx_idx[idx++] = 7;

}


void BoundingBox::create()
{

    GLuint bbx_idx[BBX_IDX_COUNT];
    glm::vec3 bbx_vert_pos[BBX_VERT_COUNT];
    glm::vec3 bbx_vert_col[BBX_VERT_COUNT];


    createBBXVertexPositions(bbx_vert_pos, max_bounds, min_bounds);
    createBBXIndices(bbx_idx);

    for(int i = 0; i < BBX_VERT_COUNT; i++)
    {
        bbx_vert_col[i] = glm::vec3(0, 0, 0); //draw black lines
    }

    count = BBX_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(bbx_idx, count * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(bbx_vert_pos, BBX_VERT_COUNT * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(bbx_vert_col, BBX_VERT_COUNT * sizeof(glm::vec3));


}

GLenum BoundingBox::drawMode(){return GL_LINES;}
