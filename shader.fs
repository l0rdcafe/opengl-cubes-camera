# version 410 core
out vec3 FragColor;

in vec3 ourColor;
in vec2 tex_coord;

uniform sampler2D tex;

void main() {
  FragColor = texture(tex, tex_coord).rgb * ourColor;
}
