#!/bin/bash 
set -e # If any command fails, the script will stop immediately 
VERSION=$1 # Store first command line argument as VERSION
git tag $VERSION # Tag version
git push --tags # Push version to online repo
URL="https://github.com/evanl23/geometry-dash-terminal/archive/refs/tags/$VERSION.tar.gz" # Construct GitHub URL
curl -L -o tmp.tar.gz "$URL" # Download .tar.gz from GitHub to local machine
SHA=$(shasum -a 256 tmp.tar.gz | awk '{print $1}') # Extract SHA256, extract just the hash, store in SHA
echo "URL: $URL" # Print URL
echo "SHA256: $SHA" # Print hash