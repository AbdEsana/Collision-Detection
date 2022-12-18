#include "BasicScene.h"
#include <read_triangle_mesh.h>
#include <utility>
#include "ObjLoader.h"
#include "IglMeshLoader.h"
#include "igl/read_triangle_mesh.cpp"
#include "igl/edge_flaps.h"
#include <Eigen/Geometry>



// #include "AutoMorphingModel.h"

using namespace cg3d;

void BasicScene::Init(float fov, int width, int height, float near, float far)
{
    camera = Camera::Create( "camera", fov, float(width) / height, near, far);
    
    AddChild(root = Movable::Create("root")); // a common (invisible) parent object for all the shapes
    auto daylight{std::make_shared<Material>("daylight", "shaders/cubemapShader")}; 
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);
    auto background{Model::Create("background", Mesh::Cube(), daylight)};
  AddChild(background);
    background->Scale(120, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();

 
    auto program = std::make_shared<Program>("shaders/basicShader");
    auto material{ std::make_shared<Material>("material", program)}; // empty material
 
    material->AddTexture(0, "textures/box0.bmp", 2);
    //auto sphereMesh{IglLoader::MeshFromFiles("sphere_igl", "data/sphere.obj")};
    auto cylMesh{IglLoader::MeshFromFiles("cyl_igl","data/sphere.obj")};
    auto cubeMesh{IglLoader::MeshFromFiles("cube_igl","data/sphere.obj")};
    
    //sphere1 = Model::Create( "sphere",sphereMesh, material);
    cyl = Model::Create( "cyl", cylMesh, material);
    cube = Model::Create( "cube", cubeMesh, material);
  
    cyl->Translate({2,0,0});
	cube->Translate({-2,0,0 });
    //cyl->Scale(20.0f);
	//cube->Scale(20.0f);
    cyl->showWireframe = true;
	cyl->showFaces = false;
    cube->showWireframe = true;
	cube->showFaces = false;
    camera->Translate(10, Axis::Z);
 
    root->AddChild(cyl);
    root->AddChild(cube);
    
	
    auto mesh = cube->GetMeshList();
    Eigen::MatrixXi F;
    Eigen::MatrixXd V;
    V = mesh[0]->data[0].vertices;
    F = mesh[0]->data[0].faces;
	tree1.init(V, F);
	mesh = cyl->GetMeshList();
	V = mesh[0]->data[0].vertices;
	F = mesh[0]->data[0].faces;
	tree2.init(V, F);


}

void BasicScene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);
    program.SetUniform4f("lightColor", 1.0f, 1.0f, 1.0f, 0.5f);
    program.SetUniform4f("Kai", 1.0f, 1.0f, 1.0f, 1.0f);
    //cube->Rotate(0.01f, Axis::XYZ);
	
	cube->Translate(0.005f*dx, Axis::X);
	cube->Translate(0.005f*dy, Axis::Y);
	
}

void BasicScene::KeyCallback(Viewport* viewport, int x, int y, int key, int scancode, int action, int mods) 
{
	auto system = camera->GetRotation().transpose();

	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) // NOLINT(hicpp-multiway-paths-covered)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_UP:
			//camera->RotateInSystem(system, 0.1f, Axis::X);
			if (allowinput) {
				dx = 0;
				dy = 1;
			}
			break;
		case GLFW_KEY_DOWN:
			//camera->RotateInSystem(system, -0.1f, Axis::X);
			if (allowinput) {
				dx = 0;
				dy = -1;
			}
			break;
		case GLFW_KEY_LEFT:
			//camera->RotateInSystem(system, 0.1f, Axis::Y);
			if (allowinput) {
				dx = -1;
				dy = 0;
			}
			break;
		case GLFW_KEY_RIGHT:
			//camera->RotateInSystem(system, -0.1f, Axis::Y);
			if (allowinput) {
				dx = 1;
				dy = 0;
			}
			break;
		case GLFW_KEY_W:
			camera->TranslateInSystem(system, { 0, 0.05f, 0 });
			break;
		case GLFW_KEY_S:
			camera->TranslateInSystem(system, { 0, -0.05f, 0 });
			break;
		case GLFW_KEY_A:
			camera->TranslateInSystem(system, { -0.05f, 0, 0 });
			break;
		case GLFW_KEY_D:
			camera->TranslateInSystem(system, { 0.05f, 0, 0 });
			break;
		case GLFW_KEY_B:
			camera->TranslateInSystem(system, { 0, 0, 0.05f });
			break;
		case GLFW_KEY_F:
			camera->TranslateInSystem(system, { 0, 0, -0.05f });
			break;
		}
	}
}



