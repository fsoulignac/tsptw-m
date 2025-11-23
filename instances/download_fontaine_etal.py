import os
import zipfile
import requests
from io import BytesIO

output_dir = "fontaine_etal_code"

def download_fontaine_repo():
    if os.path.exists(output_dir):
        print("Output folder already exists; not downloading")
        return

    response = requests.get('https://github.com/romainfontaine/tdtsptw-ejor23/archive/refs/heads/main.zip', stream=True)
    if response.status_code != 200:
        print(f"Failed to download repository: {response.status_code}")
        return

    # Send a GET request to fetch the ZIP file
    print('Downloading the repository from github')

    # Create a temporary directory to store the ZIP file
    os.makedirs(output_dir)

    # Prepare the destination file path
    zip_path = os.path.join(output_dir, "repo.zip")

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
        zip_ref.extractall(output_dir)
    print(f"Repository extracted to {output_dir}")


# Example usage
download_fontaine_repo()
