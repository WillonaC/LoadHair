/*
文件标识符“IND_HAIR” 头发根数（unsigned） 总顶点个数（unsigned） 第一条头发顶点个数（unsigned） 顶点1（x，y，z）（float） 顶点2（x，y，z） 。。。 第二条头发定点个数 。。。
二进制文件，中间没有空格。

// Write hair file
// Python
// fiber_vert_counts: [第一根头发顶点数，第二根头发顶点数，。。。]
// bundle_verts：[顶点1，顶点2，。。。]
def _ind_hair_writer(hairfile,fiber_vert_counts,bundle_verts):
    with open(hairfile, 'wb') as fp:
        fp.write(b'IND_HAIR')
        num_fibers,num_bundle_verts = len(fiber_vert_counts),len(bundle_verts)
        fp.write(struct.pack('<II', num_fibers, num_bundle_verts))
        kthbvert = 0
        for num_verts in fiber_vert_counts:
            fp.write(struct.pack('<I', num_verts))
            for __ in range(num_verts):
                vert = bundle_verts[kthbvert]
                fp.write(struct.pack('<fff', vert[0],vert[1],vert[2]))
                kthbvert += 1
*/

#include <iostream>
#include <fstream>
#include <vector>
#include "OpenGL/glew.h"
#include "OpenGL/glut.h"
#include "OpenGL/vmath.h"
#include "OpenGL/mat.h"
#include "OpenGL/GLFW/glfw3.h"
#include "OpenGL/glm/glm.hpp"
#include "OpenGL/glm/gtc/matrix_transform.hpp"
#include "OpenGL/glm/gtc/type_ptr.hpp"
//// OpenGL GLFW 全局变量
int	WindowWidth = 800;
int WindowHeight = 800;
const char* vertexShaderSource =
"#version 440\n"
"layout (location=0) in vec3 position;\n"
"void main(){\n"
"gl_Position = vec4(position.x,position.y,position.z,1.0);\n"
"}\0";
const char* fragmentShaderSource =
"#version 440\n"
"out vec4 color;\n"
"void main(){\n"
"color=vec4(1.0,1.0,1.0,1.0);\n"
"}\0";

// Read hair file
// C++
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

void main()
{
	//初始化glfw窗口
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);   //不可改变大小  
	glfwWindowHint(GLFW_DECORATED, GL_TRUE);   //有边框和标题栏 

	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "Light Stage Render", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	//初始化glew扩展库  
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cout << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
	}

	AssetDesc hair = LoadHair("comic-hair.ind", 1);
	std::cout << hair.fiber_vert_counts.size() << std::endl;

	//shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint shaderProgramme = glCreateProgram();
	glAttachShader(shaderProgramme, vertexShader);
	glAttachShader(shaderProgramme, fragmentShader);
	glLinkProgram(shaderProgramme);

	glGetShaderiv(shaderProgramme, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgramme, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAMME::LINK_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	float *pointPos;
	pointPos = new float[hair.positions.size() * 3];
	float minx = 999, miny = 999, minz = 999, maxx = -999, maxy = -999, maxz = -999;
	for (int i = 0; i < hair.positions.size(); i++)
	{
		pointPos[i * 3] = hair.positions[i][0];
		pointPos[i * 3 + 1] = hair.positions[i][1];
		pointPos[i * 3 + 2] = hair.positions[i][2];

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
	std::cout << "model: " << std::endl;
	std::cout << "x: " << minx << "\t" << maxx << std::endl;
	std::cout << "y: " << miny << "\t" << maxy << std::endl;
	std::cout << "z: " << minz << "\t" << maxz << std::endl;
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointPos), pointPos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClearDepth(1.0f);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glUseProgram(shaderProgramme);
		glBindVertexArray(vao);
		for (int accsum = 0, i = 0; i < hair.fiber_vert_counts.size(); i++)
		{
			int verts_per_fiber = hair.fiber_vert_counts[i];
			glDrawArrays(GL_LINE_STRIP, accsum, verts_per_fiber);
			accsum += verts_per_fiber;
			//if (i == hair.fiber_vert_counts.size() - 1)
				//std::cout << "***" << std::endl;
		}
		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}
}