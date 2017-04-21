REGISTRY ?= registry.gitlab.com/rpdev/opentodolist
IMAGE := ${REGISTRY}:fedora

help:
    @echo "Targets:"
    @echo "    build   - build Docker image"

build:
    docker build --pull -t ${IMAGE} .

push:
    docker push ${IMAGE}
