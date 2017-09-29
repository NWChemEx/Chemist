#!/bin/bash -e

#To use this script to auto publish documentation to your project's GitHub 
#branch do the following:
#
# 1. generate RSA key pair in the top-level directory of your project (leave
#    passphrase blank as this will be done non-interactively)
#    - ssh-keygen -t rsa -C "email_goes_here" -f bin/travisci_rsa
# 2. Upload the private, bin/travisci_rsa, to Travis-CI
#    - The best way to do this is through Travis' command-line utility
#    - sudo gem install travis
#    - travis login
#    - travis encrypt-file bin/travisci_rsa --add
#    - rm bin/travisci_rsa (we are going to upload the encrypted version)
#    - mv travisci_rsa.enc bin (I don't know why it doesn't put it there...)
#    - modify the lines Travis added to .travis.yml with *.enc 's new path
# 3. Make sure this script's HTML_PATH matches doxygen's output path
# 4. Make sure the Travis file has a line:
#    after_success:
#    - ./config/publish-doxygen
# 5. 

# Settings
REPO_PATH=git@github.com:pulsar-chem/LibChemist.git
HTML_PATH=doc/html
COMMIT_USER="Documentation Builder"
CHANGESET=$(git rev-parse --verify HEAD)

# Get a clean version of the HTML documentation repo.
rm -rf ${HTML_PATH}
mkdir -p ${HTML_PATH}
git clone -b gh-pages "${REPO_PATH}" --single-branch ${HTML_PATH}

# rm all the files through git to prevent stale files.
cd ${HTML_PATH}
git rm -rf .
cd -

# Generate the HTML documentation.
doxygen bin/Doxyfile

# Create and commit the documentation repo.
cd ${HTML_PATH}
git add .
git config user.name "${COMMIT_USER}"
git config user.email "<>"
git commit -m "Automated documentation build for changeset ${CHANGESET}."
git push origin gh-pages
cd -
