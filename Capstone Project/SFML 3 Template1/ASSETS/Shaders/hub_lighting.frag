// hub_lighting.frag
// Simulates dark hub with player as light source (replaces Candle)

uniform vec2 u_resolution;      // Screen size
uniform vec2 u_playerPosition;  // Player position in screen space
uniform float u_lightRange;     // How far the light reaches
uniform vec3 u_lightColor;      // Light color (RGB 0-1)
uniform float u_ambientDarkness; // How dark it is (0-1, higher = darker)

void main() {
    vec2 pixelPos = gl_FragCoord.xy;
    float dist = distance(pixelPos, u_playerPosition);
    float attenuation = 1.0 - smoothstep(0.0, u_lightRange, dist);
    attenuation = pow(attenuation, 2.0);
    float darkness = u_ambientDarkness * (1.0 - attenuation);
    vec3 finalColor = mix(vec3(0.0, 0.0, 0.0), u_lightColor, attenuation);
    gl_FragColor = vec4(finalColor, darkness);
}