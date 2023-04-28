# Reader for HSDS

The Reader receives HSDS data via the configured topics.
The exact behavior of the Reader is still being defined.
At this point, the basic desired capabilities are:
* logging (preliminary)
* invocation of a web hook (preliminary)
* HTTP API (no support at this time)

For the web hook, the expected API is that of the Writer.
The Reader tracks the state of the web hook server using the `Transaction` header and synchronizes appropriately.
Presumably, HTTP API of the Reader will be a read-only version of the API exposed by the Writer.

## Invocation

The Reader is configured through environment variables.
It may be useful to create a wrapper script that defines these variables.

* `DPM_APPLICATION_ID` - the id of the application in the DPM
* `DPM_PASSWORD` - the password of the application in the DPM
* `DPM_NONCE` - an alphanumeric string that distinguishes this instance of JsonWriter
* `SERVER_URL` - (optional) the URL of the server for web hook invocation

Copyright 2023 CommunityUtils Authors
