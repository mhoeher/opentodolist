IMAGE ?= registry.gitlab.com/rpdev/opentodolist:flatpak
help:
	@echo "Targets:"
	@echo "  build - Build the image."
	@echo "  publish - Upload the image to GitLab."

build:
	podman build --pull --no-cache -t ${IMAGE} .

publish:
	podman push docker://${IMAGE}

login:
	podman login registry.gitlab.com
