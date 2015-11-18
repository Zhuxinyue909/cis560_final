#pragma once

#include<la.h>
#include<openGL/drawable.h>

class BoundingBox : public Drawable
{
public:
    BoundingBox();
    BoundingBox(glm::vec3 max_bounds, glm::vec3 min_bounds);
    glm::vec3 GetCenter();
    glm::vec3 max_bound();
    glm::vec3 min_bound();
    void create();
    GLenum drawMode();


protected:
    glm::vec3 max_bounds;
    glm::vec3 min_bounds;
};

