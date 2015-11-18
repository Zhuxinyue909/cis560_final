#include <scene/geometry/mesh.h>
#include <la.h>
#include <tinyobj/tiny_obj_loader.h>
#include <iostream>

Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3):
    Triangle(p1, p2, p3, glm::vec3(1), glm::vec3(1), glm::vec3(1), glm::vec2(0), glm::vec2(0), glm::vec2(0))
{
    for(int i = 0; i < 3; i++)
    {
        normals[i] = plane_normal;
    }
}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3):
    Triangle(p1, p2, p3, n1, n2, n3, glm::vec2(0), glm::vec2(0), glm::vec2(0))
{}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3, const glm::vec2 &t1, const glm::vec2 &t2, const glm::vec2 &t3)
{
    plane_normal = glm::normalize(glm::cross(p2 - p1, p3 - p2));
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;
    normals[0] = n1;
    normals[1] = n2;
    normals[2] = n3;
    uvs[0] = t1;
    uvs[1] = t2;
    uvs[2] = t3;
}


//Returns the interpolation of the triangle's three normals based on the point inside the triangle that is given.
glm::vec3 Triangle::GetNormal(const glm::vec3 &P)
{
    float A(triangle_area(points[0], points[1], points[2]));
    float A0 = triangle_area(points[1], points[2], P);
    float A1 = triangle_area(points[0], points[2], P);
    float A2 = triangle_area(points[0], points[1], P);
    return glm::normalize(normals[0] * A0/A + normals[1] * A1/A + normals[2] * A2/A);
}

glm::vec4 Triangle::GetNormal(const glm::vec4 &position)
{
    glm::vec3 result = GetNormal(glm::vec3(position));
    return glm::vec4(result, 0);
}

//HAVE THEM IMPLEMENT THIS
//The ray in this function is not transformed because it was *already* transformed in Mesh::GetIntersection
Intersection Triangle::GetIntersection(Ray r_transformed)
{
    //TODO
    Intersection XX; //object to return
    float t; // traversed distance along the ray
    glm::vec3 P; // point of intersection
    glm::vec3 XX_normal(plane_normal);
    //distort ray
    //Ray r_transformed = r.GetTransformedCopy(transform.invT());
    //find intersection with the plane the triangle is in
    t = glm::dot(plane_normal, (points[0] - r_transformed.origin)) / glm::dot(plane_normal, r_transformed.direction);
    P = r_transformed.origin + t*r_transformed.direction;
    // okay now i know the point of intersection, let's check if it's in the triangle
    // first call custom function triangle_area()

    float s(triangle_area(points[0], points[1], points[2]));
    float s1(triangle_area(P, points[1], points[2])/s);
    float s2(triangle_area(P, points[2], points[0])/s);
    float s3(triangle_area(P, points[0], points[1])/s);

    //glm::vec3 P_reconstruct = s1*points[0] + s2*points[1] + s3*points[2];

    if (s1 < 0 || s1 >1 || s2 < 0 || s2 >1 || s3 < 0 || s3 > 1) return XX; //this case has not inside the triangle
    if (!fequal(s1 + s2 + s3, 1.0f)) return XX; // this case the s's don't sum up
    if (t<0) return XX; //this case the intersection is behind the point

    //fill in pixel color based on the material
    glm::vec3 color(material->base_color);

    if (material->texture != NULL)
    {
        glm::vec2 uv_coord(GetUVCoordinates(P)); // find UV coordinate of the intersection
        color.r = color.r * Material::GetImageColor(uv_coord, material->texture).r;
        color.g = color.g * Material::GetImageColor(uv_coord, material->texture).g;
        color.b = color.b * Material::GetImageColor(uv_coord, material->texture).b;

        if (material->normal_map!= NULL)
        {
            glm::vec3 mapped_normal(Material::GetImageColor(uv_coord, material->normal_map));
            glm::vec3 tangent(glm::cross(glm::vec3(0, 1, 0), mapped_normal));
            glm::vec3 bitangent(glm::cross(tangent, mapped_normal));
            glm::mat4 norm_mat = glm::mat4(glm::vec4(tangent, 0), glm::vec4(bitangent, 0), glm::vec4(mapped_normal, 0), glm::vec4(0,0,0,1));

            XX_normal = s1*normals[0] + s2*normals[1] + s3*normals[2];
            XX_normal = glm::normalize(glm::vec3(norm_mat*glm::vec4(XX_normal, 0)));
        }
        else
        {
            XX_normal = s1*normals[0] + s2*normals[1] + s3*normals[2];
        }
    }
    else
    {
        XX_normal = s1*normals[0] + s2*normals[1] + s3*normals[2];
    }

    XX.normal = XX_normal;
    XX.rgb = color;
    XX.point = P; // this is in the primitive plane, to be transformed in mesh
    XX.object_hit = this; 
    XX.t = t;

    return XX;
}

glm::vec2 Triangle::GetUVCoordinates(const glm::vec3 &point)
{
    //find bayercentric coordinate
    float s(triangle_area(points[0], points[1], points[2]));
    float s0(triangle_area(point, points[1], points[2])/s);
    float s1(triangle_area(point, points[2], points[0])/s);
    float s2(triangle_area(point, points[0], points[1])/s);
    //check if the point is inside the triangle, if not, return an impossible uv coordinate
    if (s0 < 0 || s0 >1 || s1 < 0 || s1 >1 || s2 < 0 || s2 > 1) return glm::vec2(2,2); //this case has not inside the triangle
    if (!fequal(s0 + s1 + s2, 1.0f)) return glm::vec2(2,2);
    //otherwise interpolate u and v with bayercenteric coordinate
    return glm::vec2(s0*uvs[0] + s1*uvs[1] + s2*uvs[2]);
}

glm::vec3 Triangle::RandPTGen()
{
    return glm::vec3(0, 0, 0);
}

void Triangle::SetBBX()
{
    //this funciton sets the bounding box for the cube using the assumed primitive geometry and transform
    glm::vec3 max_bounds(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
    glm::vec3 min_bounds(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());

    for(int i = 0; i < 3; i++)
    {

        max_bounds.x = glm::max(points[i].x, max_bounds.x);
        max_bounds.y = glm::max(points[i].y, max_bounds.y);
        max_bounds.z = glm::max(points[i].z, max_bounds.z);

        min_bounds.x = glm::min(points[i].x, min_bounds.x);
        min_bounds.y = glm::min(points[i].y, min_bounds.y);
        min_bounds.z = glm::min(points[i].z, min_bounds.z);
    }
    BBX = new BoundingBox(max_bounds, min_bounds);
}

void Triangle::SetBBX_world(glm::mat4 T_mesh)
{
    //this funciton sets the bounding box for the cube using the assumed primitive geometry and transform
    glm::vec3 max_bounds(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
    glm::vec3 min_bounds(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
    glm::vec4 tri_point_in_world;

    for(int i = 0; i < 3; i++)
    {
        tri_point_in_world = T_mesh * glm::vec4(points[i], 1);

        max_bounds.x = glm::max(tri_point_in_world.x, max_bounds.x);
        max_bounds.y = glm::max(tri_point_in_world.y, max_bounds.y);
        max_bounds.z = glm::max(tri_point_in_world.z, max_bounds.z);

        min_bounds.x = glm::min(tri_point_in_world.x, min_bounds.x);
        min_bounds.y = glm::min(tri_point_in_world.y, min_bounds.y);
        min_bounds.z = glm::min(tri_point_in_world.z, min_bounds.z);
    }
    BBX_world = new BoundingBox(max_bounds, min_bounds);
}

Intersection Mesh::GetIntersection(Ray r)
{

#define USEBVH

#ifndef USEBVH
    float t_max = std::numeric_limits<float>::infinity();
    int counts(faces.count());
    Intersection XX_triangle;
    Intersection XX_mesh;
    Ray r_transformed = r.GetTransformedCopy(transform.invT());


    for (int i = 0; i < counts; i++)
    {
        XX_triangle = faces[i]->GetIntersection(r_transformed);

        if(XX_triangle.object_hit != NULL && XX_triangle.t < t_max)
        {
            XX_mesh = XX_triangle;
            t_max = XX_triangle.t;
        }
    }

   // transform everything back to world frame

    XX_mesh.normal = glm::vec3(transform.invTransT() * glm::vec4(XX_mesh.normal, 0));
    XX_mesh.point = glm::vec3(transform.T() * glm::vec4(XX_mesh.point, 0));
    XX_mesh.t = glm::length(r.origin - XX_mesh.point);


    return XX_mesh;
#else
    Intersection XX_mesh;
    Ray r_transformed = r.GetTransformedCopy(transform.invT());

    XX_mesh = mesh_root.GetIntersection(r_transformed);

    XX_mesh.normal = glm::vec3(transform.invTransT() * glm::vec4(XX_mesh.normal, 0));
    XX_mesh.point = glm::vec3(transform.T() * glm::vec4(XX_mesh.point, 0));
    XX_mesh.t = glm::length(r.origin - XX_mesh.point);


    return XX_mesh;
#endif
}

glm::vec2 Mesh::GetUVCoordinates(const glm::vec3 &point)
{
//empty
    return glm::vec2(0,0);
}

void Mesh::SetMaterial(Material *m)
{
    this->material = m;
    for(Triangle *t : faces)
    {
        t->SetMaterial(m);
    }
}

glm::vec3 Mesh::RandPTGen()
{
    return glm::vec3(0, 0, 0);
}


void Mesh::SetBBX()

{
    glm::vec3 max_bounds(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
    glm::vec3 min_bounds(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
    for(int i = 0; i < faces.count(); i++)
    {
        faces[i]->SetBBX();

        faces[i]->SetBBX_world(transform.T());

        max_bounds.x = glm::max(faces[i]->BBX_world->max_bound().x, max_bounds.x);
        max_bounds.y = glm::max(faces[i]->BBX_world->max_bound().y, max_bounds.y);
        max_bounds.z = glm::max(faces[i]->BBX_world->max_bound().z, max_bounds.z);

        min_bounds.x = glm::min(faces[i]->BBX_world->min_bound().x, min_bounds.x);
        min_bounds.y = glm::min(faces[i]->BBX_world->min_bound().y, min_bounds.y);
        min_bounds.z = glm::min(faces[i]->BBX_world->min_bound().z, min_bounds.z);
    }

    mesh_root = BVHNode(faces, 0);
    BBX = new BoundingBox(max_bounds, min_bounds);
}



void Mesh::LoadOBJ(const QStringRef &filename, const QStringRef &local_path)
{
    QString filepath = local_path.toString(); filepath.append(filename);
    std::vector<tinyobj::shape_t> shapes; std::vector<tinyobj::material_t> materials;
    std::string errors = tinyobj::LoadObj(shapes, materials, filepath.toStdString().c_str());
    std::cout << errors << std::endl;
    if(errors.size() == 0)
    {
        //Read the information from the vector of shape_ts
        for(unsigned int i = 0; i < shapes.size(); i++)
        {
            std::vector<float> &positions = shapes[i].mesh.positions;
            std::vector<float> &normals = shapes[i].mesh.normals;
            std::vector<float> &uvs = shapes[i].mesh.texcoords;
            std::vector<unsigned int> &indices = shapes[i].mesh.indices;
            for(unsigned int j = 0; j < indices.size(); j += 3)
            {
                glm::vec3 p1(positions[indices[j]*3], positions[indices[j]*3+1], positions[indices[j]*3+2]);
                glm::vec3 p2(positions[indices[j+1]*3], positions[indices[j+1]*3+1], positions[indices[j+1]*3+2]);
                glm::vec3 p3(positions[indices[j+2]*3], positions[indices[j+2]*3+1], positions[indices[j+2]*3+2]);

                Triangle* t = new Triangle(p1, p2, p3);
                if(normals.size() > 0)
                {
                    glm::vec3 n1(normals[indices[j]*3], normals[indices[j]*3+1], normals[indices[j]*3+2]);
                    glm::vec3 n2(normals[indices[j+1]*3], normals[indices[j+1]*3+1], normals[indices[j+1]*3+2]);
                    glm::vec3 n3(normals[indices[j+2]*3], normals[indices[j+2]*3+1], normals[indices[j+2]*3+2]);
                    t->normals[0] = n1;
                    t->normals[1] = n2;
                    t->normals[2] = n3;
                }
                if(uvs.size() > 0)
                {
                    glm::vec2 t1(uvs[indices[j]*2], uvs[indices[j]*2+1]);
                    glm::vec2 t2(uvs[indices[j+1]*2], uvs[indices[j+1]*2+1]);
                    glm::vec2 t3(uvs[indices[j+2]*2], uvs[indices[j+2]*2+1]);
                    t->uvs[0] = t1;
                    t->uvs[1] = t2;
                    t->uvs[2] = t3;
                }
                this->faces.append(t);
            }
        }
        std::cout << "" << std::endl;
    }
    else
    {
        //An error loading the OBJ occurred!
        std::cout << errors << std::endl;
    }
}

void Mesh::create(){
    //Count the number of vertices for each face so we can get a count for the entire mesh
        std::vector<glm::vec3> vert_pos;
        std::vector<glm::vec3> vert_nor;
        std::vector<glm::vec3> vert_col;
        std::vector<GLuint> vert_idx;

        unsigned int index = 0;

        for(int i = 0; i < faces.size(); i++){
            Triangle* tri = faces[i];
            vert_pos.push_back(tri->points[0]); vert_nor.push_back(tri->normals[0]); vert_col.push_back(tri->material->base_color);
            vert_pos.push_back(tri->points[1]); vert_nor.push_back(tri->normals[1]); vert_col.push_back(tri->material->base_color);
            vert_pos.push_back(tri->points[2]); vert_nor.push_back(tri->normals[2]); vert_col.push_back(tri->material->base_color);
            vert_idx.push_back(index++);vert_idx.push_back(index++);vert_idx.push_back(index++);
        }

        count = vert_idx.size();
        int vert_count = vert_pos.size();

        bufIdx.create();
        bufIdx.bind();
        bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufIdx.allocate(vert_idx.data(), count * sizeof(GLuint));

        bufPos.create();
        bufPos.bind();
        bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufPos.allocate(vert_pos.data(), vert_count * sizeof(glm::vec3));

        bufCol.create();
        bufCol.bind();
        bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufCol.allocate(vert_col.data(), vert_count * sizeof(glm::vec3));

        bufNor.create();
        bufNor.bind();
        bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufNor.allocate(vert_nor.data(), vert_count * sizeof(glm::vec3));
}

//This does nothing because individual triangles are not rendered with OpenGL; they are rendered all together in their Mesh.
void Triangle::create(){}
