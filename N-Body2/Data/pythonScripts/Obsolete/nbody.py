import json
import csv
import numpy as np
import argparse

# Constants
G = 6.67430e-11  # Gravitational constant, in m^3 kg^−1 s^−2

def compute_forces(positions, masses):
    n = len(masses)
    forces = np.zeros_like(positions)
    for i in range(n):
        for j in range(i + 1, n):
            # Calculate the vector between the bodies
            r = positions[j] - positions[i]
            # Calculate the distance
            dist = np.linalg.norm(r)
            # Calculate the force magnitude
            if dist > 1e-9:  # Avoid division by zero
                force_mag = G * masses[i] * masses[j] / dist**2
                # Normalize the distance vector and calculate forces
                force_dir = r / dist
                forces[i] += force_mag * force_dir
                forces[j] -= force_mag * force_dir  # Equal and opposite forces
    return forces

def update_positions_and_velocities(positions, velocities, forces, masses, dt):
    # Update velocities and positions using Newton's second law
    accelerations = forces / masses[:, np.newaxis]
    velocities += accelerations * dt
    positions += velocities * dt
    return positions, velocities

def run_simulation(input_file, output_file, num_steps, dt):
    # Load JSON data
    with open(input_file, "r") as f:
        data = json.load(f)

    bodies = data['bodies']

    # Initialize arrays for positions, velocities, and forces
    positions = np.array([body['position'] for body in bodies])
    velocities = np.array([body['velocity'] for body in bodies])
    masses = np.array([body['mass'] for body in bodies])

    # Open a CSV file to store the results
    with open(output_file, "w", newline="") as csvfile:
        csvwriter = csv.writer(csvfile)
        
        # Write the header row (timestep, Earth x, Earth y, Earth z, Moon x, Moon y, Moon z)
        header = ['timestep']
        for body in bodies:
            header.extend([f"{body['name']} x", f"{body['name']} y", f"{body['name']} z"])
        csvwriter.writerow(header)
        
        # Simulation loop
        for step in range(num_steps):
            # Compute forces
            forces = compute_forces(positions, masses)
            
            # Update positions and velocities
            positions, velocities = update_positions_and_velocities(positions, velocities, forces, masses, dt)
            
            # Write the current state to the CSV file
            row = [step * dt] + positions.flatten().tolist()
            csvwriter.writerow(row)

    print(f"Simulation complete. Results saved to {output_file}.")

if __name__ == "__main__":
    # Parse command-line arguments
    parser = argparse.ArgumentParser(description='Run a brute force N-body simulation.')
    parser.add_argument('--input', type=str, required=True, help='Path to the input JSON file containing body data.')
    parser.add_argument('--output', type=str, required=True, help='Path to the output CSV file where results will be saved.')
    parser.add_argument('--timesteps', type=int, required=True, help='Number of timesteps to simulate.')
    parser.add_argument('--dt', type=float, required=True, help='Time step duration in seconds.')

    args = parser.parse_args()

    # Run the simulation with the provided arguments
    run_simulation(args.input, args.output, args.timesteps, args.dt)

