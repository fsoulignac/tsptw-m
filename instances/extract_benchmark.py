import os
import tarfile
from io import BytesIO

input_dir = "fontaine_etal_code/tdtsptw-ejor23-main/benchmarks/"
output_dir = "benchmarks"

def extract_benchmark(name):
    if not os.path.exists(input_dir):
        print("Input folder does not exist; try downloading it first")
        return

    # Create a temporary directory to store the ZIP file
    os.makedirs(output_dir)

    # Prepare the destination file path
    const_path = os.path.join(input_dir, name)

    # Write the content to a file and display download progress
    with tarfile.open(const_path, 'r:gz') as tar:
        print(f"Extracting {const_path} to {output_dir}...")
        # Extract all contents to the specified directory
        tar.extractall(path=output_dir)
        print("Extraction complete!")


if __name__ == "__main__":
    if os.path.exists(output_dir):
        print("Output folder already exists; ignoring")
        exit(0)

    extract_benchmark("constant.tar.gz")
    extract_benchmark("rif20-const.tar.gz")
