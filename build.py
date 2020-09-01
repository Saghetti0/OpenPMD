import os
import hashlib
import json
import argparse
import subprocess

if not os.path.exists("obj"):
    os.makedirs("obj/")

if not os.path.exists("bin"):
    os.makedirs("bin/")
if not os.path.exists("bin/server"):
    os.makedirs("bin/server/")

with open("build_config.json","r") as fh:
    build_config = json.loads(fh.read())

parser = argparse.ArgumentParser(description="Compile and link the OpenPMD source code.")
parser.add_argument("target",type=str,help="the target to build")
parser.add_argument("--force-rebuild",dest="clean_rebuild",action="store_const",const=True,default=False,help="clear all cached files and hashes and rebuild the project. use after updating build_config")
parser.add_argument("--force-relink",dest="force_relink",action="store_const",const=True,default=False,help="force the linker to run even if no files are modified")
parser.add_argument("--run",dest="run",action="store_const",const=True,default=False,help="run application immediately after compiling")
args = parser.parse_args()

if not args.target in build_config["targets"]:
    print("Invalid target! Please choose one of: " + ", ".join(build_config["targets"].keys()))
    exit(1)

build_target_data = build_config["targets"][args.target]
build_dir = build_target_data["folder"]

if args.clean_rebuild:
    if os.path.exists("last_build_" + args.target + ".json"):
        os.remove("last_build_" + args.target + ".json")

print("Checking files for target " + args.target)

if not os.path.exists("last_build_" + args.target + ".json"):
    with open("last_build_" + args.target + ".json","w") as fh:
        fh.write(json.dumps({
            "file_hashes": {}
        }))

with open("last_build_" + args.target + ".json","r") as fh:
    last_build_config = json.loads(fh.read())

relink_needed = False
# check files that have been modified
recompile_needed = []
result = [(dp + "/" + f if not dp.endswith("/") else dp + f) for dp, dn, filenames in os.walk(build_dir) for f in filenames]
for file in list(result):
    if not file.endswith(".c"):
        result.remove(file)
for file in result:
    if not last_build_config["file_hashes"].get(file):
        recompile_needed.append(file)
        if not os.path.exists("obj/" + args.target + "/" + os.path.dirname(file)):
            os.makedirs("obj/" + args.target + "/"  + os.path.dirname(file))
        continue
    hashdata = last_build_config["file_hashes"][file]
    with open(file,"rb") as fh:
        if hashdata != hashlib.sha1(fh.read()).hexdigest():
            recompile_needed.append(file)
            continue

if not relink_needed:
    relink_needed = len(recompile_needed) > 0

for file in recompile_needed:
    print("File " + file + " modified since last build, recompiling")
    # recompile
    pre, ext = os.path.splitext(file)
    command_to_execute = build_target_data["commands"]["compiler"] + " -c "
    for include_dir in build_target_data["include_dirs"]:
        command_to_execute += " -I" + include_dir.replace("\\","\\\\") + " "
    command_to_execute += " ".join(build_target_data["compiler_flags"]) + " " + file +  " -o obj/" + args.target + "/"  + pre + ".o"
    print(command_to_execute)
    return_code = os.system(command_to_execute)
    if return_code != 0:
        print("Compiler returned code " + str(return_code) + ", exiting!")
        exit(return_code)
    # hash file
    with open(file,"rb") as fh:
        last_build_config["file_hashes"][file] = hashlib.sha1(fh.read()).hexdigest()

# check files that have been deleted since last compile
for file in list(last_build_config["file_hashes"].keys()):
    if not os.path.exists(file):
        print("File " + file + " deleted since last build")
        pre, ext = os.path.splitext(file)
        if os.path.exists("obj/" + args.target + "/" + pre + ".o"):
            os.remove("obj/" + args.target + "/" + pre + ".o")
        last_build_config["file_hashes"].pop(file,None)
        if not relink_needed:
            relink_needed = True

result = [(dp + "/" + f if not dp.endswith("/") else dp + f) for dp, dn, filenames in os.walk("obj/" + args.target + "/" + build_dir) for f in filenames]
for file in list(result):
    if not file.endswith(".o"):
        result.remove(file)

if not relink_needed:
    relink_needed = args.force_relink

if relink_needed:
    print("Linking...")
    command_to_execute = build_target_data["commands"]["linker"]
    command_to_execute += " " + " ".join(result)
    for link_dir in build_target_data["link_dirs"]:
        command_to_execute += " -L" + link_dir.replace("\\","\\\\") + " "
    command_to_execute += " " + " ".join(build_target_data["linker_flags"])
    command_to_execute += " -o " + build_target_data["output"]
    print(command_to_execute)
    return_code = os.system(command_to_execute)
    if return_code != 0:
        print("Compiler returned code " + str(return_code) + ", exiting!")
        exit(return_code)
else:
    print("Nothing to do (no files modified since last build)")

# write data
with open("last_build_" + args.target + ".json","w") as fh:
    fh.write(json.dumps(last_build_config))

if args.run:
    print("Running " + build_target_data["output"] + "...\n")
    return_code = subprocess.run(build_target_data["output"],cwd=build_target_data["working_dir"])
    print("\nProcess returned with code",return_code.returncode)
