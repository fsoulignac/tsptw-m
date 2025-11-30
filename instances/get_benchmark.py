import os
import zipfile
import tarfile
import requests
import json
import subprocess
import shutil
from io import BytesIO

fontaine_code = 'https://github.com/romainfontaine/tdtsptw-ejor23/archive/refs/heads/main.zip'
fontaine_dir = 'fontaine_etal_code'
fontaine_benchmark_dir = os.path.join(fontaine_dir, "tdtsptw-ejor23-main/benchmarks/")
fontaine_benchmark_tmp = "benchmarks"


lera_code = 'https://github.com/gleraromero/tdtsptw/archive/refs/heads/master.zip'
lera_dir = 'lera_etal_code'
lera_benchmark_dir = os.path.join(lera_dir, 'tdtsptw-master/instances/')

def download_repo(url, to):

    # Create a directory to store the ZIP file
    if not os.path.exists(to):
        print(f"Creating folder {to}")
        os.makedirs(to)

    # Prepare the destination file path
    zip_path = os.path.join(to, "repo.zip")

    if os.path.exists(zip_path):
        print(f"Zip file {zip_path} already exists; not downloading it")
    else:
        response = requests.get(url, stream=True)
        if response.status_code != 200:
            raise ValueError(f"Failed to download repository: {response.status_code}")

        # Send a GET request to fetch the ZIP file
        print(f'Downloading the repository {url}')


        # Set the chunk size and initialize counters
        chunk_size = 1024  # 1 KB per chunk
        downloaded_size = 0

        # Write the content to a file and display download progress
        with open(zip_path, 'wb') as f:
            for chunk in response.iter_content(chunk_size=chunk_size):
                if chunk:  # Filter out keep-alive new chunks
                    f.write(chunk)
                    downloaded_size += len(chunk)

                    # Manually update the progress bar based on the downloaded size
                    progress = (downloaded_size // chunk_size)  # Total number of chunks downloaded
                    progress_bar = "#" * (progress % 50)  # Progress bar of 50 "#" max
                    print(f"\rDownloading: [{progress_bar:<50}] {progress} chunks", end="", flush=True)

        print("\nDownload complete!")

    # Extract the downloaded ZIP file
    with zipfile.ZipFile(zip_path, 'r') as zip_ref:
        zip_ref.extractall(to)
    print(f"Repository extracted to {to}")


def extract_fontaine_benchmark(to, benchmark_name):
    if not os.path.exists(fontaine_benchmark_dir):
        raise ValueError(f"Input folder {fontaine_benchmark_dir} does not exist; try downloading it first")


    # Create a temporary directory to store the ZIP file
    if not os.path.exists(to):
        print(f"Creating folder {to} to unzip the benchmarks in {fontaine_benchmark_dir}")
        os.makedirs(to)

    # Prepare the destination file path
    benchmark_path = os.path.join(fontaine_benchmark_dir, benchmark_name)

    # Write the content to a file and display download progress
    with tarfile.open(benchmark_path, 'r:gz') as tar:
        print(f"Extracting {benchmark_path} to {to}...")
        # Extract all contents to the specified directory
        tar.extractall(path=to)
        print("Extraction complete!")


# Traverse the directory
def transform_fontaine_benchmark(input_dir, in_path, extension, out_path):

    # Create a temporary directory to store the ZIP file
    if not os.path.exists(out_path):
        os.makedirs(out_path)

    in_path = os.path.join(input_dir, in_path)

    if not os.path.exists(in_path):
        raise ValueError(f"Input folder {in_path} does not exist!; try downloading and extracting the benchmarks")

    for tfile in os.listdir(in_path):
        if os.path.isfile(os.path.join(in_path, tfile)) and (extension is None or os.path.splitext(tfile)[1] == extension):
            name = tfile.replace(extension, ".json") if extension is not None else tfile
            ofile = os.path.join(out_path, name + ".json")
            if os.path.exists(ofile):
                print(f"File {ofile} already exists; ignoring it")
                continue

            lines = open(os.path.join(in_path, tfile)).read().strip().split('\n')
            n = int(lines[0])
            times_aux = [list(map(int, l.strip().split(' '))) for l in lines[1:n+1]]
            times = [times_aux[i] + [times_aux[i][0]] for i in range(n)]
            times.append([0] * (n+1))
            json_times = [[j for j in ti] for ti in times]

            windows = [list(map(int, [i for i in l.strip().split(' ') if i != ""])) for l in lines[n+1:] if len(l)>0 and l[0] != '#']
            windows.append(windows[0])

            print("Processing ", tfile, "to", ofile)

            data = {
                "travel_times": json_times,
                "time_windows": windows,
                "start_depot": 0,
                "end_depot": len(windows)-1,
                "vertex_count": len(windows),
            }
            with open(ofile, 'w') as json_file:
                json.dump(data, json_file)


def transform_lera_benchmark(lera_dir, in_dir, out_dir):

    if not os.path.exists(lera_dir):
        raise ValueError(f"Input folder {in_dir} does not exist!; try downloading it from the repository by Lera Romero et al.")

    # Create a temporary directory to store the ZIP file
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    in_dir = os.path.join(lera_dir, in_dir)

    for filename in os.listdir(in_dir):
        if filename.endswith('.json'):
            if filename.startswith('index') or filename.startswith('solution'):
                continue

            input_file_path = os.path.join(in_dir, filename)
            # Construct output file name by appending '_processed' before the extension
            output_file_path = os.path.join(out_dir, f"d_{filename}")

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

    download_repo(fontaine_code, fontaine_dir)

    extract_fontaine_benchmark(fontaine_benchmark_tmp, "constant.tar.gz")
    extract_fontaine_benchmark(fontaine_benchmark_tmp, "rif20-const.tar.gz")

    transform_fontaine_benchmark(fontaine_benchmark_tmp, "asc96", ".tw", "ascheuer_1996")
    transform_fontaine_benchmark(fontaine_benchmark_tmp, "das10", ".txt", "dasilva_urrutia_2010")
    transform_fontaine_benchmark(fontaine_benchmark_tmp, "dum95", ".txt", "dumas_et_al_1995")
    transform_fontaine_benchmark(fontaine_benchmark_tmp, "gen98", ".txt", "gendreau_et_al_1998")
    transform_fontaine_benchmark(fontaine_benchmark_tmp, "lan93", ".dat", "langevin_et_al_1993")
    transform_fontaine_benchmark(fontaine_benchmark_tmp, "ohl07", ".txt", "ohlmann_thomas_2007")
    transform_fontaine_benchmark(fontaine_benchmark_tmp, "pes98", None, "pesant_et_al_1998")
    transform_fontaine_benchmark(fontaine_benchmark_tmp, "pot96", ".txt", "potvin_bengio_1996")
    transform_fontaine_benchmark(fontaine_benchmark_tmp, "rif20-const/21", None, "rifki_et_al_2020")
    transform_fontaine_benchmark(fontaine_benchmark_tmp, "rif20-const/31", None, "rifki_et_al_2020")
    transform_fontaine_benchmark(fontaine_benchmark_tmp, "rif20-const/41", None, "rifki_et_al_2020")

    download_repo(lera_code, lera_dir)
    transform_lera_benchmark(lera_benchmark_dir, "td-olhmann-thomas", "d_ohlmann_thomas_2007")

    print("Creating indices")
    subprocess.run(["bash", "create_indices.sh"])

    print(f"Removing directories {fontaine_dir}, {fontaine_benchmark_tmp}, and {lera_dir}")
    shutil.rmtree(fontaine_dir)
    shutil.rmtree(fontaine_benchmark_tmp)
    shutil.rmtree(lera_dir)
