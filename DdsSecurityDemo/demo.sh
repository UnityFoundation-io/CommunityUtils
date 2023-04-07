#!/bin/bash
# Copyright 2023 CommunityUtils Authors

####################################################################################################
# The following steps are not performed in the DDS Permissions Manager but are here for reference. #
####################################################################################################

echo -n '' > index.txt
echo -n '' > index.txt.attr
echo -n '01' > serial

# Generate a self-signed certificate for the identity CA.
openssl ecparam -name prime256v1 -genkey -out identity_ca_key.pem
openssl req -config ./identity.cnf -days 3650 -new -x509 -extensions v3_ca -key identity_ca_key.pem -out identity_ca.pem

echo "Identity CA key in identity_ca_key.pem"
echo "Identity CA certificate in identity_ca.pem"

# Generate a self-signed certificate for the permissions CA.
openssl ecparam -name prime256v1 -genkey -out permissions_ca_key.pem
openssl req -config ./permissions.cnf -days 3650 -new -x509 -extensions v3_ca -key permissions_ca_key.pem -out permissions_ca.pem

echo "Permissions CA key in permissions_ca_key.pem"
echo "Permissions CA certificate in permissions_ca.pem"

# Sign the governance file with the permissions CA.
openssl smime -sign -in governance.xml -text -out governance.xml.p7s -signer permissions_ca.pem -inkey permissions_ca_key.pem

echo "Signed governance file in governance.xml.p7s"

#########################################################################
# The following steps will be performed in the DDS Permissions Manager. #
#########################################################################

# Generate a client certificate with using the identity CA.
openssl ecparam -name prime256v1 -genkey -out participant_key.pem
openssl req -new -key participant_key.pem -out participant.csr -subj "/C=US/ST=MO/L=Saint Louis/O=UNITY/CN=Test Participant"
openssl ca -batch -config identity.cnf -days 3650 -in participant.csr -out participant.pem

echo "Participant key in participant_key.pem"
echo "Participant certificate in participant.pem"

# Sign a permissions file with the permissions CA.
openssl smime -sign -in permissions.xml -text -out permissions.xml.p7s -signer permissions_ca.pem -inkey permissions_ca_key.pem

echo "Signed permissions file in permissions.xml.p7s"
