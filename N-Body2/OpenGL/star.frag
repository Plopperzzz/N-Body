#version 330 core
out vec4 FragColor;

in vec4 color;

void main()
{
	// Calculate the distance from the center of the point
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = dot(coord, coord);

    float radius = 0.005;

    // Create a radial gradient to make it circular and add blur
    float alpha = smoothstep(radius * radius, radius + 0.01, dist); // Adjust values for blur effect

    if(dist > radius + 0.05)
    {
        discard;
    } 

    FragColor = vec4(color.rgb*sin(alpha)/(alpha * alpha), 1-sin(alpha) * sin(alpha));
    //FragColor = vec4(0.0);
};