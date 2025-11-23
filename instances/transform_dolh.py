import os
import json

indir = "td-olhmann-thomas"
outdir = "d_ohlmann_thomas_2007"

def transform():

    if not os.path.exists(indir):
        print(f"Input folder {indir} does not exist!; try downloading it from the repository by Lera Romero et al.")
        return

    # Create a temporary directory to store the ZIP file
    if not os.path.exists(outdir):
        os.makedirs(outdir)


    for filename in os.listdir(indir):
        if filename.endswith('.json'):
            if filename.startswith('index') or filename.startswith('solution'):
                continue

            input_file_path = os.path.join(indir, filename)
            # Construct output file name by appending '_processed' before the extension
            output_file_path = os.path.join(outdir, f"d_{filename}")

            if os.path.exists(output_file_path):
                print(f"File {output_file_path} already exists; ignoring it")
                continue

            # Open and load the original JSON file
            with open(input_file_path, 'r') as f:
                data = json.load(f)

            new_data = {}
            new_data['start_depot'] = data['start_depot']
            new_data['end_depot'] = data['end_depot']
            new_data['vertex_count'] = data['digraph']['vertex_count']

            new_data['time_windows'] = [[int(10*w) for w in tw] for tw in data['time_windows']]
            new_data['travel_times'] = [[int(10*d) for d in ds] for ds in data['distances']]


            # Write the extracted data into the new output JSON file
            with open(output_file_path, 'w') as f:
                json.dump(new_data, f)
            print(f"Processed file saved as {output_file_path}")


if __name__ == "__main__":
    transform()
