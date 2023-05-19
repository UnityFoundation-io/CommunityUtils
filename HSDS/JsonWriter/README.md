# JsonWriter for HSDS

The JsonWriter reads an HSDS dataset stored as JSON files and publishes the data on the configured topics.

## Invocation

The JsonWriter is configured through environment variables.
It may be useful to create a wrapper script that defines these variables.

* `DPM_APPLICATION_ID` - the id of the application in the DPM
* `DPM_PASSWORD` - the password of the application in the DPM
* `DPM_NONCE` - an alphanumeric string that distinguishes this instance of JsonWriter
* `DATA_PATH` - the path to a directory containing JSON files

The DPM application must have write access to the topics.

The data read by the JsonWriter is found by concatenating the value of the `DATA_PATH` variable with the various `*_JSON_FILE` string constants defined in `CommunityUtils/HSDS/Common/HSDS3.idl`.
Thus, if `DATA_PATH` contains, `/my/hsds/data`, then the JsonWriter will attempt to load organizations from `/my/hsds/data/organization.json`.
Each JSON file should contain an array of objects where each object represents a value of the appropriate type.

Copyright 2023 CommunityUtils Authors
