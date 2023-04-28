# Writer for HSDS

The Writer receives HSDS data via an HTTP API and writes the received data to the configured topics.

## Invocation

The Writer is configured through environment variables.
It may be useful to create a wrapper script that defines these variables.

* `DPM_APPLICATION_ID` - the id of the application in the DPM
* `DPM_PASSWORD` - the password of the application in the DPM
* `DPM_NONCE` - an alphanumeric string that distinguishes this instance of JsonWriter
* `HTTP_PORT` - the listening port of the HTTP API (default 8080)
* `CREATE_WRITERS` - 0 or 1 indicating that writers should be created and used (default 1)

Using `CREATE_WRITERS=0` is useful for standing up an API to test the web hook capabilities of the Reader.

## Usage

The HTTP API contains an endpoint for each HSDS table.
The names of the endpoints are defined in the `*_ENDPOINT` string constants defined in `CommunityUtils/HSDS/Common/HSDS3.idl`.
Thus, the endpoint for organizations is `/hsds3/organization`.
Each endpoint is identical in the operations that it supports.
These operations will be presented using organizations.

### GET `/hsds3/organization`

Return 200 and a JSON array of organizations on success.

Parameters:
* `offset` - specifies the first organization to retrieve (default 0).
* `count` - specifies the number of organizations to retrieve (default returns all organizations).

Response Headers:
* `Offset` - The offset in the request.
* `Count` - The actual count of the response.
* `Total` - The total number of organizations.
* `Transaction` - The number of modifying operations completed by the API.

### PUT `/hsds3/organization`

Replace the list of organizations with the request body which should be a JSON array of organizations.
Returns 204 No Content on success.

Response Headers:
* `Total` - The total number of organizations.
* `Transaction` - The number of modifying operations completed by the API.

### POST `/hsds3/organization`

Insert the list of organizations with the request body which should be a JSON array of organizations.
Returns 204 No Content on success.

Response Headers:
* `Total` - The total number of organizations.
* `Transaction` - The number of modifying operations completed by the API.

### DELETE `/hsds3/organization`

Delete the set of organizations.
Returns 204 No Content on success.

Response Headers:
* `Total` - The total number of organizations (0 after success).
* `Transaction` - The number of modifying operations completed by the API.

### GET `/hsds3/organization/{dpmgid}/{id}`

Return 200 and a JSON object for the organization on success.

Parameters:
* `{dpmgid}` - The DPM group id of the organization
* `{id}` - the id of the organization

Response Headers:
* `Transaction` - The number of modifying operations completed by the API.

### PUT `/hsds3/organization/{dpmgid}/{id}`

Replace the organization with the request body which should be a JSON object for an organization.
Return 200 and a JSON object for the organization on success.

Parameters:
* `{dpmgid}` - The DPM group id of the organization
* `{id}` - the id of the organization

Response Headers:
* `Transaction` - The number of modifying operations completed by the API.

### DELETE `/hsds3/organization/{dpmgid}/{id}`

Delete the organization.
Returns 204 No Content on success.

Parameters:
* `{dpmgid}` - The DPM group id of the organization
* `{id}` - the id of the organization

Response Headers:
* `Transaction` - The number of modifying operations completed by the API.

### The `Transaction` Response Header

The `Transaction` response header allows a client to determine if the Writer requires resynchronization.
If the Writer restarts, the `Transaction` header will reset to 0.
A client can use this behavior to reload data.

Copyright 2023 CommunityUtils Authors
