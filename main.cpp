#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "shaderinit.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

const unsigned int screen_width = 1200;
const unsigned int screen_height = 800;

const GLuint n_vertices = 36;
const int N_CUBES = 4;
GLuint shader_id;
GLFWwindow* window;
GLuint VAO;
GLuint VBO;
int selected_cube_idx = -1;
bool is_scaling = false;
bool is_centered = false;
glm::vec3 cube_pos[N_CUBES] = {
  glm::vec3(-5.0f, 4.0f, -6.0f),
  glm::vec3(5.0f, 4.0f, -6.0f),
  glm::vec3(-5.0f, -4.0f, -6.0f),
  glm::vec3(5.0f, -4.0f, -6.0f)
};
glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

float delta_time = 0.0f;
float last_frame = 0.0f;
float camera_speed = 3 * delta_time;

bool first_mouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float last_x = (float)screen_width / 2.0;
float last_y = (float)screen_height / 2.0;
float fov = 45.0f;

bool has_texture = false;
GLuint texture_id;
float cube_colors[N_CUBES][4] = {
  { 1.0f, 0.0f, 0.0f, 1.0f },
  { 0.0f, 1.0f, 0.0f, 1.0f },
  { 0.0f, 0.0f, 1.0f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f }
};

void key_cb(GLFWwindow* windo, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    // reset scale when switching between cubes
    is_scaling = false;
    is_centered = false;
    selected_cube_idx = rand() % N_CUBES;
  }

  if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
    // resets all cubes to original positions
    selected_cube_idx = -1;
  }

  if (key == GLFW_KEY_S && action == GLFW_PRESS) {
    if (selected_cube_idx > -1) {
      // on/off switch
      is_scaling = !is_scaling;
    }
  }

  if (key == GLFW_KEY_W && action == GLFW_PRESS) {
    if (selected_cube_idx > -1) {
      is_centered = !is_centered;
    }
  }

  if (key == GLFW_KEY_T && action == GLFW_PRESS) {
    has_texture = true;
  }

  if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
    has_texture = false;
  }
}

void mouse_cb(GLFWwindow* windo, double x_pos, double y_pos) {
  if (first_mouse) {
    last_x = x_pos;
    last_y = y_pos;
    first_mouse = false;
  }

  float x_offset = x_pos - last_x;
  float y_offset = y_pos - last_y;
  last_x = x_pos;
  last_y = y_pos;

  float sensitivity = 0.1f;
  x_offset *= sensitivity;
  y_offset *= sensitivity;

  yaw += x_offset;
  pitch += y_offset;

  if (pitch > 89.0f) {
    pitch = 89.0f;
  }

  if (pitch < -89.0f) {
    pitch = -89.0f;
  }

  glm::vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  camera_front = glm::normalize(front);
}

void scroll_cb(GLFWwindow* windo, double x_offset, double y_offset) {
  fov -= (float)y_offset;
  if (fov < 1.0f) {
    fov = 1.0f;
  }

  if (fov > 45.0f) {
    fov = 45.0f;
  }
}

void handle_input() {
  if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
	  glfwSetWindowShouldClose(window, true);
  }

  float camera_speed = 3 * delta_time;
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    camera_pos += camera_speed * camera_front;
  }

  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    camera_pos -= camera_speed * camera_front;
  }

  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
  }

  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
  }

  glfwSetKeyCallback(window, key_cb);
  glfwSetCursorPosCallback(window, mouse_cb);
  glfwSetScrollCallback(window, scroll_cb);
}

int main() {
	if (!glfwInit()) {
		fprintf(stderr, "Failed to init GLFW");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	window = glfwCreateWindow(screen_width, screen_height, "OpenGLAssignment1", NULL, NULL);

	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to init GLAD" << endl;
		return -1;
	}

  glEnable(GL_DEPTH_TEST);

	Shader ourShader("shader.vs", "shader.fs");
	shader_id = ourShader.id;

	// cube coords
	float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
   -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	};

	glGenVertexArrays(1, &VAO);
	// allocate buffer to draw triangle
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	// attach buffer to graphics layer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// now draw
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  glEnableVertexAttribArray(glGetAttribLocation(ourShader.id, "uv_in"));

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  glEnable(GL_TEXTURE_2D);
	ourShader.use();

	while (!glfwWindowShouldClose(window)) {
	  float current_frame = glfwGetTime();
	  delta_time = current_frame - last_frame;
	  last_frame = current_frame;

	  handle_input();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
    glm::mat4 projection = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -6.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)screen_width / (float)screen_height, 0.1f, 100.0f);

    unsigned int view_loc = glGetUniformLocation(ourShader.id, "view");

    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(ourShader.id, "projection"), 1, GL_FALSE, &projection[0][0]);

		for (int i = 0; i < N_CUBES; i++) {
      glm::mat4 cube_model = glm::mat4(1.0f);

      if (i == selected_cube_idx) {
        if (is_centered) {
          cube_model = glm::translate(cube_model, glm::vec3(0.0f, 0.0f, -6.0f));
        } else {
          cube_model = glm::translate(cube_model, cube_pos[i]);
        }

        cube_model = glm::rotate(cube_model, (float)glfwGetTime() * glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        if (is_scaling) {
          cube_model = glm::scale(cube_model, glm::vec3(2.5f, 2.5f, 2.5f));
        }
      } else {
        cube_model = glm::translate(cube_model, cube_pos[i]);
        cube_model = glm::rotate(cube_model, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
      }

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glUniform1i(glGetUniformLocation(ourShader.id, "tex"), 0);

      glUniformMatrix4fv(glGetUniformLocation(ourShader.id, "model"), 1, GL_FALSE, &cube_model[0][0]);
		  glDrawArrays(GL_TRIANGLES, 0, n_vertices);
		}

		glfwSwapBuffers(window);
	  glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
