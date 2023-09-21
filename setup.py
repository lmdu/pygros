import os
import glob
import zipfile
import urllib.request
from setuptools import setup, Extension

root_dir = os.path.dirname(os.path.abspath(__file__))

sources = glob.glob(os.path.join(root_dir, 'src', '*.c'))
include_dirs = []

description = "A python package for finding genomic range overlaps"

with open('README.rst') as fh:
	long_description = fh.read()

with open(os.path.join('src', 'version.h')) as fh:
	version = fh.read().split()[2].strip('"')

cgrs_ver = '0.1'
cgrs_dir = os.path.join(root_dir, "cgranges-{}".format(cgrs_ver))
cgrs_file = "{}.zip".format(cgrs_dir)
cgrs_url = "https://github.com/lh3/cgranges/archive/refs/tags/v{}.zip".format(cgrs_ver)

if not os.path.exists(cgrs_dir):
	if not os.path.isfile(cgrs_file):
		urllib.request.urlretrieve(cgrs_url, cgrs_file)

	with zipfile.ZipFile(cgrs_file) as _zip:
		_zip.extractall()

sources.append(os.path.join(cgrs_dir, 'cgranges.c'))
include_dirs.append(cgrs_dir)

extension = Extension('pygros', 
	sources = sources,
	include_dirs = include_dirs
)

setup(
	name = 'pygros',
	version = version,
	ext_modules = [extension],
	description = description,
	long_description = long_description,
	author = 'Lianming Du',
	author_email = 'adullb@qq.com',
	url = 'https://github.com/lmdu/pygros',
	license = 'MIT',
	keywords = 'bioinformatics genome interval range overlap intersection',
	classifiers = [
			"Development Status :: 5 - Production/Stable",
			"Intended Audience :: Developers",
			"Intended Audience :: Education",
			"Intended Audience :: Science/Research",
			"Natural Language :: English",
			"License :: OSI Approved :: MIT License",
			"Programming Language :: C",
			"Programming Language :: Python :: 3.6",
			"Programming Language :: Python :: 3.7",
			"Programming Language :: Python :: 3.8",
			"Programming Language :: Python :: 3.9",
			"Programming Language :: Python :: 3.10",
			"Programming Language :: Python :: 3.11",
			"Operating System :: Microsoft :: Windows",
			"Operating System :: POSIX :: Linux",
			"Operating System :: Unix",
			"Topic :: Scientific/Engineering :: Bio-Informatics"
	]
)