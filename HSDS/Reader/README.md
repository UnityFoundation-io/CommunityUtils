# Reader for HSDS

The Reader receives HSDS data via the configured topics.
The exact behavior of the Reader is still being defined.
Currently, the reader has preliminary support for:
* logging
* invocation of a web hook
* HTTP API

For the web hook, the expected API is that of the Writer.
The Reader tracks the state of the web hook server using the `Transaction` header and synchronizes appropriately.

The HTTP API of the Reader is a read-only version of the API exposed by the [Writer](../Writer/README.md).

## Invocation

The Reader is configured through environment variables.
It may be useful to create a wrapper script that defines these variables.

* `DPM_APPLICATION_ID` - the id of the application in the DPM
* `DPM_PASSWORD` - the password of the application in the DPM
* `DPM_NONCE` - an alphanumeric string that distinguishes this instance of JsonWriter
* `HTTP_PORT` - the listening port of the HTTP API (default 8080)
* `ENABLE_HTTP_LOG_ACCESS` - when 1, log HTTP API activity, default 1
* `SERVER_URL` - (optional) the URL of the server for web hook invocation

Copyright 2023 CommunityUtils Authors
