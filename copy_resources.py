import pathlib
import os
import sys
import shutil
import posixpath
import filecmp
import subprocess


def copy_file(input_path, output_path):
	shutil.copyfile(input_path, output_path)


def compile_shader(input_path, output_path):
	result = subprocess.run([glslc_path, "-o", output_path, "--target-env=vulkan1.3", "-O", input_path])
	if result.returncode != 0:
		sys.exit(1)


def process_file(input_path: str):
	ext = posixpath.splitext(input_path)[1]
	relative_path = posixpath.relpath(input_path, source_dir)
	output_path = posixpath.join(build_dir, relative_path)
	
	if posixpath.exists(output_path) and filecmp.cmp(input_path, output_path, shallow=True):
		return
	
	os.makedirs(posixpath.dirname(output_path), exist_ok=True)
	
	if ext == ".vert":
		compile_shader(input_path, output_path)
	elif ext == ".frag":
		compile_shader(input_path, output_path)
	elif ext == ".geom":
		compile_shader(input_path, output_path)
	elif ext == ".comp":
		compile_shader(input_path, output_path)
	else:
		copy_file(input_path, output_path)


if len(sys.argv) != 4:
	sys.exit(1)

source_dir = sys.argv[1]
build_dir = sys.argv[2]
glslc_path = sys.argv[3]

for path, subdirs, files in os.walk(source_dir):
	for name in files:
		process_file(pathlib.Path(path, name).as_posix())