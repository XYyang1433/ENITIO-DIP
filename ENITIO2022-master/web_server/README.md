Requirements
---------------
- Python 3.x installation


Installation
-------------
Upon cloning this repository, the virtual environment as well as the required python packages are installed
by running the setup script for [Windows](requirements/setup_venv.bat). 
Do remember to edit the python version in the script file. 


Package Management
-------------------
This repository uses [pip-tools](https://github.com/jazzband/pip-tools) to synchronise python packages across computers. To add a new Python Package:

1. Insert the name of the python package in ``requirements.in``.
2. Then run `pip-compile` so that ``requirements.txt`` can be updated.
3. Finally, run `pip-sync` to download the new packages.
