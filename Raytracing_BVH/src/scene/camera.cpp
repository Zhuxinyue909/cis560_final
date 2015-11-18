#include "camera.h"

#include <la.h>
#include <iostream>


Camera::Camera():
    Camera(400, 400)
{
    look = glm::vec3(0,0,-1);
    up = glm::vec3(0,1,0);
    right = glm::vec3(1,0,0);
}

Camera::Camera(unsigned int w, unsigned int h):
    Camera(w, h, glm::vec3(0,0,10), glm::vec3(0,0,0), glm::vec3(0,1,0))
{}

Camera::Camera(unsigned int w, unsigned int h, const glm::vec3 &e, const glm::vec3 &r, const glm::vec3 &worldUp):
    fovy(45),
    width(w),
    height(h),
    near_clip(0.1f),
    far_clip(1000),
    eye(e),
    ref(r),
    world_up(worldUp)
{
    RecomputeAttributes();
}

Camera::Camera(const Camera &c):
    fovy(c.fovy),
    width(c.width),
    height(c.height),
    near_clip(c.near_clip),
    far_clip(c.far_clip),
    aspect(c.aspect),
    eye(c.eye),
    ref(c.ref),
    look(c.look),
    up(c.up),
    right(c.right),
    world_up(c.world_up),
    V(c.V),
    H(c.H)
{}

void Camera::CopyAttributes(const Camera &c)
{
    fovy = c.fovy;
    near_clip = c.near_clip;
    far_clip = c.far_clip;
    eye = c.eye;
    ref = c.ref;
    look = c.look;
    up = c.up;
    right = c.right;
    width = c.width;
    height = c.height;
    aspect = c.aspect;
    V = c.V;
    H = c.H;
}

void Camera::RecomputeAttributes()
{
    float len;
    aspect = width/height;
    look = glm::normalize(ref - eye);
    right = glm::normalize(glm::cross(look, world_up));
    up = glm::normalize(glm::cross(right, look));
    len = glm::length(ref - eye);
    V = static_cast<float>(len*tan(fovy*DEG2RAD/2))*up;
    H = static_cast<float>(len*aspect*tan(fovy*DEG2RAD/2))*right;
}

glm::mat4 Camera::getViewProj()
{
    return this->PerspectiveProjectionMatrix() * this->ViewMatrix();
}

glm::mat4 Camera::PerspectiveProjectionMatrix()
{
    //Get the dimension of the image plane first
    const float TOP = near_clip * tan(fovy*DEG2RAD/2);
    const float BOTTOM = - TOP;
    const float RIGHT = TOP*aspect;
    const float LEFT = -RIGHT;

    //constrcut the matrix by contructing the 4 column vectors
    glm::vec4 VM_1(2*near_clip /(RIGHT - LEFT), 0, 0, 0);
    glm::vec4 VM_2(0, 2*near_clip /(TOP - BOTTOM), 0, 0);
    glm::vec4 VM_3(-(RIGHT + LEFT)/(RIGHT - LEFT), -(TOP + BOTTOM)/(TOP - BOTTOM), far_clip/(far_clip - near_clip), 1);
    glm::vec4 VM_4(0, 0, -(far_clip * near_clip)/(far_clip - near_clip), 0);
    glm::mat4 PPM (VM_1, VM_2, VM_3, VM_4);
    return PPM;
}

glm::mat4 Camera::ViewMatrix()
{
    //rotation matrix
    //take the row vectors
    glm::vec4 V_O_1(right, 0);
    glm::vec4 V_O_2(up, 0);
    glm::vec4 V_O_3(look, 0);
    glm::vec4 V_O_4(0, 0, 0, 1);
    //initialize O_trans since the constructor take the vectors as column vectors
    glm::mat4 O_trans(V_O_1, V_O_2, V_O_3, V_O_4);
    glm::mat4 O = glm::transpose(O_trans);//transpose to the the correct O


    //translation matrix
    glm::vec4 V_T_1(1, 0, 0, 0);
    glm::vec4 V_T_2(0, 1, 0, 0);
    glm::vec4 V_T_3(0, 0, 1, 0);
    glm::vec4 V_T_4(-eye, 1);
    glm::mat4 T(V_T_1, V_T_2, V_T_3, V_T_4);

    //homogenous transform matrix
    glm::mat4 VM = O*T;

    return VM;
}

void Camera::RotateAboutUp(float deg)
{
    deg *= DEG2RAD;
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), deg, up);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}
void Camera::RotateAboutRight(float deg)
{
    deg *= DEG2RAD;
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), deg, right);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}

void Camera::TranslateAlongLook(float amt)
{
    glm::vec3 translation = look * amt;
    eye += translation;
    ref += translation;
}

void Camera::TranslateAlongRight(float amt)
{
    glm::vec3 translation = right * amt;
    eye += translation;
    ref += translation;
}
void Camera::TranslateAlongUp(float amt)
{
    glm::vec3 translation = up * amt;
    eye += translation;
    ref += translation;
}

Ray Camera::Raycast(const glm::vec2 &pt)
{
    return Raycast(pt.x, pt.y);
}

Ray Camera::Raycast(float x, float y)
{
    // this takes pixel position
    // map pixel position into ndc coordinates
    float ndc_x(-1 + 2*x/width);
    float ndc_y(1 - 2*y/height);

    //call RaycastNDC using ndc coordinates
    return RaycastNDC(ndc_x, ndc_y);
}

Ray Camera::RaycastNDC(float ndc_x, float ndc_y)
{
    //cast the ray given that the input arguments are in the image plane
    glm::vec3 refpt(0, 0, 0);//the z component for the point in the camera frame
    glm::vec3 pt3D(ndc_x*H + ndc_y*V + refpt);
    glm::vec3 direction(glm::normalize(pt3D - eye));

    Ray cast(eye, direction); //returns a ray with the origin at eye(camera position, with the correct direction)

    return cast;
}

void Camera::create()
{
    std::vector<glm::vec3> pos;
    std::vector<glm::vec3> col;
    std::vector<GLuint> idx;

    //0: Eye position
    pos.push_back(eye);
    //1 - 4: Near clip
        //Lower-left
        Ray r = this->RaycastNDC(-1,-1);
        pos.push_back(eye + r.direction * near_clip);
        //Lower-right
        r = this->RaycastNDC(1,-1);
        pos.push_back(eye + r.direction * near_clip);
        //Upper-right
        r = this->RaycastNDC(1,1);
        pos.push_back(eye + r.direction * near_clip);
        //Upper-left
        r = this->RaycastNDC(-1,1);
        pos.push_back(eye + r.direction * near_clip);
    //5 - 8: Far clip
        //Lower-left
        r = this->RaycastNDC(-1,-1);
        pos.push_back(eye + r.direction * far_clip);
        //Lower-right
        r = this->RaycastNDC(1,-1);
        pos.push_back(eye + r.direction * far_clip);
        //Upper-right
        r = this->RaycastNDC(1,1);
        pos.push_back(eye + r.direction * far_clip);
        //Upper-left
        r = this->RaycastNDC(-1,1);
        pos.push_back(eye + r.direction * far_clip);

    for(int i = 0; i < 9; i++){
        col.push_back(glm::vec3(1,1,1));
    }

    //Frustum lines
    idx.push_back(1);idx.push_back(5);
    idx.push_back(2);idx.push_back(6);
    idx.push_back(3);idx.push_back(7);
    idx.push_back(4);idx.push_back(8);
    //Near clip
    idx.push_back(1);idx.push_back(2);
    idx.push_back(2);idx.push_back(3);
    idx.push_back(3);idx.push_back(4);
    idx.push_back(4);idx.push_back(1);
    //Far clip
    idx.push_back(5);idx.push_back(6);
    idx.push_back(6);idx.push_back(7);
    idx.push_back(7);idx.push_back(8);
    idx.push_back(8);idx.push_back(5);

    //Camera axis
    pos.push_back(eye); col.push_back(glm::vec3(0,0,1)); idx.push_back(9);
    pos.push_back(eye + look); col.push_back(glm::vec3(0,0,1));idx.push_back(10);
    pos.push_back(eye); col.push_back(glm::vec3(1,0,0));idx.push_back(11);
    pos.push_back(eye + right); col.push_back(glm::vec3(1,0,0));idx.push_back(12);
    pos.push_back(eye); col.push_back(glm::vec3(0,1,0));idx.push_back(13);
    pos.push_back(eye + up); col.push_back(glm::vec3(0,1,0));idx.push_back(14);

    count = idx.size();

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(idx.data(), count * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(pos.data(), pos.size() * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(col.data(), col.size() * sizeof(glm::vec3));
}

GLenum Camera::drawMode(){return GL_LINES;}
