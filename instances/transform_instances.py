import json
import os

input_dir = "benchmarks"

# Traverse the directory
def transform_dir(in_path, extension, out_path):

    # Create a temporary directory to store the ZIP file
    if not os.path.exists(out_path):
        os.makedirs(out_path)

    in_path = os.path.join(input_dir, in_path)

    if not os.path.exists(in_path):
        print(f"Input folder {in_path} does not exist!; try downloading and extracting the benchmarks")
        return

    for tfile in os.listdir(in_path):
        if os.path.isfile(os.path.join(in_path, tfile)) and (extension is None or os.path.splitext(tfile)[1] == extension):
            ofile = os.path.join(out_path, tfile.replace(extension, ".json") if extension is not None else tfile + ".json")
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


transform_dir("asc96", ".tw", "ascheuer_1996")
transform_dir("das10", ".txt", "dasilva_urrutia_2010")
transform_dir("dum95", ".txt", "dumas_et_al_1995")
transform_dir("gen98", ".txt", "gendreau_et_al_1998")
transform_dir("lan93", ".dat", "langevin_et_al_1993")
transform_dir("ohl07", ".txt", "ohlmann_thomas_2007")
transform_dir("pes98", None, "pesant_et_al_1998")
transform_dir("pot96", ".txt", "potvin_bengio_1996")
transform_dir("rif20-const/21", None, "rifki_et_al_2020")
transform_dir("rif20-const/31", None, "rifki_et_al_2020")
transform_dir("rif20-const/41", None, "rifki_et_al_2020")


