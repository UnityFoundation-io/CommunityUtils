# CommunitySecurityPlugin

Using OpenDDS, CommUNITY will create a shared dataspace across multiple organizations.
Intentionally, different organizations will write samples on the same topic.
This creates an opportunity for one organization to overwrite samples written by another application.
The purpose of this library is to install security controls that prevent this from happening.

First, we define a DDS Permissions Manager Group ID (DMPGID).
This value will appear in the subject name of certificate as a member named `DMPGID`.
Second, we require that every sample have key field named `dpmgid` that also contains the DPMGID.
The security plugin checks that these two values are the sample.

The library contains a single function to install the plugin.
The plugin should be installed before any participant is created.

The IDL for the samples must be compiled with complete type object support.

    dcps_ts_flags += -Gxtypes-complete