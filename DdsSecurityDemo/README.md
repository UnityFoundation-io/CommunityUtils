# DDS Security Demo

The `demo.sh` shows how to
* bootstrap an Identity CA
* bootstrap a Permissions CA
* sign a governance file
* create keys and certificates
* sign permissions files

## Uploading to GCP Secret Manager

The Identity CA (key and certificate), Permissions CA (key and certificate), and signed governance file must be available to the DDS Permissions Manager.
One way to do this is with GCP Secret Manager.
The commands for creating the secrets are:

    gcloud secrets create identity_ca_key_pem --data-file=identity_ca_key.pem
    gcloud secrets create identity_ca_pem --data-file=identity_ca.pem
    gcloud secrets create permissions_ca_key_pem --data-file=permissions_ca_key.pem
    gcloud secrets create permissions_ca_pem --data-file=permissions_ca.pem
    gcloud secrets create governance_xml_p7s --data-file=governance.xml.p7s

The secrets can be updated by the following commands:

    gcloud secrets versions add identity_ca_key_pem --data-file=identity_ca_key.pem
    gcloud secrets versions add identity_ca_pem --data-file=identity_ca.pem
    gcloud secrets versions add permissions_ca_key_pem --data-file=permissions_ca_key.pem
    gcloud secrets versions add permissions_ca_pem --data-file=permissions_ca.pem
    gcloud secrets versions add governance_xml_p7s --data-file=governance.xml.p7s

Copyright 2023 CommunityUtils Authors
