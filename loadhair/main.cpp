#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iomanip>
#include "tiny_obj_loader.h"
#include "camera.h"
#include "Shader.h"

#include "OpenGL/glut.h"
#include "OpenGL/vmath.h"
#include "OpenGL/mat.h"
#include "OpenGL/glew.h"
#include "OpenGL/GLFW/glfw3.h"
#include "OpenGL/glm/glm.hpp"
#include "OpenGL/glm/gtc/matrix_transform.hpp"
#include "OpenGL/glm/gtc/type_ptr.hpp"

//// OpenGL GLFW 全局变量
int	WindowWidth = 800;
int WindowHeight = 800;

glm::vec3 tmp = glm::vec3(0.0, 0.0, 30.0);
Camera camera(tmp, -tmp.x, -tmp.y, -tmp.z);

// model rotation & translation
float RX = 0.0;
float RY = 0.0;
float RZ = 0.0;
float rotate_degree = 0.0;
float ModelRotate_X = RX + 0.0;
float ModelRotate_Y = RY + 0.0;
float ModelRotate_Z = RZ + 0.0;
float ModelTranslate_X = 0.0;
float ModelTranslate_Y = -8.0;
float ModelTranslate_Z = 0.0;

//// Shader 全局变量
Shader ShaderProgram;//顶点着色器对象  
GLuint VAO;//vertex array object  
GLuint VBO_Vertex;


vmath::mat4 ModelM, ProjM;
glm::mat4 CameraM, ModelMM, ProjMM;

// mouse movement
bool MouseButtonLeftPress_Flag = false;
bool MouseButtonRightPress_Flag = false;
bool MouseButtonMiddlePress_Flag = false;
int CursorStartPos_X;
int CursorStartPos_Y;
int CursorPos_X = 0;
int CursorPos_Y = 0;

GLfloat deltaTime = 0.0f;	//当前帧与上一帧的时间差
GLfloat lastFrame = 0.0f;	//上一帧的时间
bool keys[1024];
float PI = 3.1415926;

void Keyboard_CallBack(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	else if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || GLFW_REPEAT))
	{
		ModelRotate_Y = ModelRotate_Y + 0.1;
		if (ModelRotate_Y >= 360) ModelRotate_Y = 0;
	}
	else if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || GLFW_REPEAT))
	{
		ModelRotate_Y = ModelRotate_Y - 0.1;
		if (ModelRotate_Y <= 0) ModelRotate_Y = 360;
	}
	else if (key == GLFW_KEY_UP && (action == GLFW_PRESS || GLFW_REPEAT))
	{
		ModelRotate_X = ModelRotate_X + 0.1;
		if (ModelRotate_X >= 360) ModelRotate_X = 0;
	}
	else if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || GLFW_REPEAT))
	{
		ModelRotate_X = ModelRotate_X - 0.1;
		if (ModelRotate_X <= 0) ModelRotate_X = 360;
	}
	else if (key == GLFW_KEY_N && (action == GLFW_PRESS || GLFW_REPEAT))
	{
		ModelRotate_Z = ModelRotate_Z + 0.1;
		if (ModelRotate_Z >= 360) ModelRotate_Z = 0;
	}
	else if (key == GLFW_KEY_M && (action == GLFW_PRESS || GLFW_REPEAT))
	{
		ModelRotate_Z = ModelRotate_Z - 0.1;
		if (ModelRotate_Z <= 0) ModelRotate_Z = 360;
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void do_movement()
{
	//摄像机
	if (keys[GLFW_KEY_W])
	{
		//camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_S])
	{
		//camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A])
	{
		//camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_D])
	{
		//camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	if (keys[GLFW_KEY_I])
	{
		camera.ProcessKeyboard(UPLOOK, deltaTime);
	}
	if (keys[GLFW_KEY_K])
	{
		camera.ProcessKeyboard(DOWNLOOK, deltaTime);
	}
	if (keys[GLFW_KEY_J])
	{
		camera.ProcessKeyboard(LEFTLOOK, deltaTime);
	}
	if (keys[GLFW_KEY_L])
	{
		camera.ProcessKeyboard(RIGHTLOOK, deltaTime);
	}

}

void Mouse_Button_Callback(GLFWwindow* window, int button, int action, int mods)
{
	//// 处理鼠标左键
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT)
	{
		MouseButtonLeftPress_Flag = true;
		CursorStartPos_X = CursorPos_X;
		CursorStartPos_Y = CursorPos_Y;
		/*
		std::cout << CursorPos_X << "   --   " << CursorPos_Y << std::endl;
		GLint    viewport[4];
		GLdouble modelview[16];
		GLdouble projection[16];
		glGetIntegerv(GL_VIEWPORT, viewport); // 得到的是最后一个设置视口的参数
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		float winz;
		glReadPixels(CursorPos_X, WindowHeight -1 - CursorPos_Y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winz);
		if (winz != 1)
		{
			std::cout << "winz: " << winz << std::endl;
			GLdouble object_x, object_y, object_z;
			gluUnProject((GLdouble)CursorPos_X, (GLdouble)WindowHeight - 1 - CursorPos_Y, (GLdouble)winz, modelview, projection, viewport, &object_x, &object_y, &object_z);
			std::cout << "3d: " << object_x << " " << object_y << " " << object_z << std::endl;
			glm::vec4 obj_pos = glm::vec4(object_x, object_y, object_z, 1);
			glm::vec4 obj_position = glm::inverse(ProjMM * CameraM * ModelMM) * obj_pos;
			obj_position /= obj_position[3];
			std::cout << "new 3d: " << obj_position[0] << " " << obj_position[1] << " " << obj_position[2] << " " << obj_position[3] << std::endl;

			float pix[3] = { 0.0 };
			glReadPixels(CursorPos_X, WindowHeight - 1 - CursorPos_Y, 1, 1, GL_RGB, GL_FLOAT, pix);
			std::cout << pix[0] * 20 << " " << pix[1] * 20 << " " << pix[2] * 20 << std::endl;

		}
		*/
	}

	if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT)
	{
		MouseButtonLeftPress_Flag = false;
	}



	//// 处理鼠标右键
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		MouseButtonRightPress_Flag = true;
		CursorStartPos_X = CursorPos_X;
		CursorStartPos_Y = CursorPos_Y;
	}

	if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		MouseButtonRightPress_Flag = false;
	}



	//// 处理鼠标中键
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		MouseButtonMiddlePress_Flag = true;
		CursorStartPos_X = CursorPos_X;
		CursorStartPos_Y = CursorPos_Y;
	}

	if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		MouseButtonMiddlePress_Flag = false;
	}

}

void Mouse_Cursor_Callback(GLFWwindow* window, double x, double y)
{
	CursorPos_X = int(x);
	CursorPos_Y = int(y);

	//// 模型旋转
	if (MouseButtonLeftPress_Flag == true)
	{
		//ModelRotate_Y = ModelRotate_Y + (float(CursorPos_X - CursorStartPos_X))  * 0.1;  // 鼠标移动窗口的宽度，模型水平旋转90度
		//ModelRotate_X = ModelRotate_X + (float(CursorPos_Y - CursorStartPos_Y))  * 0.1;  // 鼠标移动窗口的高度，模型垂直旋转90度
		camera.ProcessKeyboard(LEFTLOOK, (float(CursorPos_X - CursorStartPos_X))  * 0.01);
		camera.ProcessKeyboard(DOWNLOOK, (float(CursorPos_Y - CursorStartPos_Y))  * 0.01);

		CursorStartPos_X = CursorPos_X;
		CursorStartPos_Y = CursorPos_Y;

	}


	//// 模型移动

	if (MouseButtonRightPress_Flag == true)
	{
		ModelTranslate_X = ModelTranslate_X + (float(CursorPos_X - CursorStartPos_X)) / WindowWidth * 90;  // 鼠标移动窗口的宽度，模型水平旋转90度
		ModelTranslate_Y = ModelTranslate_Y + (float(-CursorPos_Y + CursorStartPos_Y)) / WindowHeight * 90;  // 鼠标移动窗口的高度，模型垂直旋转90度

		CursorStartPos_X = CursorPos_X;
		CursorStartPos_Y = CursorPos_Y;

	}


	return;
}

void Mouse_Scroll_Callback(GLFWwindow* window, double x, double y)
{
	if (camera.Fov >= 1.0f && camera.Fov <= 80.0f)
		camera.Fov -= y;
	if (camera.Fov <= 1.0f)
		camera.Fov = 1.0f;
	if (camera.Fov >= 80.0f)
		camera.Fov = 80.0f;
}

struct AssetDesc {
	std::vector<glm::vec3> positions;
	// Poly
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;
	// Hair
	std::vector<glm::vec3> tangents;
	std::vector<float> distp;
	std::vector<unsigned> fiber_vert_counts;

	std::vector<std::string> texpaths;
};

AssetDesc LoadHair(std::string path, unsigned sample_rate_denom)
{
	std::ifstream fp(path, std::ios::binary);

	char header[9];
	fp.read(header, 8);
	header[8] = '\0';

	auto read_unsigned = [&fp]()->unsigned {
		unsigned n;
		fp.read(reinterpret_cast<char*>(&n), sizeof(n));
		return n;
	};

	auto read_float = [&fp]()->float {
		float n;
		fp.read(reinterpret_cast<char*>(&n), sizeof(n));
		return n;
	};

	unsigned num_fibs = read_unsigned();
	unsigned num_verts = read_unsigned();

	float keep_ratio = 1.f / sample_rate_denom;

	std::vector<unsigned> fiber_vert_counts;
	std::vector<glm::vec3> positions;
	for (int kthfib = 1; kthfib <= num_fibs; ++kthfib) {
		auto fibv_count = read_unsigned();
		fiber_vert_counts.push_back(fibv_count);
		for (unsigned kthp = 0; kthp < fibv_count; ++kthp) {
			float x = read_float();
			float y = read_float();
			float z = read_float();
			positions.emplace_back(x, y, z);
		}
	}

	// Calculate smooth tangents.
	// Each tangents is the average of the pre&next.
	std::vector<glm::vec3> tangents(num_verts);
	for (std::size_t kthvert = 0,
		kthfib = 0; kthfib < fiber_vert_counts.size(); ++kthfib) {
		// First vert.
		tangents[kthvert] = glm::normalize(positions[kthvert + 1] - positions[kthvert]);
		++kthvert;

		// Middle verts who have pre&next.
		for (std::size_t i = 1; i < fiber_vert_counts[kthfib] - 1; ++i) {
			auto pre_tangent = glm::normalize(positions[kthvert] - positions[kthvert - 1]);
			auto next_tangent = glm::normalize(positions[kthvert + 1] - positions[kthvert]);
			tangents[kthvert] = glm::normalize(pre_tangent + next_tangent);
			++kthvert;
		}
		// Final vert, echo first vert.
		tangents[kthvert] = glm::normalize(positions[kthvert] - positions[kthvert - 1]);
		++kthvert;
	}

	// Calculate scales
	std::vector<float> distp(num_verts);
	{
		int kthvert = 0;
		for (auto num_verts : fiber_vert_counts)
			for (int i = 0; i < num_verts; ++i)
				distp[kthvert++] = static_cast<float>(i) / static_cast<float>(num_verts - 1);
	}

	AssetDesc congo;
	congo.fiber_vert_counts.swap(fiber_vert_counts);
	congo.tangents.swap(tangents);
	congo.distp.swap(distp);
	congo.positions.swap(positions);
	return congo;
}

int main(int argc, char** argv)
{
	//初始化glfw窗口
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);   //不可改变大小  
	glfwWindowHint(GLFW_DECORATED, GL_TRUE);   //有边框和标题栏 

	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "Light Stage Render", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, Keyboard_CallBack);
	glfwSetMouseButtonCallback(window, Mouse_Button_Callback);
	glfwSetCursorPosCallback(window, Mouse_Cursor_Callback);
	glfwSetScrollCallback(window, Mouse_Scroll_Callback);
	glewExperimental = GL_TRUE;

	//初始化glew扩展库  
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cout << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
	}

	ShaderVersion();

	//load model
	AssetDesc hair = LoadHair("comic-hair.ind", 1);
	int NumVertex = hair.positions.size();
	std::cout << hair.fiber_vert_counts.size() << std::endl;
	std::cout << hair.positions.size() << std::endl;

	// GL Functions
	//glEnable(GL_CULL_FACE); // 开启剔除操作效果
	//glCullFace(GL_BACK); //背面剔除，不做渲染，节省计算量
	//glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);	//GL_SMOOTH: interpolation with the colors of vertice
								//GL_FLAT: fill with the color of the last vertex
								//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE);
	//glBlendFuncSeparate(GL_ONE_MINUS_DST_COLOR, GL_ONE, GL_ONE_MINUS_DST_COLOR, GL_ONE);
	//glBlendEquation(GL_MAX);
	//glBlendEquation(GL_FUNC_ADD);

	///// Shader Program
	ShaderProgram = Shader("shader.vs", "shader.frag");

	//产生VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// 产生VBO 
	glGenBuffers(1, &VBO_Vertex);

	//加载顶点数据到VBO 
	GLint VertexAttrib = glGetAttribLocation(ShaderProgram.Program, "Vertex");
	GLint ColorAttrib = glGetAttribLocation(ShaderProgram.Program, "Color");

	float *pointPos;
	
	pointPos = new float[NumVertex * 3];
	float minx = 999, miny = 999, minz = 999, maxx = -999, maxy = -999, maxz = -999;
	for (int i = 0; i < NumVertex; i++)
	{
		pointPos[i * 3] = hair.positions[i][0];
		pointPos[i * 3 + 1] = hair.positions[i][1];
		pointPos[i * 3 + 2] = hair.positions[i][2];
		{
			if (minx > pointPos[i * 3])
				minx = pointPos[i * 3];
			if (maxx < pointPos[i * 3])
				maxx = pointPos[i * 3];
			if (miny > pointPos[i * 3 + 1])
				miny = pointPos[i * 3 + 1];
			if (maxy < pointPos[i * 3 + 1])
				maxy = pointPos[i * 3 + 1];
			if (minz > pointPos[i * 3 + 2])
				minz = pointPos[i * 3 + 2];
			if (maxz < pointPos[i * 3 + 2])
				maxz = pointPos[i * 3 + 2];
		}
	}
	{
		std::cout << "model: " << std::endl;
		std::cout << "x: " << minx << "\t" << maxx << std::endl;
		std::cout << "y: " << miny << "\t" << maxy << std::endl;
		std::cout << "z: " << minz << "\t" << maxz << std::endl;
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO_Vertex);
	glBufferData(GL_ARRAY_BUFFER, NumVertex * 3 * sizeof(GLfloat), pointPos, GL_STATIC_DRAW);
	glVertexAttribPointer(VertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)NULL);
	glEnableVertexAttribArray(VertexAttrib);//顶点坐标

	glBindVertexArray(0); // Unbind VAO
	//loop
	int flag = 2294*32;
	glViewport(0, 0, WindowWidth, WindowHeight);
	while (!glfwWindowShouldClose(window))
	{
		glClearDepth(1.0f);
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();
		do_movement();

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		ShaderProgram.Use();


		//cout << "ModelRotate_X: " << ModelRotate_X << endl << "ModelRotate_Y: " << ModelRotate_Y << endl << "ModelRotate_Z: " << ModelRotate_Z << endl << endl;
		//cout << "ModelTranslate_X: " << ModelTranslate_X << endl << "ModelTranslate_Y: " << ModelTranslate_Y << endl << "ModelTranslate_Z: " << ModelTranslate_Z << endl << endl;
		//cout << "FOV: " << camera.Fov << endl;
		//std::cout << camera.Position[0] <<" "<< camera.Position[1] << " " << camera.Position[2] <<" "<< std::endl;
		vmath::mat4 ModelRotate = vmath::rotate(ModelRotate_X, ModelRotate_Y, ModelRotate_Z);
		vmath::mat4 ModelTranslate = vmath::translate(ModelTranslate_X, ModelTranslate_Y, ModelTranslate_Z);
		vmath::mat4 ModelScale = vmath::scale(0.5f, 0.5f, 0.5f);
		ModelM = ModelTranslate * ModelRotate * ModelScale;

		glUniformMatrix4fv(glGetUniformLocation(ShaderProgram.Program, "ModelM"), 1, GL_FALSE, ModelM);

		//std::cout << "modelM" << std::endl;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				ModelMM[i][j] = ModelM[i][j];
				//std::cout << ModelM[i][j] << " ";
			}
			//std::cout << std::endl;
		}

		CameraM = camera.GetViewMatrix();;
		glUniformMatrix4fv(glGetUniformLocation(ShaderProgram.Program, "CameraM"), 1, GL_FALSE, glm::value_ptr(CameraM));

		//std::cout << "CameraM" << std::endl;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				//std::cout << CameraM[i][j] << " ";
			}
			//std::cout << std::endl;
		}

		ProjM = (vmath::perspective(camera.Fov, (GLfloat)WindowWidth / (GLfloat)WindowHeight, 1.0f, 100.0f));
		glUniformMatrix4fv(glGetUniformLocation(ShaderProgram.Program, "ProjM"), 1, GL_FALSE, ProjM);

		//std::cout << "ProjM" << std::endl;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				ProjMM[i][j] = ProjM[i][j];
				//std::cout << ProjM[i][j] << " ";
			}
			//std::cout << std::endl;
		}

		////使用VAO、VBO绘制
		glBindVertexArray(VAO);
		
		//for (int accsum = 2294 * 32, i = 2294; i < 2295; i++)
		for (int accsum = 0, i = 0; i < hair.fiber_vert_counts.size(); i++)
		{
			//if (i == 2294)
			//{
				//accsum += 32;
				//continue;
			//}
			int verts_per_fiber = hair.fiber_vert_counts[i];
			glDrawArrays(GL_LINE_STRIP, accsum, verts_per_fiber);
			accsum += verts_per_fiber;
			//if (i == hair.fiber_vert_counts.size() - 1)
			//std::cout << "***" << std::endl;
		}
		//glDrawArrays(GL_LINE_STRIP, 0,2); // 73417
		glBindVertexArray(0);

		flag++;
		//交换缓冲
		glfwSwapBuffers(window);
	}


	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO_Vertex);
	glfwTerminate();
	return 0;
}