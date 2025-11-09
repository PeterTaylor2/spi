# Any entry in the computers directory which matches the COMPUTERNAME environment variable will be included
# in the configuration for the build.
# 
# The idea is to define which versions of python and visual studio are installed on this computer.
# 
# You can get the version numbers for visual studio by running the python script spi/makefiles/python/vsVersions.py.
# 
# Here are some sample entries that can be placed in the computer specific file.
# 
# Note that if all computers are set-up in the same manner in the system then it is better to put these entries
# in the spi/makefiles/config/site.mk file instead.

G_WIN32_PYTHON39=$(LOCALAPPDATA)/Programs/Python/Python39-32/python.exe
G_WIN64_PYTHON39=$(LOCALAPPDATA)/Programs/Python/Python39/python.exe
G_WIN64_PYTHON312=$(LOCALAPPDATA)/Programs/Python/Python312/python.exe
G_WIN64_PYTHON313=$(LOCALAPPDATA)/Programs/Python/Python313/python.exe
G_WIN64_PYTHON314=$(LOCALAPPDATA)/Programs/Python/Python314/python.exe
G_WIN32_PY_VERSIONS?=39
G_WIN32_PY_VERSION?=39
G_WIN32_XL_VERSIONS?=12
G_WIN64_XL_VERSIONS=15
G_WIN64_PY_VERSIONS?=39 312 313
G_WIN64_PY_VERSION?=39

# version numbers for versions of Microsoft Visual Studio
# these are for when we are over-riding what is in msvc_site.mk

G_VS17_PACKAGE_TYPE=Community
G_VS17_KITS_VERSION=10.0.26100.0
G_VS17_TOOLS_VERSION=14.44.35207

# you need pdflatex for creating some of the PDF files from LaTeX files
# again this is computer/site specific

G_PDFLATEX=c:/apps/MiKTeX2.9/miktex/bin/pdflatex

