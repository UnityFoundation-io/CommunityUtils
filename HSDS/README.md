# CommUNITY Support for HSDS

This directory contains utilities for interacting with HSDS version 3 topics in CommUNITY.
See https://docs.openreferral.org/en/latest/hsds/about.html for a background on HSDS.
The HSDS schema is captured in `Common/HSDS3.idl`.

* JsonWriter - writes HSDS data stored as JSON files
* Writer - writes HSDS data received via an HTTP API
* Reader - logs HSDS data and optionally invokes a web hook.

## General Principles

The applications assume a deployment of CommUNITY which includes a DDS Permissions Manager (DPM) and RtpsRelay.
The RtpsRelay is optional, however, its use is encouraged because it reduces connectivity challenges.

The DPM should contain topics for the various HSDS tables.
The DPM should also contain one or more applications for use with the utilities.
The DPM applications should be created appropriate access to the topics.
Specifically, a JsonWriter or Writer must be able to write to all of the HSDS topics and the Reader must be able to read from all of the HSDS topics.

## Building

1. Build OpenDDS with security enabled.
2. Let `%%DDS_ROOT%%` be the directory for OpenDDS.
3. Let `%%COMMUNITY_UTILS_ROOT%%` be the root directory of this project.
4. In the root directory of this project, define a `setenv.sh` file subsituting `%%DDS_ROOT%%` and `%%COMMUNITY_UTILS_ROOT%%`

        #!/bin/bash

        source "%%DDS_ROOT%%"

        export COMMUNITY_UTILS_ROOT="%%COMMUNITY_UTILS_ROOT%%"

        export DYLD_LIBRARY_PATH="${DYLD_LIBRARY_PATH}:${COMMUNITY_UTILS_ROOT}/CommunitySecurityPlugin:${COMMUNITY_UTILS_ROOT}/HSDS/Common"
        export LD_LIBRARY_PATH="${DYLD_LIBRARY_PATH}:${COMMUNITY_UTILS_ROOT}/CommunitySecurityPlugin:${COMMUNITY_UTILS_ROOT}/HSDS/Common"

5. In the root directory of this project

        source setenv.sh
        mwc.pl -type gnuace HSDS.mwc -include MPC/config
        make
        
## General Configuration

Configuration options common to all of the utilities can be added to the `setenv.sh` in the previous section.
This includes the names of the topics, the URL of the DPM, the DPM group id for all of the applications, and the URL of the RtpsRelay.
If the DPM applications are not in the same DPM group, then the DPM group id can be omitted and set for each application.
Example:

        export ACCESSIBILITY_TOPIC="B.54.HSDS3 Accessibility"
        export ADDRESS_TOPIC="B.54.HSDS3 Address"
        export ATTRIBUTE_TOPIC="B.54.HSDS3 Attribute"
        export CONTACT_TOPIC="B.54.HSDS3 Contact"
        export COST_OPTION_TOPIC="B.54.HSDS3 Cost Option"
        export FUNDING_TOPIC="B.54.HSDS3 Funding"
        export LANGUAGE_TOPIC="B.54.HSDS3 Language"
        export LOCATION_TOPIC="B.54.HSDS3 Location"
        export META_TABLE_DESCRIPTION_TOPIC="B.54.HSDS3 Meta Table Description"
        export METADATA_TOPIC="B.54.HSDS3 Metadata"
        export ORGANIZATION_TOPIC="B.54.HSDS3 Organization"
        export ORGANIZATION_IDENTIFIER_TOPIC="B.54.HSDS3 Organization Identifier"
        export PHONE_TOPIC="B.54.HSDS3 Phone"
        export PROGRAM_TOPIC="B.54.HSDS3 Program"
        export REQUIRED_DOCUMENT_TOPIC="B.54.HSDS3 Required Document"
        export SCHEDULE_TOPIC="B.54.HSDS3 Schedule"
        export SERVICE_TOPIC="B.54.HSDS3 Service"
        export SERVICE_AREA_TOPIC="B.54.HSDS3 Service Area"
        export SERVICE_AT_LOCATION_TOPIC="B.54.HSDS3 Service At Location"
        export TAXONOMY_TOPIC="B.54.HSDS3 Taxonomy"
        export TAXONOMY_TERM_TOPIC="B.54.HSDS3 Taxonomy Term"

        export DPM_URL=https://dpm.unityfoundation.io
        export DPM_GID=54
        export RELAY_CONFIG_URL=http://34.66.84.199/config

Copyright 2023 CommunityUtils Authors
