#pragma once

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

#include <openGL/glwidget277.h>
#include <la.h>
#include <openGL/shaderprogram.h>
#include <scene/camera.h>
#include <scene/scene.h>
#include <la.h>
#include <scene/xmlreader.h>
#include <raytracing/integrator.h>
#include <scene/xmlreader.h>
#include <raytracing/integrator.h>
#include <mytimer.h>



class MyGL
    : public GLWidget277
{
    Q_OBJECT
private:
    QOpenGLVertexArrayObject vao;

    ShaderProgram prog_lambert;
    ShaderProgram prog_flat;

    Camera gl_camera;//This is a camera we can move around the scene to view it from any angle.
                                //However, the camera defined in the config file is the one from which the scene will be rendered.
                                //If you move gl_camera, you will be able to see the viewing frustum of the scene's camera.

    Scene scene;
    BVHNode Root;
    XMLReader xml_reader;
    Integrator integrator;
    IntersectionEngine intersection_engine;

public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void SceneLoadDialog();
    void GLDrawScene();

    void ResizeToSceneCamera();
    void RaytraceScene();

protected:
    void keyPressEvent(QKeyEvent *e);
    void DrawBBX(BVHNode *start_pt); //recursive function to draw all the bounding boxes
    void DrawBBX_mesh(BVHNode *start_pt);


signals:
    void sig_ResizeToCamera(int,int);

};
