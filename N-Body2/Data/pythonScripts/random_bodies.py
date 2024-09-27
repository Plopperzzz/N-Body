import json
import random
import argparse
import math

def generate_random_bodies(num_bodies, mass_range, radius_range, position_range, velocity_range):
    bodies = []
    
    for i in range(num_bodies):
        body = {
            "id": i,
            "name": f"Body_{i}",
            "mass": random.uniform(*mass_range),
            "radius": random.uniform(*radius_range),
            "position": [
                random.uniform(*position_range),
                random.uniform(*position_range),
                random.uniform(*position_range)
            ],
            "velocity": [
                random.uniform(*velocity_range),
                random.uniform(*velocity_range),
                random.uniform(*velocity_range)
            ],
            "force": [0.0, 0.0, 0.0]  # Assuming force starts at zero
        }
        bodies.append(body)
    
    return bodies

def generate_circular_bodies(num_bodies, mass_range, radius_range, circle_radius, circle_center, circle_plane):
    bodies = []
    
    for i in range(num_bodies):
        angle = 2 * math.pi * i / num_bodies  # Evenly spaced angles
        if circle_plane == 'xy':
            x = circle_center[0] + circle_radius * math.cos(angle)
            y = circle_center[1] + circle_radius * math.sin(angle)
            z = circle_center[2]
        elif circle_plane == 'xz':
            x = circle_center[0] + circle_radius * math.cos(angle)
            y = circle_center[1]
            z = circle_center[2] + circle_radius * math.sin(angle)
        elif circle_plane == 'yz':
            x = circle_center[0]
            y = circle_center[1] + circle_radius * math.cos(angle)
            z = circle_center[2] + circle_radius * math.sin(angle)
        else:
            raise ValueError("Invalid circle_plane. Choose from 'xy', 'xz', 'yz'.")
        
        # For velocity, you might want to set it tangentially for circular motion
        # Here, we'll assign random velocities unless specified otherwise
        body = {
            "id": i,
            "name": f"Body_{i}",
            "mass": random.uniform(*mass_range),
            "radius": random.uniform(*radius_range),
            "position": [x, y, z],
            "velocity": [
                random.uniform(-1e4, 1e4),  # You might want to adjust this for realistic circular motion
                random.uniform(-1e4, 1e4),
                random.uniform(-1e4, 1e4)
            ],
            "force": [0.0, 0.0, 0.0]  # Assuming force starts at zero
        }
        bodies.append(body)
    
    return bodies

def save_bodies_to_json(bodies, filename):
    data = {"bodies": bodies}
    with open(filename, 'w') as f:
        json.dump(data, f, indent=4)

if __name__ == "__main__":

    # Setup argparse to handle command-line arguments
    parser = argparse.ArgumentParser(description="Generate random bodies in space or arrange them on a circle")
    
    # Add arguments for number of bodies and ranges
    parser.add_argument("--num_bodies", type=int, default=5, help="Number of bodies to generate")
    parser.add_argument("--mass_range", type=float, nargs=2, default=[1e20, 1e30], help="Range for body mass (min max)")
    parser.add_argument("--radius_range", type=float, nargs=2, default=[1e3, 1e6], help="Range for body radius (min max)")
    parser.add_argument("--position_range", type=float, nargs=2, default=[-1e9, 1e9], help="Range for body position (min max)")
    parser.add_argument("--velocity_range", type=float, nargs=2, default=[-1e4, 1e4], help="Range for body velocity (min max)")
    parser.add_argument("--file", type=str, default="random_bodies.json", help="Output JSON file")
    parser.add_argument("--path", type=str, default="../", help="Output directory for JSON file")
    
    # Add arguments for arrangement
    parser.add_argument("--arrangement", type=str, choices=['random', 'circle'], default='random', help="Arrangement of bodies: 'random' or 'circle'")
    parser.add_argument("--circle_radius", type=float, default=None, help="Radius of the circle (required if arrangement is 'circle')")
    parser.add_argument("--circle_center", type=float, nargs=3, default=[0.0, 0.0, 0.0], help="Center of the circle as three floats (x y z)")
    parser.add_argument("--circle_plane", type=str, choices=['xy', 'xz', 'yz'], default='xy', help="Plane of the circle: 'xy', 'xz', or 'yz' (only relevant if arrangement is 'circle')")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Validate arguments
    if args.arrangement == 'circle' and args.circle_radius is None:
        parser.error("--circle_radius is required when --arrangement is 'circle'")
    
    # Generate bodies based on the arrangement
    if args.arrangement == 'random':
        bodies = generate_random_bodies(
            args.num_bodies,
            args.mass_range,
            args.radius_range,
            args.position_range,
            args.velocity_range
        )
    elif args.arrangement == 'circle':
        bodies = generate_circular_bodies(
            args.num_bodies,
            args.mass_range,
            args.radius_range,
            args.circle_radius,
            args.circle_center,
            args.circle_plane
        )
    else:
        # This should not happen due to argparse choices
        raise ValueError("Invalid arrangement option.")
    
    # Ensure the output path ends without a trailing slash
    output_path = args.path.rstrip('/')
    
    # Save the bodies to the specified output file
    save_bodies_to_json(bodies, f"{output_path}/{args.file}")
    
    print(f"Generated {args.num_bodies} bodies with arrangement '{args.arrangement}' and saved to {output_path}/{args.file}")
