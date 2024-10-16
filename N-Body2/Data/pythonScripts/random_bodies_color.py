
import json
import random
import argparse
import math

G = 6.67430e-11  # Gravitational constant

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
            "force": [0.0, 0.0, 0.0],  # Assuming force starts at zero
            "color": [
                random.uniform(0, 1),  # Red
                random.uniform(0, 1),  # Green
                random.uniform(0, 1),  # Blue
                random.uniform(0.5, 1) # Alpha (using 0.5 - 1 for better visibility)
            ]
        }
        bodies.append(body)
    
    return bodies

def generate_elliptical_bodies(num_bodies, mass_range, radius_range, min_radius, max_radius, circle_center, circle_plane, central_mass=1e30, eccentricity_range=(0.1, 0.5), start_id=0):
    bodies = []

    for i in range(num_bodies):
        angle = 2 * math.pi * i / num_bodies  # Evenly spaced angles

        # Generate random semi-major axis (a) and eccentricity (e) for the ellipse
        a = random.uniform(min_radius, max_radius)  # Semi-major axis (orbital distance)
        e = random.uniform(*eccentricity_range)  # Eccentricity (controls how elliptical the orbit is)
        b = a * math.sqrt(1 - e**2)  # Semi-minor axis

        # Parametric equation of an ellipse for position (r_x, r_y) on the chosen plane
        if circle_plane == 'xy':
            x = circle_center[0] + a * math.cos(angle)
            y = circle_center[1] + b * math.sin(angle)
            z = circle_center[2]
            r = math.sqrt((x - circle_center[0])**2 + (y - circle_center[1])**2)  # Radial distance
        elif circle_plane == 'xz':
            x = circle_center[0] + a * math.cos(angle)
            y = circle_center[1]
            z = circle_center[2] + b * math.sin(angle)
            r = math.sqrt((x - circle_center[0])**2 + (z - circle_center[2])**2)  # Radial distance
        elif circle_plane == 'yz':
            x = circle_center[0]
            y = circle_center[1] + a * math.cos(angle)
            z = circle_center[2] + b * math.sin(angle)
            r = math.sqrt((y - circle_center[1])**2 + (z - circle_center[2])**2)  # Radial distance
        else:
            raise ValueError("Invalid circle_plane. Choose from 'xy', 'xz', 'yz'.")

        # Calculate velocity for elliptical orbit
        # In elliptical orbits, velocity depends on position in orbit (Kepler's second law).
        velocity_magnitude = math.sqrt(G * central_mass * (2 / r - 1 / a))  # Elliptical velocity magnitude
        
        # Randomize velocity slightly to introduce non-perfect elliptical motion
        velocity_magnitude *= random.uniform(0.9, 1.1)

        # Determine tangential velocity direction based on the angle
        if circle_plane == 'xy':
            vx = -velocity_magnitude * math.sin(angle)
            vy = velocity_magnitude * math.cos(angle)
            vz = 0.0
        elif circle_plane == 'xz':
            vx = -velocity_magnitude * math.sin(angle)
            vy = 0.0
            vz = velocity_magnitude * math.cos(angle)
        elif circle_plane == 'yz':
            vx = 0.0
            vy = -velocity_magnitude * math.sin(angle)
            vz = velocity_magnitude * math.cos(angle)

        # Create the body object with mass, position, velocity, and color
        body = {
            "id": i+start_id,
            "name": f"Body_{i}",
            "mass": random.uniform(*mass_range),
            "radius": random.uniform(*radius_range),
            "position": [x, y, z],
            "velocity": [vx, vy, vz],
            "force": [0.0, 0.0, 0.0],  # Initial force set to zero
            "color": [
                random.uniform(0, 1),  # Red
                random.uniform(0, 1),  # Green
                random.uniform(0, 1),  # Blue
                random.uniform(0.5, 1) # Alpha (using 0.5 - 1 for better visibility)
            ]
        }
        bodies.append(body)

    return bodies
    
def save_bodies_to_json(bodies, filename):
    data = {"bodies": bodies}
    with open(filename, 'w') as f:
        json.dump(data, f, indent=4)

if __name__ == "__main__":

    # Setup argparse to handle command-line arguments
    parser = argparse.ArgumentParser(description="Generate random bodies in space or arrange them in elliptical orbits")
    
    # Add arguments for number of bodies and ranges
    parser.add_argument("--num_bodies", type=int, default=5, help="Number of bodies to generate")
    parser.add_argument("--mass_range", type=float, nargs=2, default=[1e20, 1e30], help="Range for body mass (min max)")
    parser.add_argument("--radius_range", type=float, nargs=2, default=[1e3, 1e6], help="Range for body radius (min max)")
    parser.add_argument("--position_range", type=float, nargs=2, default=[-1e9, 1e9], help="Range for body position (min max)")
    parser.add_argument("--velocity_range", type=float, nargs=2, default=[-1e4, 1e4], help="Range for body velocity (min max)")
    parser.add_argument("--file", type=str, default="random_bodies.json", help="Output JSON file")
    parser.add_argument("--path", type=str, default="../", help="Output directory for JSON file")
    parser.add_argument("--start_id", type=int, default=0, help="Starting id index")
    
    # Add arguments for arrangement
    parser.add_argument("--arrangement", type=str, choices=['random', 'elliptical'], default='random', help="Arrangement of bodies: 'random' or 'elliptical'")
    parser.add_argument("--circle_radius", type=float, default=None, help="Radius of the orbit for bodies in elliptical arrangement")
    parser.add_argument("--circle_center", type=float, nargs=3, default=[0.0, 0.0, 0.0], help="Center of the orbit as three floats (x y z)")
    parser.add_argument("--circle_plane", type=str, choices=['xy', 'xz', 'yz'], default='xy', help="Plane of the orbit: 'xy', 'xz', or 'yz' (only relevant if arrangement is 'elliptical')")
    
    # Additional arguments for elliptical orbits
    parser.add_argument("--min_radius", type=float, default=1e9, help="Minimum semi-major axis for elliptical orbits")
    parser.add_argument("--max_radius", type=float, default=1e10, help="Maximum semi-major axis for elliptical orbits")
    parser.add_argument("--eccentricity_range", type=float, nargs=2, default=[0.1, 0.5], help="Range of eccentricities for elliptical orbits (min max)")
    parser.add_argument("--central_mass", type=float, default=1e30, help="Mass of the central body for elliptical orbits")

    # Parse arguments
    args = parser.parse_args()

    # Validate arguments for elliptical arrangement
    if args.arrangement == 'elliptical' and args.circle_radius is None:
        parser.error("--circle_radius is required when --arrangement is 'elliptical'")

    # Generate bodies based on the arrangement
    if args.arrangement == 'random':
        bodies = generate_random_bodies(
            args.num_bodies,
            args.mass_range,
            args.radius_range,
            args.position_range,
            args.velocity_range
        )
    elif args.arrangement == 'elliptical':
        bodies = generate_elliptical_bodies(
            args.num_bodies,
            args.mass_range,
            args.radius_range,
            args.min_radius,
            args.max_radius,
            args.circle_center,
            args.circle_plane,
            args.central_mass,
            args.eccentricity_range,
            args.start_id
        )
    else:
        # This should not happen due to argparse choices
        raise ValueError("Invalid arrangement option.")

    # Ensure the output path ends without a trailing slash
    output_path = args.path.rstrip('/')

    # Save the bodies to the specified output file
    save_bodies_to_json(bodies, f"{output_path}/{args.file}")

    print(f"Generated {args.num_bodies} bodies with arrangement '{args.arrangement}' and saved to {output_path}/{args.file}")
