import subprocess
import os

def run_synthesiser(range_limit, layer_start_index, n_outputs):
    # Define the path to the directory with the hex LUT values
    results_dir = os.path.join(os.path.dirname(__file__), 'verilog')
    neuron_dir = os.path.join(os.path.dirname(__file__), 'neuron_logs')

    for x in range(range_limit):
        executable_base_path = './RandomInsertion.exe'  
        truth_table_file = os.path.join(results_dir, f'layer{layer_start_index}_N{x}_hex.txt')
        neuron_input_file = os.path.join(neuron_dir, f'layer{layer_start_index}_N{x}_input.txt')
        rarity_threshold = '0'  # Set the desired rarity threshold
        output_path = "../NeuraLUT_Private/datasets/jet_substructure/test_random_725_r1/verilog"  # Assuming output path is the same as results_dir
        output_name = f'layer{layer_start_index}_N{x}'

        try:
            # Run the executable and capture the output
            result = subprocess.run(
                [
                    executable_base_path,
                    '-table', truth_table_file,
                    '-input', neuron_input_file,
                    '-rarity', rarity_threshold,
                    '-output', output_path,
                    '-name', output_name,
                    '-n_out', str(n_outputs)  # Pass the number of output bits
                ],
                check=True,
                universal_newlines=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )
            
            # Executable output
            print("Output from C++ executable:")
            print(result.stdout)

        except subprocess.CalledProcessError as e:
            print(f"Error running the executable: {e}")
            print("Standard output:")
            print(e.stdout)
            print("Standard error:")
            print(e.stderr)

        except FileNotFoundError:
            print("Executable not found. Please check the path.")

# Function calls
run_synthesiser(32, 0, 4)  # Example: layer 0, 32 neurons, 4 output bits
run_synthesiser(5, 1, 4)