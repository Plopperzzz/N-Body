import json
import random
import argparse

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

def save_bodies_to_json(bodies, filename):
    data = {"bodies": bodies}
    with open(filename, 'w') as f:
        json.dump(data, f, indent=4)

if __name__ == "__main__":

    # Setup argparse to handle command-line arguments
    parser = argparse.ArgumentParser(description="Generate random bodies in space")
    
    # Add arguments for number of bodies and ranges
    parser.add_argument("--num_bodies", type=int, default=5, help="Number of bodies to generate")
    parser.add_argument("--mass_range", type=float, nargs=2, default=[1e20, 1e30], help="Range for body mass (min max)")
    parser.add_argument("--radius_range", type=float, nargs=2, default=[1e3, 1e6], help="Range for body radius (min max)")
    parser.add_argument("--position_range", type=float, nargs=2, default=[-1e9, 1e9], help="Range for body position (min max)")
    parser.add_argument("--velocity_range", type=float, nargs=2, default=[-1e4, 1e4], help="Range for body velocity (min max)")
    parser.add_argument("--file", type=str, default="random_bodies.json", help="Output JSON file")
    parser.add_argument("--path", type=str, default="../", help="Output JSON file")

    # Parse arguments
    args = parser.parse_args()

    # Generate random bodies using the provided ranges
    bodies = generate_random_bodies(args.num_bodies, args.mass_range, args.radius_range, args.position_range, args.velocity_range)

    # Save the bodies to the specified output file
    save_bodies_to_json(bodies, args.path + "/" + args.file)

    print(f"Generated {args.num_bodies} bodies and saved to {args.path + "/" + args.file}")