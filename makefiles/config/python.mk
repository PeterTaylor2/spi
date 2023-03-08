#
# specific information for running python scripts
#

G_PY_MAJOR_VERSION=$(shell $(G_PYTHON) -c "import sys; print(sys.version_info.major)")

info::
	@echo "G_PYTHON=$(G_PYTHON)"
	@echo "G_PY_MAJOR_VERSION=$(G_PY_MAJOR_VERSION)"

