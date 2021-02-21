# Reverse Osmosis Controller Web App

## Description
The root contains the back-end application which talks directly to the database. It serves as an API for the front-end to make requests.

## How to run
At the moment, there is an issue running docker-compose build on Raspberry Pi 3 arm64 architecture (e.g. Running Ubuntu 18.04). Workaround described below.
On a amd64 architecture, simply create a .env (dotenv) file with the required variables (shown below) followed by 'docker-compose up' and you're good to go.

### arm64 specfic
I use docker buildx to build the frontend/backend images for arm64 on an amd64 platform and push them to docker hub. Then, modify the docker-compose.yml to
not build the images locally by commenting out the 'build:' statements. Then create the .env file and run docker-compose up like normal and it should 
automatically pull the latest images from docker hub.

## Building Notes -- Frontend
Environment variables to be used by Vue locally at runtime must be present in a .env file inside ./client at build time. They are embedded into the static
files. I'll need to figure out a way to not hide secrets there, but for now the API secret is stashed there. Once a .env is present in the client folder
with the required variables (noted below), you can build the frontend normally.

# .ENV Required Variables
## Required for Docker-Compose, runtime ENV
PARTICLE_USERNAME= # Username for particle cloud
PARTICLE_PASSWORD= # Password for particle cloud

API_SECRET= # Any random string, must match VUE_APP_API_SECRET below

DOCKER_USERNAME= # Used to name the images on the docker repository

FRONTEND_PORT= # The port to host the front-end on, usually 80
BACKEND_PORT= # Any available port, must match VUE_APP_API_PORT below

## Required for Frontend, at buildtime
VUE_APP_API_SECRET= # Must equal API_SECRET above
VUE_APP_API_PORT= # Must equal BACKEND_PORT above