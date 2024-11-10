#include "Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <iostream>
#include <vector>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::ifstream vertexSourceFile;
	vertexSourceFile.open(vertexPath);
	std::string vertexSource;
	std::string line;

	if (!vertexSourceFile.is_open())
	{
		std::cout << "COULD NOT OPEN VERTEX SHADER SOURCE FILE!\n";
	}

	while (getline(vertexSourceFile, line))
	{
		vertexSource += line + '\n';
	}

	const char* vertexChar = vertexSource.c_str();
	
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexChar, NULL);
	glCompileShader(vertexShader);

	int vertexCompiled = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexCompiled);

	if (vertexCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &errorLog[0]);

		std::cout << "Failed to compile vertex shader.\n";

		for (char c : errorLog) std::cout << c;
		std::cout << '\n';
		glDeleteShader(vertexShader);
	}

	std::ifstream fragmentSourceFile(fragmentPath);
	std::string fragmentSource;
	while (getline(fragmentSourceFile, line))
	{
		fragmentSource += line + '\n';
	}
	const char* fragChar = fragmentSource.c_str();

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragChar, NULL);
	glCompileShader(fragmentShader);

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	m_ID = shaderProgram;

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR: SHADER PROGRAM LINKING FAILED:\n" << infoLog << '\n';
	}
}

Shader::~Shader()
{
	glDeleteProgram(m_ID);
}

void Shader::Use()
{
	glUseProgram(m_ID);
}
