#version 330 core
out vec4 FragColor;

in vec4 color;

void main()
{
	// Calculate the distance from the center of the point
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);

    // Create a radial gradient to make it circular and add blur
    float alpha = smoothstep(5, 0.45, dist); // Adjust values for blur effect

    // Set color and transparency
    FragColor = color; // White color with alpha channel
};
