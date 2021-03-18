#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

class Shader {
  public:
	  unsigned int id;

	  Shader(const char* vertex_path, const char* fragment_path) {
		  string vertex_code;
		  string fragment_code;
		  ifstream v_shader_file;
		  ifstream f_shader_file;

		  v_shader_file.exceptions(ifstream::failbit | ifstream::badbit);
		  f_shader_file.exceptions(ifstream::failbit | ifstream::badbit);

		  try {
			  v_shader_file.open(vertex_path);
			  f_shader_file.open(fragment_path);

			  stringstream v_shader_stream;
			  stringstream f_shader_stream;

			  v_shader_stream << v_shader_file.rdbuf();
			  f_shader_stream << f_shader_file.rdbuf();

			  v_shader_file.close();
			  f_shader_file.close();

			  vertex_code = v_shader_stream.str();
			  fragment_code = f_shader_stream.str();
		  }
		  catch (ifstream::failure& err) {
			  cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << endl;
		  }

		  const char* v_shader_code = vertex_code.c_str();
		  const char* f_shader_code = fragment_code.c_str();

		  unsigned int vertex;
		  unsigned int fragment;

		  vertex = glCreateShader(GL_VERTEX_SHADER);
		  glShaderSource(vertex, 1, &v_shader_code, NULL);
		  glCompileShader(vertex);

		  checkCompileErrors(vertex, "VERTEX");

		  fragment = glCreateShader(GL_FRAGMENT_SHADER);
		  glShaderSource(fragment, 1, &f_shader_code, NULL);
		  glCompileShader(fragment);

		  checkCompileErrors(fragment, "FRAGMENT");

		  id = glCreateProgram();
		  glAttachShader(id, vertex);
		  glAttachShader(id, fragment);
		  glLinkProgram(id);

		  checkCompileErrors(id, "PROGRAM");

		  glDeleteShader(vertex);
		  glDeleteShader(fragment);
	  }

	  void use() {
		  glUseProgram(id);
	  }

	  void setBool(const string& name, bool value) {
		  glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
	  }

	  void setInt(const string& name, int value) {
		  glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	  }

	  void setFloat(const string& name, float value) {
		  glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	  }

    private:
		void checkCompileErrors(unsigned int shader, string type) {
			int success;
			char info_log[1024];

			if (type != "PROGRAM") {
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

				if (!success) {
					glGetShaderInfoLog(shader, 1024, NULL, info_log);
					cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << info_log  << "\n --------------------------------------------" << endl;
				}
			}
			else {
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if (!success) {
					glGetProgramInfoLog(shader, 1024, NULL, info_log);
					cout << "ERROR::PROGRAM_LINKING of type: " << type << "\n" << info_log << "\n --------------------------------------------------" << endl;
				}
			}
		}
};

#endif
