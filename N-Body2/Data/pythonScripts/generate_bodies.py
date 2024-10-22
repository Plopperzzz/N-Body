import json
import random
import argparse
import math
import sys

G = 6.67430e-11  # Gravitational constant

def generate_random_bodies(num_bodies, mass_range, radius_range, position_range, velocity_range, start_id=0):
    bodies = []
    
    for i in range(num_bodies):
        body = {
            "id": i + start_id,
            "name": f"Body_{i + start_id}",
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

def generate_elliptical_bodies(galaxy_config, start_id=0):
    num_bodies = galaxy_config["num_bodies"]
    mass_range = galaxy_config["mass_range"]
    radius_range = galaxy_config["radius_range"]
    min_radius = galaxy_config["min_radius"]
    max_radius = galaxy_config["max_radius"]
    circle_center = galaxy_config["circle_center"]
    circle_plane = galaxy_config["circle_plane"]
    central_mass = galaxy_config["central_mass"]
    eccentricity_range = galaxy_config["eccentricity_range"]
    central_body_id = start_id + num_bodies  # Assign an ID for the central mass

    bodies = []

    # Add central mass (e.g., black hole or massive star at the center of the galaxy)
    central_body = {
        "id": central_body_id,
        "name": f"Central_Mass_{central_body_id}",
        "mass": central_mass,
        "radius": galaxy_config.get("central_radius", 1e6),
        "position": circle_center,
        "velocity": galaxy_config.get("central_velocity", [0.0, 0.0, 0.0]),
        "force": [0.0, 0.0, 0.0],
        "color": galaxy_config.get("central_color", [1.0, 1.0, 0.0, 1.0])  # Default to yellow
    }
    bodies.append(central_body)

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

        # Adjust velocities if the central mass has velocity
        vx += central_body["velocity"][0]
        vy += central_body["velocity"][1]
        vz += central_body["velocity"][2]

        # Create the body object with mass, position, velocity, and color
        body = {
            "id": i + start_id,
            "name": f"Body_{i + start_id}",
            "mass": random.uniform(*mass_range),
            "radius": random.uniform(*radius_range),
            "position": [x, y, z],
            "velocity": [vx, vy, vz],
            "force": [0.0, 0.0, 0.0],  # Initial force set to zero
            "color": [
                random.uniform(0, 1),  # Red
                random.uniform(0, 1),  # Green
                random.uniform(0, 1),  # Blue
                random.uniform(0.5, 1)  # Alpha (using 0.5 - 1 for better visibility)
            ]
        }
        bodies.append(body)

    return bodies

def generate_galaxies(galaxy_configs):
    bodies = []
    start_id = 0
    for idx, galaxy_config in enumerate(galaxy_configs):
        print(f"Generating galaxy {idx+1} with {galaxy_config['num_bodies']} bodies.")
        galaxy_bodies = generate_elliptical_bodies(galaxy_config, start_id=start_id)
        bodies.extend(galaxy_bodies)
        start_id = bodies[-1]["id"] + 1  # Update start_id for the next galaxy
    return bodies

def save_bodies_to_json(bodies, filename):
    data = {"bodies": bodies}
    with open(filename, 'w') as f:
        json.dump(data, f, indent=4)

if __name__ == "__main__":

    # Setup argparse to handle command-line arguments
    parser = argparse.ArgumentParser(description="Generate random bodies in space or arrange them in elliptical orbits, including multiple galaxies.")
    
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
    parser.add_argument("--arrangement", type=str, choices=['random', 'elliptical', 'galaxies'], default='random', help="Arrangement of bodies: 'random', 'elliptical', or 'galaxies'")
    parser.add_argument("--circle_radius", type=float, default=None, help="Radius of the orbit for bodies in elliptical arrangement")
    parser.add_argument("--circle_center", type=float, nargs=3, default=[0.0, 0.0, 0.0], help="Center of the orbit as three floats (x y z)")
    parser.add_argument("--circle_plane", type=str, choices=['xy', 'xz', 'yz'], default='xy', help="Plane of the orbit: 'xy', 'xz', or 'yz' (only relevant if arrangement is 'elliptical')")
    
    # Additional arguments for elliptical orbits
    parser.add_argument("--min_radius", type=float, default=1e9, help="Minimum semi-major axis for elliptical orbits")
    parser.add_argument("--max_radius", type=float, default=1e10, help="Maximum semi-major axis for elliptical orbits")
    parser.add_argument("--eccentricity_range", type=float, nargs=2, default=[0.1, 0.5], help="Range of eccentricities for elliptical orbits (min max)")
    parser.add_argument("--central_mass", type=float, default=1e30, help="Mass of the central body for elliptical orbits")
    parser.add_argument("--galaxy_configs_file", type=str, default=None, help="JSON file containing configurations for multiple galaxies")
    
    # Parse arguments
    args = parser.parse_args()

    # Ensure the output path ends without a trailing slash
    output_path = args.path.rstrip('/')

    if args.arrangement == 'random':
        bodies = generate_random_bodies(
            args.num_bodies,
            args.mass_range,
            args.radius_range,
            args.position_range,
            args.velocity_range,
            args.start_id
        )
        arrangement_description = 'random'
    elif args.arrangement == 'elliptical':
        # Validate arguments for elliptical arrangement
        if args.circle_radius is None:
            parser.error("--circle_radius is required when --arrangement is 'elliptical'")

        galaxy_config = {
            "num_bodies": args.num_bodies,
            "mass_range": args.mass_range,
            "radius_range": args.radius_range,
            "min_radius": args.min_radius,
            "max_radius": args.max_radius,
            "circle_center": args.circle_center,
            "circle_plane": args.circle_plane,
            "central_mass": args.central_mass,
            "eccentricity_range": args.eccentricity_range
        }

        bodies = generate_elliptical_bodies(galaxy_config, start_id=args.start_id)
        arrangement_description = 'elliptical'
    elif args.arrangement == 'galaxies':
        if args.galaxy_configs_file is None:
            print("Error: --galaxy_configs_file is required when --arrangement is 'galaxies'")
            sys.exit(1)

        # Load galaxy configurations from JSON file
        with open(args.galaxy_configs_file, 'r') as f:
            galaxy_configs = json.load(f)

        # Validate galaxy_configs
        if not isinstance(galaxy_configs, list):
            print("Error: Galaxy configurations file must contain a list of galaxy configurations.")
            sys.exit(1)

        bodies = generate_galaxies(galaxy_configs)
        arrangement_description = f"{len(galaxy_configs)} galaxies"
    else:
        # This should not happen due to argparse choices
        raise ValueError("Invalid arrangement option.")

    # Save the bodies to the specified output file
    save_bodies_to_json(bodies, f"{output_path}/{args.file}")

    print(f"Generated {len(bodies)} bodies with arrangement '{arrangement_description}' and saved to {output_path}/{args.file}")