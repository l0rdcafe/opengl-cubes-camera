# version 410 core
out vec3 FragColor;

in vec3 ourColor;
in vec2 uv;

uniform sampler2D tex;

void main() {
  FragColor = texture(tex, uv).rgb * ourColor;
}
