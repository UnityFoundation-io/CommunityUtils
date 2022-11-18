# Time Demo for CommUNITY

This directory contains a demo for CommUNITY.
The TimeWriter program writes samples containing the current time at a 1 second interval.
The TimeReader program reads and prints the samples.

## Building

1. Build the CommunitySecurityPlugin.
2. Create a `setenv.sh` that sources the `setenv.sh` from the CommunitySecurityPlugin.
3. Add and export a variable `COMMUNITY_SECURITY_PLUGIN_ROOT` that is the path to the CommunitySecurityPlugin.
4. Add and export a variable `DPM_URL` that is the url of the DDS Permissions Manager.
5. `source setenv.sh`
6. `mwc.pl -type gnuace -include "${COMMUNITY_SECURITY_PLUGIN_ROOT}"`
7. `make`

## Set up

In the DDS Permissions Manager

1. Create an application for the writer.  Generate credentials and save the passphrase.
2. Create an application for the reader.  Generate credentials and save the passphrase.
3. Create a topic.
4. Grant write access on the topic to the writer.
5. Grant read access on the topic to the reader.

## Run-time Dependencies

* curl
* jq

## Running the Writer

The `time_writer.sh` script downloads DDS Security Documents from the DDS Permissions Manager and creates a DDS Participant that writes time samples.

1. `source setenv.sh`
2. `./time_writer.sh USERNAME PASSWORD TOPIC DPMGID`
   1. USERNAME is the username of the application from Set up Step 1 (see the application detail screen).
   2. PASSWORD is the password for the application from Set up Step 1.
   3. TOPIC is the canonical topic name (see the application detail screen).
   4. DPMGID is the group id of writer application (see the application detail screen).

## Running the Reader

The `time_reader.sh` script downloads DDS Security Documents from the DDS Permissions Manager and creates a DDS Participant that reads time samples.

1. `source setenv.sh`
2. `./time_reader.sh USERNAME PASSWORD TOPIC`
   1. USERNAME is the username of the application from Set up Step 2 (see the application detail screen).
   2. PASSWORD is the password for the application from Set up Step 2.
   3. TOPIC is the canonical topic name (see the application detail screen).
