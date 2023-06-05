# Pump.py

### Dependencies
It is recommended to use virtual environments; see https://realpython.com/python-virtual-environments-a-primer/
The following dependencies must be installed via:
`pip install click requests pandas openpyxl`

### Execution
The `DPM_GID` environment variable must be set. To ge the latest information on usage,
please execute `python3 python-cli/pump.py --help`. 

### Tests
Assuming `DPM_GID` and `DPD_URL` environment variables are set, to run tests, execute: 
`python3 python-cli/test_pump.py`