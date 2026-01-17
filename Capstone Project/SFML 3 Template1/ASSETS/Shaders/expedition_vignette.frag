// expedition_vignette.frag
// Health/BPM based vignette for combat

uniform vec2 u_resolution;      // Screen size
uniform vec3 u_healthColor;     // Current health color (RGB 0-1)
uniform float u_healthRatio;    // Health ratio 0-1
uniform float u_bpmPulse;       // BPM pulse 0-1
uniform float u_intensity;      // Overall intensity

void main() {
    // Get normalized screen coordinates (0 to 1)
    vec2 uv = gl_FragCoord.xy / u_resolution;
    
    // Calculate distance from center
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(uv, center);
    
    // Create vignette effect (dark at edges, clear in center)
    float vignette = smoothstep(0.8, 0.2, dist);
    
    // Invert for edge glow effect
    float edgeGlow = 1.0 - vignette;
    
    // Add BPM pulse
    float pulse = u_bpmPulse * 0.6 + 0.4; // 0.4 to 1.0
    
    // Calculate alpha based on health and distance from edge
    float alpha = edgeGlow * (1.0 - u_healthRatio * 0.5) * pulse * u_intensity;
    
    // Output color with edge glow
    gl_FragColor = vec4(u_healthColor, alpha);
}
