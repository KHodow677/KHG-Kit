#version 450 core
out vec4 frag_color;
in vec2 TexCoord;
uniform sampler2D u_framebuffer_texture;
uniform float u_bloom_spread;
uniform float u_bloom_intensity;
uniform float u_time;
uniform float u_brightness_decrease;
uniform float u_noise_intensity;
float random(vec2 uv) {
  return fract(sin(dot(uv.xy, vec2(12.9898, 78.233))) * 43758.5453);
}
vec4 apply_noise(vec4 color, vec2 uv) {
  float noise = random(uv * u_time) * u_noise_intensity;
  return vec4(color.rgb + noise, color.a);
}
void main() {
  ivec2 size = textureSize(u_framebuffer_texture, 0);
  vec4 sum = vec4(0.0);
  for (int n = 0; n < 9; ++n) {
    float uv_y = (TexCoord.y * size.y) + (u_bloom_spread * float(n - 4));
    vec4 h_sum = vec4(0.0);
    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x - (4.0 * u_bloom_spread), uv_y), 0);
    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x - (3.0 * u_bloom_spread), uv_y), 0);
    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x - (2.0 * u_bloom_spread), uv_y), 0);
    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x - u_bloom_spread, uv_y), 0);
    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x, uv_y), 0);
    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x + u_bloom_spread, uv_y), 0);
    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x + (2.0 * u_bloom_spread), uv_y), 0);
    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x + (3.0 * u_bloom_spread), uv_y), 0);
    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x + (4.0 * u_bloom_spread), uv_y), 0);
    sum += h_sum / 9.0;
  }
  vec4 originalColor = texture(u_framebuffer_texture, TexCoord);
  vec4 bloomColor = (sum / 9.0) * u_bloom_intensity;
  vec4 noisyColor = apply_noise(originalColor + bloomColor, TexCoord);
  noisyColor.rgb -= u_brightness_decrease;
  frag_color = clamp(noisyColor, 0.0, 1.0);
}
