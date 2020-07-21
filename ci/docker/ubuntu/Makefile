IMAGE ?= registry.gitlab.com/rpdev/opentodolist:ubuntu
help:
	@echo "Targets:"
	@echo "  build - Build the image."
	@echo "  publish - Upload the image to GitLab."

build:
	./run-build.sh ${IMAGE}

publish:
	podman push docker://${IMAGE}

login:
	podman login registry.gitlab.com
